#include <hgr/TransformAnimationSet.h>
#include <hgr/Node.h>
#include <math/float3.h>
#include <math/float3x4.h>
#include <math/quaternion.h>
#include <config.h>


USING_NAMESPACE(lang)
USING_NAMESPACE(math)


BEGIN_NAMESPACE(hgr) 


TransformAnimationSet::TransformAnimationSet( int n ) :
	Hashtable<String,P(TransformAnimation), Hash<String> >( n )
{
}

void TransformAnimationSet::setEndBehaviour( TransformAnimation::BehaviourType endbehaviour )
{
	for ( HashtableIterator<String,P(TransformAnimation)> it = begin() ; it != end() ; ++it )
	{
		TransformAnimation* a = it.value();
		if ( a != 0 )
			a->setEndBehaviour( endbehaviour );
	}
}

void TransformAnimationSet::optimize()
{
	for ( HashtableIterator<String,P(TransformAnimation)> it = begin() ; it != end() ; ++it )
	{
		TransformAnimation* a = it.value();
		if ( a != 0 )
			a->optimize();
	}
}

float TransformAnimationSet::endTime() const
{
	for ( HashtableIterator<String,P(TransformAnimation)> it = begin() ; it != end() ; ++it )
	{
		TransformAnimation* a = it.value();
		if ( a != 0 && a->positionKeys() > 1 || a->rotationKeys() > 1 )
			return a->endTime();
	}
	return 0.f;
}

void TransformAnimationSet::rotate( Node* root, const float3x3& rot )
{
	for ( Node* child = root->firstChild() ; child != 0 ; child = root->getNextChild(child) )
	{
		TransformAnimation* a = get( child->name() );
		if ( a != 0 )
			a->rotate( rot );
	}
}

void TransformAnimationSet::scale( Node* root, float scale )
{
	for ( Node* child = root->firstChild() ; child != 0 ; child = root->getNextChild(child) )
	{
		TransformAnimation* a = get( child->name() );
		if ( a != 0 )
			a->scale( scale );
	}
}

void TransformAnimationSet::blend( TransformAnimationSet** animlist, const float* times,
	const float* weights, int n, Node* node )
{
	float3 blendpos(0,0,0);
	float3 blendscale(0,0,0);
	quaternion blendrot(0,0,0,1);
	float sumweight = 0.f;

	for ( int i = 0 ; i < n ; ++i )
	{
		TransformAnimation* a = 0;
		if ( animlist[i] != 0 )
			a = animlist[i]->get( node->name() );

		float3 pos, scale;
		quaternion rot;
		if ( a != 0 )
		{
			a->eval( times[i], &pos, &rot, &scale );
		}
		else
		{
			pos = node->position();
			scale = float3(1.f,1.f,1.f);
			rot = quaternion( node->rotation().orthonormalize() );
		}

		float weight = weights[i];
		sumweight += weight;

		blendpos += pos * weight;
		blendscale += scale * weight;

		if ( sumweight-weight < 1e-6f  )
		{
			blendrot = rot;
		}
		else
		{
			if ( blendrot.dot(rot) < 0.f )
				rot = -rot;
			blendrot = blendrot.slerp( weight/sumweight, rot );
		}
	}

	float3x4 tm( blendrot, blendpos, blendscale );
	node->setTransform( tm );
}


END_NAMESPACE() // hgr

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
