#ifndef _HGR_LINES_H
#define _HGR_LINES_H


#include <gr/Shader.h>
#include <hgr/Visual.h>


BEGIN_NAMESPACE(hgr) 


/**
 * Line list.
 * Lines can have gradient color and transparency.
 * Lines can define 'paths' which can be used for example
 * to path finding.
 * @ingroup hgr
 */
class Lines :
	public Visual
{
public:
	/**
	 * Creates empty line/path list.
	 * @param shadername Name of the line shader to be used.
	 */
	explicit Lines( NS(gr,Context)* context, const NS(lang,String)& shadername="lines-alpha" );

	Lines( const Lines& other );

	~Lines();

	/**
	 * Returns clone of this node. Clones child nodes as well.
	 */
	virtual Node*	clone() const;

	/** 
	 * Renders this visual to the active device.
	 * @param context Rendering context.
	 * @param camera Active camera.
	 * @param priority Shader priority level which is to be rendered.
	 */
	void	render( NS(gr,Context)* context, Camera* camera, int priority );

	/**
	 * Returns array of shaders used by this visual.
	 * NOTE: Implementation should not clear the list of returned shaders
	 * before adding new ones, since shadesr from multiple Visuals might be 
	 * collected at the same time.
	 */
	void	getShaders( NS(lang,Array)<NS(gr,Shader)*>& shaders );

	/**
	 * Sets rendering technique to be used while rendering the visual.
	 */
	void	setTechnique( const char* name );

	/**
	 * Computes bounding volume of the visual in model space.
	 */
	void	computeBound();

	/**
	 * Reserves space for n lines and paths.
	 */
	void	reserve( int lines, int paths );

	/**
	 * Removes all lines from the line list.
	 */
	void	removeLines();

	/**
	 * Adds a constant color line with optional transparency to the line list.
	 * Line list need to be locked by creating Lock object to scope before calling this method.
	 * This method requires that there is space for the line in the line list.
	 * You can check if there is space by calling count() and comparing that 
	 * the return value is below maxCount().
	 * @param startpoint Start position of the line in model space.
	 * @param endpoint End position of the line in model space.
	 * @param color Color and transparency of the line (R,G,B,A). Opaque white is (1,1,1,1) and black (0,0,0,0).
	 */
	void	addLine( const NS(math,float3)& startpoint, const NS(math,float3)& endpoint,
				const NS(math,float4)& color );

	/**
	 * Adds a gradient line with optional transparency to the line list.
	 * @param startpoint Start position of the line in model space.
	 * @param endpoint End position of the line in model space.
	 * @param startcolor Start color and transparency of the line (R,G,B,A). Opaque white is (1,1,1,1) and black (0,0,0,0).
	 * @param endcolor End color and transparency of the line (R,G,B,A). Opaque white is (1,1,1,1) and black (0,0,0,0).
	 */
	void	addLine( const NS(math,float3)& startpoint, const NS(math,float3)& endpoint,
				const NS(math,float4)& startcolor, const NS(math,float4)& endcolor );

	/**
	 * Adds a box to the line list, consisting of 8 lines of specified color.
	 */
	void	addBox( const NS(math,float3x4)& tm, 
				const NS(math,float3)& boxmin, const NS(math,float3)& boxmax,
				const NS(math,float4)& color );

	/**
	 * Adds a sphere approximation to the line list, consisting of n lines.
	 */
	void	addSphere( const NS(math,float3x4)& tm, float radius, 
				const NS(math,float4)& color, int lines=32 );

	/**
	 * Adds a cylinder to the line list.
	 * @param tm Cylinder bottom transform
	 * @param radius Cylinder radius
	 * @param height Cylinder height along positive Z axis
	 * @param color Color of the lines
	 * @param segments Number of segments in the cylinder.
	 */
	void 	addCylinder( const NS(math,float3x4)& tm, float radius, float height,
				const NS(math,float4)& color, int segments=12 );

	/**
	 * Adds a 'path' to the line list. Path consists of sequence of lines,
	 * which have been added to the list previously.
	 * @param beginline The first line which belongs to this path.
	 * @param beginline One beyond the last line which belongs to this path.
	 */
	void	addPath( int beginline, int endline );

	/**
	 * Returns ith line start point.
	 */
	const NS(math,float3)&	getLineStart( int i ) const			{return m_lines[i].start;}

	/**
	 * Returns ith line end point.
	 */
	const NS(math,float3)&	getLineEnd( int i ) const			{return m_lines[i].end;}

	/**
	 * Returns the first line in path i.
	 */
	int		getPathBegin( int i ) const							{return m_paths[i].begin;}

	/**
	 * Returns one beyond the last line in path i.
	 */
	int		getPathEnd( int i ) const							{return m_paths[i].end;}

	/**
	 * Returns number of lines in the line list.
	 */
	int		lines() const;

	/**
	 * Returns number of paths in the line list.
	 */
	int		paths() const;

private:
	class Line
	{
	public:
		NS(math,float3)	start;
		NS(math,float3)	end;
		NS(math,float4)	startcolor;
		NS(math,float4)	endcolor;
	};

	class Path
	{
	public:
		int		begin;
		int		end;
	};

	P(NS(gr,Shader))		m_shader;
	NS(lang,Array)<Line>	m_lines;
	NS(lang,Array)<Path>	m_paths;
	int					m_maxLines;

	Lines& operator=( const Lines& );
};


END_NAMESPACE() // hgr


#endif // _HGR_LINES_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
