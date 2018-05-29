inline const NS(math,float3x4)& Camera::cachedWorldTransform() const
{
	return m_worldtm;
}

inline const NS(math,float3x4)& Camera::cachedViewTransform() const
{
	return m_viewtm;
}

inline const NS(math,float4x4)& Camera::cachedViewProjectionTransform() const
{
	return m_viewprojtm;
}

inline const NS(math,float3x4)& Camera::getCachedWorldTransform( Node* node ) const
{
	return m_worldTransformCache[ node->m_tmindex ];
}

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
