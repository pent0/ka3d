inline Scene::FogType Scene::fogType() const
{
	return m_fogType;
}

inline float Scene::fogStart() const
{
	return m_fogStart;
}

inline float Scene::fogEnd() const
{
	return m_fogEnd;
}

inline const NS(math,float3)& Scene::fogColor() const
{
	return m_fogColor;
}

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
