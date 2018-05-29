#include <hgr/Camera.h>
#include <gr/Shader.h>
#include <gr/Context.h>
#include <hgr/Mesh.h>
#include <hgr/Light.h>
#include <hgr/Visual.h>
#include <hgr/PipeSetup.h>
#include <lang/Math.h>
#include <lang/Debug.h>
#include <lang/algorithm/sort.h>
#include <lang/pp.h>
#include <string.h>

#if defined(PLATFORM_WIN32) && defined(_DEBUG)
#include <framework/App.h>
USING_NAMESPACE(framework)
//#define DEBUG_FRUSTUM_CULLING
//#define DEBUG_DUMP_OBJECT_RENDERING_ORDER_BY_PRESSING_D
//#define DEBUG_LINES
#endif

#include <config.h>


//#define ENABLE_SORTING
#define ENABLE_FRUSTUM_CULLING
#define ENABLE_SPHERE_FRUSTUM_TEST
//#define ENABLE_OBBOX_FRUSTUM_TEST
#pragma warning( disable : 4127 )


USING_NAMESPACE(gr)
USING_NAMESPACE(math)
USING_NAMESPACE(lang)


BEGIN_NAMESPACE(hgr) 


void Camera::Statistics::reset()
{
	memset( this, 0, sizeof(Statistics) );
}

Camera::Camera() :
	m_frustum(),
	m_worldtm( 1.f ),
	m_viewtm( 1.f ),
	m_viewprojtm( 1.f ),
	m_lightSorterPtr( 0 ),
	m_ortho( false )
{
	setClassId( NODE_CAMERA );
}

Camera::Camera( const Camera& other ) : 
	Node( other ),
	m_frustum( other.m_frustum ),
	m_worldtm( other.m_worldtm ),
	m_viewtm( other.m_viewtm ),
	m_viewprojtm( other.m_viewprojtm ),
	m_lightSorterPtr( 0 ),
	m_ortho( other.m_ortho )
{
	setClassId( NODE_CAMERA );
}

Camera::~Camera()
{
}

Node* Camera::clone() const
{
	return new Camera( *this );
}

void Camera::setVerticalFov( float fovy )
{
	m_frustum.setVerticalFov( fovy );
}

void Camera::setHorizontalFov( float fovx )
{
	m_frustum.setHorizontalFov( fovx );
}

void Camera::setFront( float front )
{
	m_frustum.setFront( front );
}

void Camera::setBack( float back )													
{
	m_frustum.setBack( back );
}

float3x4 Camera::viewTransform() const
{
	return worldTransform().inverse();
}

void Camera::render( Context* context )
{
#ifdef DEBUG_LINES
	if ( m_lines == 0 )
	{
		m_lines = new Lines( context );
		m_lines->linkTo( root() );
	}
	m_lines->removeLines();
	float3 p0 = position()+rotation().getColumn(2)*1000;
	m_lines->addLine( p0, p0+float3(0,100,0), float4(1,0,0,1) );

	Node* root = this->root();
	for ( Node* node = root ; node != 0 ; node = node->next(root) )
	{
		if ( node->classId() == Node::NODE_MESH )
		{
			Mesh* mesh = (Mesh*)node;

			float3x4 tm = node->worldTransform();
			float3x4 trans( float3x3(1), mesh->boundCenter() );
			tm = tm * trans;
			m_lines->addSphere( tm, mesh->boundRadius(), float4(1,0,0,1), 16 );
		}
	}
#endif

	if ( Context::PLATFORM_N3D == context->platform() )
		mirrorXAxis();

	PipeSetup::getNodes( this, m_nodes );
	PipeSetup::getLights( m_nodes, m_lightSorter );
	cacheTransforms( context, m_nodes );
	cullVisuals( m_nodes, m_visuals );

	PipeSetup::getShaders( m_visuals, m_shaders );
	PipeSetup::getPriorities( m_shaders, m_priorities );

	render( context, -100, 100, m_visuals, m_priorities, &m_lightSorter );

	if ( Context::PLATFORM_N3D == context->platform() )
		mirrorXAxis();
}

void Camera::render( Context* context, int minpriority, int maxpriority,
	const Array<Visual*>& visuals, const Array<int>& priorities,
	LightSorter* lightsorter )
{
#ifdef DEBUG_DUMP_OBJECT_RENDERING_ORDER_BY_PRESSING_D
	bool debugdump = ( App::get()->isKeyDown(App::KEY_D) );
#endif

	const int visualcount = visuals.size();
	const int prioritycount = priorities.size();

	m_lightSorterPtr = lightsorter;

	for ( int k = 0 ; k < prioritycount ; ++k )
	{
		int priority = priorities[k];
		if ( priority >= minpriority && priority <= maxpriority )
		{
			if ( (priority&1) == 0 )
			{
				// even priority materials are rendered front-to-back
				for ( int i = 0 ; i < visualcount ; ++i )
				{
#ifdef DEBUG_DUMP_OBJECT_RENDERING_ORDER_BY_PRESSING_D
					if ( debugdump )
					{
						Debug::printf( "node %s rendered\n", visuals[i]->name().c_str() );
					}
#endif

					visuals[i]->render( context, this, priority );
				}
			}
			else
			{
				// odd priority materials are rendered back-to-front
				for ( int i = visualcount-1 ; i >= 0 ; --i )
				{
#ifdef DEBUG_DUMP_OBJECT_RENDERING_ORDER_BY_PRESSING_D
					if ( debugdump )
					{
						Debug::printf( "node %s rendered\n", visuals[i]->name().c_str() );
					}
#endif

					visuals[i]->render( context, this, priority );
				}
			}
		}
	}

	m_lightSorterPtr = 0;
}

float Camera::front() const															
{
	return m_frustum.front();
}

float Camera::back() const															
{
	return m_frustum.back();
}

float Camera::horizontalFov() const
{
	return m_frustum.horizontalFov();
}

float Camera::verticalFov() const
{
	return m_frustum.verticalFov();
}

Array<Light*>& Camera::getLightsSortedByDistance( const float3& worldpos ) const
{
	assert( m_lightSorterPtr ); // if 0 then not in rendering
	return m_lightSorterPtr->getLightsByDistance( worldpos );
}

void Camera::cullVisuals( const Array<Node*>& nodes, Array<Visual*>& visuals )
{
	assert( m_viewtm == viewTransform() ); // cached transforms not up-to-date

#ifdef DEBUG_FRUSTUM_CULLING
	static bool s_visualsLocked = false;
	if ( App::get()->isKeyDown(App::KEY_L) )
		s_visualsLocked = !s_visualsLocked;
	if ( s_visualsLocked ) 
		return;
#endif

#ifdef DEBUG_DUMP_OBJECT_RENDERING_ORDER_BY_PRESSING_D
	bool debugdump = ( NS(framework,App)::get()->isKeyDown(NS(framework,App)::KEY_D) );
	static int count = 0;
	if ( debugdump )
		count++;
	if (debugdump) Debug::printf( "-------------------------- [%d]\n", count );
#endif

	// view frustum in world space
#ifdef ENABLE_FRUSTUM_CULLING
	float4 frustumworld[ViewFrustum::PLANE_COUNT];
	m_frustum.getPlanes( cachedWorldTransform(), frustumworld );
#endif

	// cull visuals by bounding boxes
	m_visualSorter.clear();
	visuals.clear();

	statistics.visualsBeforeCull = 0;
	statistics.visualsAfterCull = 0;

	for ( int i = 0 ; i < nodes.size() ; ++i )
	{
		Node* node = nodes[i];

		if ( node->enabled() && node->isVisualNode() )
		{
			Visual* vis = static_cast<Visual*>( node );
			statistics.visualsBeforeCull++;

#if defined(ENABLE_FRUSTUM_CULLING) || defined(ENABLE_SORTING)
			const float3x4& worldtm = getCachedWorldTransform( vis );
#endif

#ifdef ENABLE_FRUSTUM_CULLING
			bool visible = vis->isBoundInfinity();
			if ( !visible )
			{
#ifdef ENABLE_SPHERE_FRUSTUM_TEST
				float r = vis->boundRadius();
				//float r = Math::max( vis->boundBoxMax().length(), vis->boundBoxMin().length() );
				visible = ViewFrustum::testSphere( worldtm, r, frustumworld, vis->frustumCheckHint );
#endif

#ifdef ENABLE_OBBOX_FRUSTUM_TEST
				if ( vis->isBoundWorld() )
					visible = ViewFrustum::testAABox( vis->boundBoxMinWorld(), vis->boundBoxMaxWorld(), frustumworld );
				else
					visible = ViewFrustum::testOBox( worldtm, vis->boundBoxMin(), vis->boundBoxMax(), frustumworld, vis->frustumCheckHint );
#endif
			}
#else
			const bool visible = true;
#endif

			if ( visible )
			{
#ifdef ENABLE_SORTING
				VisualSorter objdata;
				objdata.depth = m_viewtm(2,0)*worldtm(0,3) + m_viewtm(2,1)*worldtm(1,3) + m_viewtm(2,2)*worldtm(2,3) + m_viewtm(2,3);
				objdata.obj = vis;
				m_visualSorter.add( objdata );
#else
				visuals.add( vis );
#endif

#ifdef DEBUG_DUMP_OBJECT_RENDERING_ORDER_BY_PRESSING_D
				if ( debugdump )
				{
					float3 pos = m_viewtm.transform( worldtm.translation() );
					Debug::printf( "node %s: %g %g %g\n", vis->name().c_str(), pos.x, pos.y, pos.z );
					//assert( pos.z == objdata.depth );
				}
#endif

				statistics.visualsAfterCull++;
			}
		}
	}

	// sort visuals by Z-distance to camera (smaller camera space Z first)
	// return visuals in the user array
#ifdef ENABLE_SORTING
	LANG_SORT( m_visualSorter.begin(), m_visualSorter.end() );
	visuals.resize( m_visualSorter.size() );
	for ( int i = 0 ; i < visuals.size() ; ++i )
		visuals[i] = m_visualSorter[i].obj;
#endif

#ifdef DEBUG_DUMP_OBJECT_RENDERING_ORDER_BY_PRESSING_D
	if ( debugdump )
	{
		for ( int i = 0 ; i < visuals.size() ; ++i )
		{
			Node* vis = visuals[i];
			float3 pos = m_viewtm.transform( vis->worldTransform().translation() );
			Debug::printf( "%d. node %s: %g %g %g\n", i+1, vis->name().c_str(), pos.x, pos.y, pos.z );
		}
	}
#endif
}

void Camera::cacheTransforms( Context* context, const Array<Node*>& nodes )
{

	context->setOrthographicProjection( orthographicProjection() );

	// NOTE: Even if orthographic projection is used, we still need to call
	// setPerspectiveProjection() because it sets viewport related stuff.
#ifdef PLATFORM_BREW
	context->setPerspectiveProjection( horizontalFov(), front(), back(), 1.6f ); // HACK: VX 9800
#else
	context->setPerspectiveProjection( horizontalFov(), front(), back(), m_frustum.aspect() );
#endif

	Array<float3x4>& tmcache = m_worldTransformCache;
	tmcache.clear();

	for ( int i = 0 ; i < nodes.size() ; ++i )
	{
		assert( tmcache.size() < 0x10000 );

		Node* node = nodes[i];
		node->m_tmindex = (short)tmcache.size();

		Node* parent = node->parent();
		if ( 0 == parent )
			tmcache.add( node->transform() );
		else
			tmcache.add( tmcache[parent->m_tmindex] * node->transform() );
	}

	m_frustum.setAspect( context->aspect() );
	m_worldtm = tmcache[m_tmindex];
	m_viewtm = m_worldtm.inverse();

#if !defined(PLATFORM_NGI) && !defined(PLATFORM_BREW)
	m_viewprojtm = context->projectionTransform() * m_viewtm;
#endif
}

void Camera::mirrorXAxis()
{
	float3x4 tm = transform();
	tm.setColumn( 0, -tm.getColumn(0) );
	setTransform( tm );
}

void Camera::setOrthographicProjection( bool enabled )
{
	m_ortho = enabled;
}


END_NAMESPACE() // hgr
