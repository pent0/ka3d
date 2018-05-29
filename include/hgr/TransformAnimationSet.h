#ifndef _HGR_TRANSFORMANIMATIONSET_H
#define _HGR_TRANSFORMANIMATIONSET_H


#include <hgr/TransformAnimation.h>
#include <lang/String.h>
#include <lang/Hashtable.h>


BEGIN_NAMESPACE(math) 
	class float3x3;END_NAMESPACE()


BEGIN_NAMESPACE(hgr) 


class Node;


/**
 * Set of key frame based affine transformations.
 * Individual animations can be accessed by name in constant time.
 *
 * @ingroup hgr
 */
class TransformAnimationSet :
	public NS(lang,Hashtable)< NS(lang,String), P(TransformAnimation), NS(lang,Hash)< NS(lang,String) > >
{
public:
	/**
	 * Creates a set with initial space reserved for n animations.
	 */
	explicit TransformAnimationSet( int n );

	/**
	 * Sets common animation playback end behaviour for the whole set.
	 */
	void	setEndBehaviour( TransformAnimation::BehaviourType endbehaviour );

	/**
	 * Rotates top level nodes of the animation set by given matrix.
	 * Useful for example to pre-process character animation data to specific direction.
	 */
	void	rotate( Node* root, const NS(math,float3x3)& rot );

	/**
	 * Scales top level node of the animation set by given scale.
	 * Useful for example to pre-process character animation data to specific scale.
	 */
	void	scale( Node* root, float scale );

	/**
	 * Calls optimize() for each TransformAnimation in the set.
	 */
	void	optimize();

	/**
	 * Returns end time of the animation set.
	 * Assumes that all animations in the set have same length.
	 */
	float	endTime() const;

	/**
	 * Blends n transformation animations together and applies transformation to the node.
	 */
	static void	blend( TransformAnimationSet** animlist, const float* times,
					const float* weights, int n, Node* node );

private:
	TransformAnimationSet( const TransformAnimationSet& );
	TransformAnimationSet& operator=( const TransformAnimationSet& );
};


END_NAMESPACE() // hgr


#endif // _HGR_TRANSFORMANIMATIONSET_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
