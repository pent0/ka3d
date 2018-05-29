#include <math/Domain.h>


BEGIN_NAMESPACE(hgr) 


/*
 * IMPLEMENTATION CLASS. Used for calculating particle system property values.
 */
template <class T> class ParticleSystem_Integral
{
public:
	T		v;
	T		dv;

	void set( NS(math,Domain)& startdomain, NS(math,Domain)& enddomain, float invlife )
	{
		startdomain.getRandom( &v );
		enddomain.getRandom( &dv );
		dv -= v;
		dv *= invlife;
	}

	void integrate( float dt )
	{
		v += dv*dt;
	}
};


END_NAMESPACE() // hgr
