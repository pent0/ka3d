#include <hgr/Lines.h>
#include <gr/Context.h>
#include <gr/Primitive.h>
#include <gr/VertexFormat.h>
#include <hgr/Camera.h>
#include <lang/Math.h>
#include <lang/Debug.h>
#include <math/float4x4.h>
#include <config.h>


USING_NAMESPACE(gr)
USING_NAMESPACE(lang)
USING_NAMESPACE(math)


BEGIN_NAMESPACE(hgr) 


Lines::Lines( Context* context, const String& shadername ) :
	m_shader(0),
	m_lines(),
	m_paths(),
	m_maxLines(0)
{
	setClassId( NODE_LINES );

	// we allow non-renderable line creation as well
	// since not all line collections need to be rendered
	if ( context && context->platform() != Context::PLATFORM_SW )
		m_shader = context->createShader( shadername );
	else
		setEnabled( false );
}

Lines::~Lines()
{
}

Lines::Lines( const Lines& other ) :
	Visual( other ),
	m_shader( other.m_shader ),
	m_lines( other.m_lines ),
	m_paths( other.m_paths ),
	m_maxLines( other.m_maxLines )
{
}

Node* Lines::clone() const
{
	return new Lines( *this );
}

void Lines::reserve( int lines, int paths )
{
	m_maxLines = lines;
	m_lines.resize( lines );
	m_lines.clear();
	m_paths.resize( paths );
	m_paths.clear();
}

void Lines::removeLines()
{
	m_lines.clear();
	m_paths.clear();
}

void Lines::addLine( const float3& startpoint, const float3& endpoint, const float4& color )
{
	addLine( startpoint, endpoint, color, color );
}

void Lines::addLine( const float3& startpoint, const float3& endpoint,
	const float4& startcolor, const float4& endcolor )
{
	Line line;
	line.start = startpoint;
	line.end = endpoint;
	line.startcolor = startcolor;
	line.endcolor = endcolor;
	m_lines.add( line );
}

void Lines::addPath( int begin, int end )
{
	Path path;
	path.begin = begin;
	path.end = end;
	m_paths.add( path );
}

int	Lines::lines() const
{
	return m_lines.size();
}

int	Lines::paths() const
{
	return m_paths.size();
}

void Lines::render( Context* context, Camera* camera, int priority )
{
	if ( !m_shader )
		return;

	assert( m_shader ); // line set was created without context -> cannot be rendered

	if ( m_lines.size() > 0 && priority == m_shader->priority() )
	{
#if !defined(PLATFORM_NGI) && !defined(PLATFORM_BREW)
		float4x4 totaltm = camera->cachedViewProjectionTransform() * camera->getCachedWorldTransform(this);
#endif
		const float4x4& viewtm = camera->cachedViewTransform();
		const float4x4& worldtm = camera->getCachedWorldTransform(this);

		if ( m_lines.size() > m_maxLines )
			m_maxLines = m_lines.size();

		VertexFormat vf;
		vf.addPosition().addDiffuse( VertexFormat::DF_V4_8 );
		P(Primitive) lines = context->getDynamicPrimitive( Primitive::PRIM_LINE, vf, m_maxLines*2, 0 );
		lines->setVertexRangeBegin( 0 );
		lines->setVertexRangeEnd( m_lines.size()*2 );
		lines->setShader( m_shader );

		{
			Primitive::Lock lk( lines, Primitive::LOCK_WRITE );
			float4 v[2];
			for ( int i = 0 ; i < m_lines.size() ; ++i )
			{
				v[0] = float4( m_lines[i].start, 1 );
				v[1] = float4( m_lines[i].end, 1 );
				lines->setVertexPositions( i+i, v, 2 );

				const float4& c0 = m_lines[i].startcolor;
				const float4& c1 = m_lines[i].endcolor;
				v[0] = float4(c0.z,c0.y,c0.x,c0.w) * 255.f;
				v[1] = float4(c1.z,c1.y,c1.x,c1.w) * 255.f;
				lines->setVertexDiffuseColors( i+i, v, 2 );
			}
		}

		Shader::Begin use( m_shader );
		if ( use.passes() > 0 )
		{
#if !defined(PLATFORM_NGI) && !defined(PLATFORM_BREW)
			m_shader->setMatrix( Shader::PARAM_TOTALTM, totaltm );
#endif
			m_shader->setMatrix( Shader::PARAM_WORLDTM, worldtm );
			m_shader->setMatrix( Shader::PARAM_VIEWTM, viewtm );

			for ( int k = 0 ; k < use.passes() ; ++k )
			{
				Shader::Pass pass( m_shader, k );
				lines->render();
			}
		}
	}
}

void Lines::getShaders( Array<Shader*>& shaders )
{
	if ( !m_shader )
		return;

	assert( m_shader ); // line set was created without context -> cannot be rendered
	shaders.add( m_shader );
}

void Lines::computeBound()
{
	float3 minv( MAX_BOUND, MAX_BOUND, MAX_BOUND );
	float3 maxv( -MAX_BOUND, -MAX_BOUND, -MAX_BOUND );

	for ( int i = 0 ; i < m_lines.size() ; ++i )
	{
		const float3& v0 = m_lines[i].start;
		const float3& v1 = m_lines[i].end;

		minv.x = Math::min( v0.x, minv.x );
		minv.y = Math::min( v0.y, minv.y );
		minv.z = Math::min( v0.z, minv.z );
		maxv.x = Math::max( v0.x, maxv.x );
		maxv.y = Math::max( v0.y, maxv.y );
		maxv.z = Math::max( v0.z, maxv.z );

		minv.x = Math::min( v1.x, minv.x );
		minv.y = Math::min( v1.y, minv.y );
		minv.z = Math::min( v1.z, minv.z );
		maxv.x = Math::max( v1.x, maxv.x );
		maxv.y = Math::max( v1.y, maxv.y );
		maxv.z = Math::max( v1.z, maxv.z );
	}

	setBoundBox( minv, maxv );
}

void Lines::addBox( const float3x4& tm, 
	const float3& boxmin, const float3& boxmax,
	const float4& color )
{
	float3 x = tm.getColumn(0) * boxmax.x;
	float3 y = tm.getColumn(1) * boxmax.y;
	float3 z = tm.getColumn(2) * boxmax.z;
	float3 xn = tm.getColumn(0) * boxmin.x;
	float3 yn = tm.getColumn(1) * boxmin.y;
	float3 zn = tm.getColumn(2) * boxmin.z;
	float3 p = tm.getColumn(3);
	
	const float3 points[] = 
	{
		p+xn+y+zn, p+x+y+zn, p+x+yn+zn, p+xn+yn+zn,
		p+xn+y+z, p+x+y+z, p+x+yn+z, p+xn+yn+z,
	};

	const int polys[] =
	{
		0,1, 1,2, 2,3, 3,0,
		4,5, 5,6, 6,7, 7,4,
		0,4, 1,5, 2,6, 3,7,
	};
	const int INDICES = sizeof(polys)/sizeof(polys[0]);

	for ( int i = 0 ; i < INDICES ; i += 2 )
		addLine( points[polys[i]], points[polys[i+1]], color );
}

void Lines::addSphere( const NS(math,float3x4)& tm, float radius,
	const float4& color, int lines )
{
	assert( lines >= 8 );

	const int halflines = lines / 2;
	const float da = (Math::PI*2.f) / float(halflines);
	float a0 = 0.f;
	float a1 = da;
	float3 points[4];

	for ( int i = 0 ; i < halflines ; ++i )
	{
		points[0] = float3( Math::cos(a0)*radius, Math::sin(a0)*radius, 0.f ),
		points[1] = float3( Math::cos(a1)*radius, Math::sin(a1)*radius, 0.f ),
		points[2] = float3( 0.f, points[0].y, points[0].x ),
		points[3] = float3( 0.f, points[1].y, points[1].x );

		for ( int k = 0 ; k < 4 ; ++k )
			points[k] = tm.transform( points[k] );
		
		addLine( points[0], points[1], color );
		addLine( points[2], points[3], color );

		a0 += da;
		a1 += da;
	}
}

void Lines::addCylinder( const NS(math,float3x4)& tm, float radius, float height,
	const NS(math,float4)& color, int segments )
{
	assert( segments >= 4 );

	const float da = (Math::PI*2.f) / float(segments);
	float a = 0.f;
	
	for ( int i = 0 ; i < segments ; ++i )
	{
		float3 p0 = tm.transform( float3( Math::cos(a)*radius, 0.f, Math::sin(a)*radius ) );
		float3 p1 = tm.transform( float3( Math::cos(a+da)*radius, 0.f, Math::sin(a+da)*radius ) );
		float3 p2 = tm.transform( float3( Math::cos(a)*radius, height, Math::sin(a)*radius ) );
		float3 p3 = tm.transform( float3( Math::cos(a+da)*radius, height, Math::sin(a+da)*radius ) );
	
		addLine( p0, p1, color );
		addLine( p2, p3, color );
		addLine( p0, p2, color );
		
		a += da;
	}
}


END_NAMESPACE() // hgr

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
