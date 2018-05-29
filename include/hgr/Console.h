#ifndef _HGR_CONSOLE_H
#define _HGR_CONSOLE_H


#include <gr/Texture.h>
#include <hgr/Visual.h>

BEGIN_NAMESPACE(gr) 
	class Shader;END_NAMESPACE()

#ifdef PLATFORM_NGI
BEGIN_NAMESPACE(ngi) 
	class IFontFactory;
	class IFont;
	class IBitmap; 
END_NAMESPACE()
#endif


BEGIN_NAMESPACE(hgr) 


/**
 * Unicode (UTF-8) text rendering console.
 * Can be used as part of 3D scene or (more commonly) as external component used to draw text.
 *
 * After constructing the object use addPage() method
 * to add Unicode character page textures to the console.
 * (at least page 0, ISO-LATIN1/ASCII chart, is needed, and it is given in the constructor already)
 *
 * To draw text using console:
 * console->printf( "Hello, world!" );
 * console->render( context );
 * You probably want also to call console->clear() after rendering to clear any buffered text for next frame.
 *
 * There is two ways to position text: Each item has offset, and then the whole console
 * has position (inherited from Visual). When rendering text both offsets are added together
 * to form the final pixel position on screen.
 *
 * @ingroup hgr
 */
class Console :
	public NS(hgr,Visual)
{
public:
	/**
	 * Creates empty text list.
	 * @param context Rendering context.
	 * @param isolatin1 ISO-Latin1 font texture, 256 (16x16) characters.
	 * @param fontwidth Width of a single character in the font in pixels.
	 * @param fontheight Width of a single character in the font in pixels.
	 * @param textoffset Additional offset (X) in pixels between two consequtive characters in output text.
	 * @param shadername Name of the text shader to be used.
	 */
	Console( NS(gr,Context)* context, 
		NS(gr,Texture)* isolatin1, int fontwidth, int fontheight, int textoffset=0,
		const NS(lang,String)& shadername="sprite-alpha" );

	Console( const Console& other );

	~Console();

	/**
	 * Returns clone of this node. Clones child nodes as well.
	 */
	virtual Node*	clone() const;

	/**
	 * Adds text to this console using printf conventions.
	 * @param fmt Format string using printf conventions.
	 */
	void	printf( const char* fmt, ... );

	/**
	 * Adds text to this console using printf conventions and specified offset from origin.
	 * @param x X offset (in pixels).
	 * @param y Y offset (in pixels).
	 * @param fmt Format string using printf conventions.
	 */
	void	printf( int x, int y, const char* fmt, ... );

	/**
	 * Adds an image to this console.
	 * Note that actual rendering is done when render() is called.
	 * @param x Top-left corner of image in device coordinates.
	 * @param y Top-left corner of image in device coordinates.
	 * @param tex Image to draw.
	 */
	void	drawImage( int x, int y, NS(gr,Texture)* tex );

	/**
	 * Adds a part of an image to this console.
	 * Note that actual rendering is done when render() is called.
	 * @param x Top-left corner of image in device coordinates.
	 * @param y Top-left corner of image in device coordinates.
	 * @param subimage Rectangle inside image texture to draw from.
	 * @param tex Image to draw.
	 */
	void	drawImage( int x, int y, const NS(gr,Rect)& subimage, NS(gr,Texture)* tex );

	/**
	 * Adds a stretched image to this console.
	 * Note that actual rendering is done when render() is called.
	 * @param x Top-left corner of image in device coordinates.
	 * @param y Top-left corner of image in device coordinates.
	 * @param tex Image to draw.
	 */
	void	stretchImage( int x, int y, int w, int h, NS(gr,Texture)* tex );

	/**
	 * Adds a stretched part of an image to this console.
	 * Note that actual rendering is done when render() is called.
	 * @param x Top-left corner of image in device coordinates.
	 * @param y Top-left corner of image in device coordinates.
	 * @param subimage Rectangle inside image texture to draw from.
	 * @param tex Image to draw.
	 */
	void	stretchImage( int x, int y, int w, int h, const NS(gr,Rect)& subimage, NS(gr,Texture)* tex );

	/**
	 * Removes all buffered text and images from the console.
	 */
	void	clear();

	/** 
	 * Renders text to the active device. Uses previous cursor position as start position for the text.
	 * @param context Rendering context.
	 */
	void	render( NS(gr,Context)* context );

	/** 
	 * Renders text to the active device.
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
	 * Adds character texture page to the console.
	 * @param tex Character texture of 256 (16x16) characters.
	 * @param page Page index (=first Unicode code point in the page divided by 256)
	 */
	void	addPage( NS(gr,Texture)* tex, int page );

private:
	class Image
	{
	public:
		NS(math,float3)	pos; // position on screen
		P(NS(gr,Texture))	tex;
		float			x0; // source (texture) rectangle
		float			y0;
		float			x1;
		float			y1;
		float			width; // image width on screen
		float			height; // image height on screen
	};

	class TextChar
	{
	public:
		float			x0;
		float			y0;
		int				code;
	};

	P(NS(gr,Shader))					m_shader;
	int								m_fontWidth;
	int								m_fontHeight;
	int								m_textOffset;
	float							m_cursorX;
	float							m_cursorY;
	NS(lang,Array)<TextChar>			m_text;
	NS(lang,Array)<P(NS(gr,Texture))>		m_pages;
	NS(lang,Array)<Image>				m_images;

#ifdef PLATFORM_NGI
	ngi::IFontFactory* 				m_fontFactory;
	ngi::IFont* 					m_font;
	ngi::IBitmap* 					m_fontBitmap;
#endif

	void	put( float x, float y, const char* msg );
	void	draw( NS(gr,Primitive)* prim, NS(gr,Texture)* tex );

	Console& operator=( const Console& );
};


END_NAMESPACE() // hgr


#endif // _HGR_CONSOLE_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
