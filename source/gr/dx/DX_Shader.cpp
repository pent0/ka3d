#include <gr/dx/DX_common.h>
#include <gr/dx/DX_Shader.h>
#include <gr/dx/DX_IncludeManager.h>
#include <gr/dx/DX_Context.h>
#include <gr/dx/DX_Texture.h>
#include <gr/dx/DX_CubeTexture.h>
#include <gr/dx/DX_GraphicsException.h>
#include <gr/dx/DX_helpers.h>
#include <io/PathName.h>
#include <io/FileInputStream.h>
#include <lang/String.h>
#include <lang/Profile.h>
#include <math/float4x4.h>
#include <d3dx9effect.h>
#include <config.h>


USING_NAMESPACE(io)
USING_NAMESPACE(lang)
USING_NAMESPACE(math)


// if any of these is present, it must be set
static const char* const REQUIRED_PARAMETER_LIST[] =
{
	"BASEMAP",
	"LIGHTMAP",
	"REFLMAP",
	"NORMALMAP",
	"AMBIENTC",
	"DIFFUSEC",
	"SPECULARC"
	"SHININESS",
	"CAMERAP",
	"WORLDTM",
	"VIEWTM",
	"VIEWTMINV",
	"VIEWPROJTM",
	"TOTALTM",
	"BONEWORLDTM",
	"LIGHTP0",
	"LIGHTC0",
};

static const int REQUIRED_PARAMETERS = int( sizeof(REQUIRED_PARAMETER_LIST) / sizeof(REQUIRED_PARAMETER_LIST[0]) );


BEGIN_NAMESPACE(gr) 


DX_Shader::DX_Shader( DX_Context* context, const String& name, 
	const String& fxfilename, int flags ) :
	DX_ContextItem( context, CLASSID_SHADER ),
	m_name( name ),
	m_filename( fxfilename ),
	m_fx( 0 ),
	m_requiredParams( 0 ),
	m_priority( 0 ),
	m_sort( SORT_NONE ),
	m_enabled( true )
{
	// pixel shader version string
	char psver[16] = "PS_1_X";
	if ( context->shaderHardware() ) psver[3] = char((context->caps().PixelShaderVersion>>8)&0xF) + '0';

	// prepare conditional compilation
	D3DXMACRO deflist[] =
	{
		{ "ENGINE_RUNTIME", "" },
		{ (flags&SHADER_LIGHTMAPPING) != 0 ? "ENABLE_LIGHTMAPPING" : "DISABLE_LIGHTMAPPING", "" },
		{ psver, "" },
		{ 0, 0 }
	};

	// get d3d device
	IDirect3DDevice9* dev = context->device();
	DWORD d3dflags = D3DXSHADER_PACKMATRIX_COLUMNMAJOR;
	LPD3DXBUFFER errors = 0;

	// compite shader
	HRESULT hr = S_OK;
	if ( context->debugShaders() )
	{
		d3dflags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT | D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
		hr = DX_TRY( D3DXCreateEffectFromFile(dev, fxfilename.c_str(), deflist, 0, d3dflags, 0, &m_fx, &errors) );
	}
	else
	{
		Array<char> buf;
		FileInputStream fin( fxfilename );
		buf.resize( fin.available() );
		fin.read( buf.begin(), buf.size() );

		DX_IncludeManager incl( PathName(fxfilename).parent() );
		hr = DX_TRY( D3DXCreateEffect(dev, buf.begin(), buf.size(), deflist, &incl, d3dflags, 0, &m_fx, &errors) );
	}
	DX_AutoRelease autorel( m_fx );

	// process errors
	const int MAX_ERR = 500;
	char errdesc[MAX_ERR+1] = {0};
	if ( 0 != errors )
	{
		int count = errors->GetBufferSize();
		if ( count > MAX_ERR )
			count = MAX_ERR;
		memcpy( errdesc, errors->GetBufferPointer(), count );
		errdesc[count] = 0;
		DX_release( errors );
	}

	if ( D3D_OK != hr )
		throwError( DX_GraphicsException( Format("Failed to init \"{0}\" {1} ({2})", fxfilename, errdesc, gr::toString(hr)) ) );

	validateHandles();

	autorel.cancel(); // init ok so do not release the shader
}

DX_Shader::DX_Shader( const DX_Shader& other ) :
	DX_ContextItem( other ),
	m_name( other.m_name ),
	m_filename( other.m_filename ),
	m_fx( 0 ),
	m_requiredParams( other.m_requiredParams ),
	m_priority( other.m_priority ),
	m_sort( other.m_sort ),
	m_enabled( other.m_enabled )
{
	if ( other.m_fx != 0 )
		other.m_fx->CloneEffect( m_context->device(), &m_fx );

	validateHandles();

	m_requiredParams = other.m_requiredParams;
}

DX_Shader::~DX_Shader()
{
	DX_release( m_fx );
}

Shader* DX_Shader::clone() const
{
	return new DX_Shader( *this );
}

void DX_Shader::setTechnique( const char* name )
{
	// set requested technique
	D3DXHANDLE tech = 0;
	if ( name != 0 )
	{
		tech = m_fx->GetTechniqueByName( name );
		if ( 0 == tech )
		{
			// technique not supported, disable shader and return
			m_enabled = false;
			return;
		}
		DX_TRY( m_fx->SetTechnique( tech ) );
	}
	else
	{
		DX_TRY( m_fx->FindNextValidTechnique( 0, &tech ) );
		DX_TRY( m_fx->SetTechnique( tech ) );
	}
	m_enabled = true;
}

int DX_Shader::priority() const
{
	return m_priority;
}

DX_Shader::SortType DX_Shader::sort() const
{
	return m_sort;
}

bool DX_Shader::enabled() const
{
	return m_enabled;
}

D3DXHANDLE DX_Shader::getHandle( const char* name )
{
	D3DXHANDLE handle = m_fx->GetParameterByName( 0, name );

	if ( handle != 0 && m_requiredParams != 0 )
	{
		for ( int i = 0 ; i < REQUIRED_PARAMETERS ; ++i )
		{
			if ( !strcmp( REQUIRED_PARAMETER_LIST[i], name ) )
			{
				m_requiredParams &= ~(1<<i);
				break;
			}
		}
	}

	return handle;
}

inline D3DXHANDLE DX_Shader::getHandle( ParamType param )
{
	if ( m_requiredParams != 0 )
		return getHandle( Shader::toString(param) );
	return m_params[param];
}

void DX_Shader::setMatrixArray( ParamType param, float4x4** value, int count )
{
	if ( SORT_NONE != m_sort && PARAM_BONEWORLDTM == param )
	{
		if ( SORT_INSIDEOUT != m_sort )
		{
			m_context->boneworldtm.resize( count );
			for ( int i = 0 ; i < m_context->boneworldtm.size() ; ++i )
				m_context->boneworldtm[i] = *value[i];
		}
	}

	D3DXHANDLE handle = getHandle(param);
	if ( handle != 0 )
		DX_TRY( m_fx->SetMatrixPointerArray(handle, (const D3DXMATRIX**)(value), count) );
}

void DX_Shader::setTexture( ParamType param, BaseTexture* value )
{
	D3DXHANDLE handle = getHandle(param);
	if ( handle != 0 )
	{
		if ( value->classId() == CLASSID_TEXTURE )
		{
			DX_Texture* v = static_cast<DX_Texture*>( value );
			DX_TRY( m_fx->SetTexture(handle,v->texture()) );
		}
		else if ( value->classId() == CLASSID_CUBETEXTURE )
		{
			DX_CubeTexture* v = static_cast<DX_CubeTexture*>( value );
			DX_TRY( m_fx->SetTexture(handle,v->texture()) );
		}
	}
}

void DX_Shader::setMatrix( ParamType param, const float4x4& value )
{
	if ( SORT_NONE != m_sort )
	{
		if ( PARAM_WORLDTM == param )
			m_context->worldtm = value;
	}

	D3DXHANDLE handle = getHandle(param);
	if ( handle != 0 )
	{
		D3DXMATRIX m;
		for ( int i = 0 ; i < 4 ; ++i )
		{
			m(i,0) = value(0,i);
			m(i,1) = value(1,i);
			m(i,2) = value(2,i);
			m(i,3) = value(3,i);
		}
		DX_TRY( m_fx->SetMatrix(handle, &m ) );
	}
}

void DX_Shader::setTexture( const char* name, BaseTexture* value )
{
	D3DXHANDLE handle = getHandle( name );
	if ( handle != 0 )
	{
		if ( value != 0 )
		{
			DX_Texture* v = static_cast<DX_Texture*>( value );
			DX_TRY( m_fx->SetTexture(handle,v->texture()) );
		}
		else
		{
			DX_TRY( m_fx->SetTexture(handle,0) );
		}
	}
}

void DX_Shader::setMatrix( const char* name, const float4x4& value )
{
	D3DXHANDLE handle = getHandle( name );
	if ( handle != 0 )
		DX_TRY( m_fx->SetMatrix(handle, (const D3DXMATRIX*)&value) );
}

void DX_Shader::setVector( ParamType param, const float4& value )
{
	if ( SORT_NONE != m_sort )
	{
		if ( PARAM_CAMERAP == param )
			m_context->camerapos = value;
	}

	D3DXHANDLE handle = getHandle(param);
	if ( handle != 0 )
		DX_TRY( m_fx->SetVector(handle, (const D3DXVECTOR4*)&value) );
}

void DX_Shader::setVector( const char* name, const float4& value )
{
	D3DXHANDLE handle = getHandle( name );
	if ( handle != 0 )
		DX_TRY( m_fx->SetVector(handle, (const D3DXVECTOR4*)&value) );
}

void DX_Shader::setFloat( ParamType param, float value )
{
	D3DXHANDLE handle = getHandle(param);
	if ( handle != 0 )
		DX_TRY( m_fx->SetFloat(handle, value) );
}

BaseTexture* DX_Shader::getTexture( const char* name )
{
	BaseTexture* foundtex = 0;
	D3DXHANDLE handle = getHandle( name );

	if ( handle != 0 )
	{
		IDirect3DBaseTexture9* d3dbasetex = 0;
		HRESULT hr = m_fx->GetTexture( handle, &d3dbasetex );
		if ( hr != D3D_OK )
			return 0;

		// 2D texture?
		IDirect3DTexture9* d3dtex = 0;
		hr = d3dbasetex->QueryInterface( IID_IDirect3DTexture9, (void**)&d3dtex );
		if ( S_OK == hr )
		{
			for ( DX_ContextItem* it = m_context->itemlist.first() ; it != 0 ; it = it->next() )
			{
				if ( it->isType(CLASSID_TEXTURE) )
				{
					DX_Texture* tex = static_cast<DX_Texture*>(it);
					if ( tex->texture() == d3dtex )
					{
						foundtex = tex;
						break;
					}
				}
			}

			DX_release( d3dtex );
			DX_release( d3dbasetex );
			return foundtex;
		}

		// cube texture?
		IDirect3DCubeTexture9* d3dcubetex = 0;
		hr = d3dbasetex->QueryInterface( IID_IDirect3DCubeTexture9, (void**)&d3dcubetex );
		if ( S_OK == hr )
		{
			for ( DX_ContextItem* it = m_context->itemlist.first() ; it != 0 ; it = it->next() )
			{
				if ( it->isType(CLASSID_CUBETEXTURE) )
				{
					DX_CubeTexture* tex = static_cast<DX_CubeTexture*>(it);
					if ( tex->texture() == d3dcubetex )
					{
						foundtex = tex;
						break;
					}
				}
			}

			DX_release( d3dcubetex );
			DX_release( d3dbasetex );
			return foundtex;
		}
	}
	return foundtex;
}

float4x4 DX_Shader::getMatrix( const char* name )
{
	float4x4 value( 0.f );
	D3DXHANDLE handle = getHandle( name );
	if ( handle != 0 )
		DX_TRY( m_fx->GetMatrix(handle, (D3DXMATRIX*)&value) );
	return value;
}

float4 DX_Shader::getVector( const char* name )
{
	float4 value( 0, 0, 0, 0 );
	D3DXHANDLE handle = getHandle( name );
	if ( handle != 0 )
		DX_TRY( m_fx->GetVector(handle, (D3DXVECTOR4*)&value) );
	return value;
}

float DX_Shader::getFloat( const char* name )
{
	float value( 0.f );
	D3DXHANDLE handle = getHandle( name );
	if ( handle != 0 )
		DX_TRY( m_fx->GetFloat(handle, &value) );
	return value;
}

void DX_Shader::setFloat( const char* name, float value )
{
	D3DXHANDLE handle = getHandle( name );
	if ( handle != 0 )
		DX_TRY( m_fx->SetFloat(handle, value) );
}

int DX_Shader::begin()
{
	if ( !m_enabled )
		return 0;

	m_context->setSort( m_sort );

	UINT passes = 0;
	DX_TRY( m_fx->Begin(&passes,0) );
	return passes;
}

void DX_Shader::beginPass( int pass )
{
	assert( m_enabled );

	if ( m_requiredParams != 0 )
	{
		for ( int i = 0 ; i < REQUIRED_PARAMETERS ; ++i )
			if ( m_requiredParams & (1<<i) )
				throwError( DX_GraphicsException( Format("Shader parameter {0} required by shader \"{1}\" (used by material \"{2}\")", REQUIRED_PARAMETER_LIST[i], m_filename, m_name) ) );
		assert( m_requiredParams == 0 );
	}

	DX_TRY( m_fx->BeginPass(pass) );
}

void DX_Shader::endPass()
{
	assert( m_enabled );
	DX_TRY( m_fx->EndPass() );
}

void DX_Shader::end()
{
	if ( !m_enabled )
		return;

	DX_TRY( m_fx->End() );
}

void DX_Shader::setName( const String& name )
{
	m_name = name;
}

void DX_Shader::setSort( SortType sort )
{
	m_sort = sort;
}

void DX_Shader::deviceLost()
{
	DX_TRY( m_fx->OnLostDevice() );
}

void DX_Shader::deviceReset()
{
	DX_TRY( m_fx->OnResetDevice() );
}

const String& DX_Shader::name() const
{
	return m_name;
}

void DX_Shader::validateHandles()
{
	setTechnique( 0 );

	// find out required parameters
	assert( REQUIRED_PARAMETERS < 32 );
	m_requiredParams = 0;
	for ( int i = 0 ; i < REQUIRED_PARAMETERS ; ++i )
	{
		D3DXHANDLE handle = m_fx->GetParameterByName( 0, REQUIRED_PARAMETER_LIST[i] );
		if ( handle != 0 )
			m_requiredParams += (1<<i);
	}

	// get shader PRIORITY if any
	m_priority = 0;
	D3DXHANDLE handle = m_fx->GetParameterByName( 0, "PRIORITY" );
	if ( 0 != handle )
	{
		INT priority = 0;
		HRESULT hr = DX_TRY(m_fx->GetInt(handle,&priority));
		if ( D3D_OK != hr )
			throwError( DX_GraphicsException( Format("Failed to init \"{0}\", PRIORITY is not integer", m_filename, priority) ) );
		if ( priority < -100 || priority > 100 )
			throwError( DX_GraphicsException( Format("Failed to init \"{0}\", invalid PRIORITY ({1}, should be from -100 to 100)", m_filename, priority) ) );
		m_priority = (int8_t)priority;
	}

	// get shader SORT type if any
	m_sort = SORT_NONE;
	handle = m_fx->GetParameterByName( 0, "SORT" );
	if ( 0 != handle )
	{
		LPCSTR sort = 0;
		HRESULT hr = DX_TRY(m_fx->GetString(handle,&sort));
		if ( hr == D3D_OK && sort != 0 )
		{
			if ( !strcmp(sort,"NONE") )
				m_sort = SORT_NONE;
			else if ( !strcmp(sort,"BACKTOFRONT") )
				m_sort = SORT_BACKTOFRONT;
			else if ( !strcmp(sort,"FRONTTOBACK") )
				m_sort = SORT_FRONTTOBACK;
			else if ( !strcmp(sort,"INSIDEOUT") )
				m_sort = SORT_INSIDEOUT;
			else if ( !strcmp(sort,"OUTSIDEIN") )
				m_sort = SORT_OUTSIDEIN;
			else
			{
				String sortstr = sort;
				DX_release( m_fx );
				throwError( DX_GraphicsException( Format("Failed to init \"{0}\", invalid SORT annotation: {1}", m_filename, sortstr) ) );
			}
		}
	}

	assert( m_fx != 0 );
	for ( int i = 0 ; i < PARAM_COUNT ; ++i )
		m_params[i] = m_fx->GetParameterByName( 0, Shader::toString((ParamType)i) );
}

String DX_Shader::toString() const
{
	return Format("Shader( \"{0}\" )", m_name).format();
}


END_NAMESPACE() // gr

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
