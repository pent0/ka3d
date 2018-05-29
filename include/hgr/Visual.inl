inline float Visual::boundRadius() const		
{
	assert(!isBoundWorld()); 
	return m_boundRadius;
}

inline NS(math,float3) Visual::boundCenter() const
{
	return NS(math,float3)( 
		(m_boundBoxMin.x+m_boundBoxMax.x)*.5f,
		(m_boundBoxMin.y+m_boundBoxMax.y)*.5f,
		(m_boundBoxMin.z+m_boundBoxMax.z)*.5f );
}

inline const NS(math,float3)& Visual::boundBoxMin() const		
{
	assert(!isBoundWorld()); 
	return m_boundBoxMin;
}

inline const NS(math,float3)& Visual::boundBoxMax() const		
{
	assert(!isBoundWorld()); 
	return m_boundBoxMax;
}

inline const NS(math,float3)& Visual::boundBoxMinWorld() const		
{
	assert(isBoundWorld()); 
	return m_boundBoxMin;
}

inline const NS(math,float3)& Visual::boundBoxMaxWorld() const		
{
	assert(isBoundWorld()); 
	return m_boundBoxMax;
}

inline bool Visual::isBoundInfinity() const
{
	return m_boundRadius == MAX_BOUND;
}

inline bool Visual::isBoundWorld() const
{
	return 0 != (flags() & NODE_BOUNDWORLDSPACE);
}

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
