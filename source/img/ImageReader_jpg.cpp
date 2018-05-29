#ifdef IMG_JPG_SUPPORT

#include <img/ImageReader.h>
#include <io/InputStream.h>
#include <io/IOException.h>
#include <lang/Exception.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "jpeg-6b/jpeglib.h"
#include "jpeg-6b/jinclude.h"
#include "jpeg-6b/jerror.h"
#include <config.h>


USING_NAMESPACE(io)
USING_NAMESPACE(gr)
USING_NAMESPACE(lang)


BEGIN_NAMESPACE(img) 


struct JpgData
{
	jpeg_decompress_struct	cinfo;
	jpeg_error_mgr			errmgr;
};


#define INPUT_BUF_SIZE  4096
#define OUTPUT_BUF_SIZE 4096

METHODDEF(void) errorExit( j_common_ptr cinfo )
{
	jpeg_abort( cinfo );
	jpeg_destroy( cinfo );
	throwError( Exception() );
}

METHODDEF(void) outputMessage( j_common_ptr )
{
}

typedef struct {
	struct jpeg_source_mgr pub;	/* public fields */

	InputStream* infile;		/* source stream */

	JOCTET * buffer;		/* start of buffer */
	boolean start_of_file;	/* have we gotten any data yet? */
} my_source_mgr;

typedef my_source_mgr* pix_src_ptr;

METHODDEF(void)
init_source (j_decompress_ptr cinfo) {
	pix_src_ptr src = (pix_src_ptr) cinfo->src;

	/* We reset the empty-input-file flag for each image,
 	 * but we don't clear the input buffer.
	 * This is correct behavior for reading a series of images from one source.
	*/

	src->start_of_file = TRUE;
}

/*
 * Fill the input buffer --- called whenever buffer is emptied.
 *
 * In typical applications, this should read fresh data into the buffer
 * (ignoring the current state of next_input_byte & bytes_in_buffer),
 * reset the pointer & count to the start of the buffer, and return TRUE
 * indicating that the buffer has been reloaded.  It is not necessary to
 * fill the buffer entirely, only to obtain at least one more byte.
 *
 * There is no such thing as an EOF return.  If the end of the file has been
 * reached, the routine has a choice of ERREXIT() or inserting fake data into
 * the buffer.  In most cases, generating a warning message and inserting a
 * fake EOI marker is the best course of action --- this will allow the
 * decompressor to output however much of the image is there.  However,
 * the resulting error message is misleading if the real problem is an empty
 * input file, so we handle that case specially.
 *
 * In applications that need to be able to suspend compression due to input
 * not being available yet, a FALSE return indicates that no more data can be
 * obtained right now, but more may be forthcoming later.  In this situation,
 * the decompressor will return to its caller (with an indication of the
 * number of scanlines it has read, if any).  The application should resume
 * decompression after it has loaded more data into the input buffer.  Note
 * that there are substantial restrictions on the use of suspension --- see
 * the documentation.
 *
 * When suspending, the decompressor will back up to a convenient restart point
 * (typically the start of the current MCU). next_input_byte & bytes_in_buffer
 * indicate where the restart point will be if the current call returns FALSE.
 * Data beyond this point must be rescanned after resumption, so move it to
 * the front of the buffer rather than discarding it.
 */

METHODDEF(boolean)
fill_input_buffer (j_decompress_ptr cinfo) 
{
	pix_src_ptr src = (pix_src_ptr) cinfo->src;

	size_t nbytes = src->infile->read( src->buffer, INPUT_BUF_SIZE );

	if (nbytes <= 0) {
		if (src->start_of_file)	/* Treat empty input file as fatal error */
			ERREXIT(cinfo, JERR_INPUT_EMPTY);

		WARNMS(cinfo, JWRN_JPEG_EOF);

		/* Insert a fake EOI marker */

		src->buffer[0] = (JOCTET) 0xFF;
		src->buffer[1] = (JOCTET) JPEG_EOI;

		nbytes = 2;
	}

	src->pub.next_input_byte = src->buffer;
	src->pub.bytes_in_buffer = nbytes;
	src->start_of_file = FALSE;

	return TRUE;
}

/*
 * Skip data --- used to skip over a potentially large amount of
 * uninteresting data (such as an APPn marker).
 *
 * Writers of suspendable-input applications must note that skip_input_data
 * is not granted the right to give a suspension return.  If the skip extends
 * beyond the data currently in the buffer, the buffer can be marked empty so
 * that the next read will cause a fill_input_buffer call that can suspend.
 * Arranging for additional bytes to be discarded before reloading the input
 * buffer is the application writer's problem.
 */

METHODDEF(void)
skip_input_data (j_decompress_ptr cinfo, long num_bytes) {
	pix_src_ptr src = (pix_src_ptr) cinfo->src;

	/* Just a dumb implementation for now.  Could use fseek() except
     * it doesn't work on pipes.  Not clear that being smart is worth
	 * any trouble anyway --- large skips are infrequent.
	*/

	if (num_bytes > 0) {
		while (num_bytes > (long) src->pub.bytes_in_buffer) {
		  num_bytes -= (long) src->pub.bytes_in_buffer;

		  (void) fill_input_buffer(cinfo);

		  /* note we assume that fill_input_buffer will never return FALSE,
		   * so suspension need not be handled.
		   */
		}

		src->pub.next_input_byte += (size_t) num_bytes;
		src->pub.bytes_in_buffer -= (size_t) num_bytes;
	}
}

/*
 * Terminate source --- called by jpeg_finish_decompress
 * after all data has been read.  Often a no-op.
 *
 * NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
 * application must deal with any cleanup that should happen even
 * for error exit.
 */

METHODDEF(void)
term_source (j_decompress_ptr /*cinfo*/) {
  /* no work necessary here */
}

/*
 * Prepare for input from a stdio stream.
 * The caller must have already opened the stream, and is responsible
 * for closing it after finishing decompression.
 */

GLOBAL(void)
jpeg_pix_src (j_decompress_ptr cinfo, InputStream* infile ) {
	pix_src_ptr src;

	// allocate memory for the buffer. is released automatically in the end

	if (cinfo->src == NULL) {
		cinfo->src = (struct jpeg_source_mgr *) (*cinfo->mem->alloc_small)
			((j_common_ptr) cinfo, JPOOL_PERMANENT, SIZEOF(my_source_mgr));

		src = (pix_src_ptr) cinfo->src;

		src->buffer = (JOCTET *) (*cinfo->mem->alloc_small)
			((j_common_ptr) cinfo, JPOOL_PERMANENT, INPUT_BUF_SIZE * SIZEOF(JOCTET));
	}

	// initialize the jpeg pointer struct with pointers to functions
	
	src = (pix_src_ptr) cinfo->src;
	src->pub.init_source = init_source;
	src->pub.fill_input_buffer = fill_input_buffer;
	src->pub.skip_input_data = skip_input_data;
	src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
	src->pub.term_source = term_source;
	src->infile = infile;
	src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
	src->pub.next_input_byte = NULL; /* until buffer loaded */
}


void ImageReader::readHeader_jpg()
{
	try
	{
		assert( sizeof(JpgData) < sizeof(m_ffbuffer) );
		JpgData* ffdata = reinterpret_cast<JpgData*>( m_ffbuffer );
		jpeg_decompress_struct* cinfo = &ffdata->cinfo;
		jpeg_error_mgr* errmgr = &ffdata->errmgr;

		memset( errmgr, 0, sizeof(errmgr) );
		errmgr->error_exit = errorExit;
		errmgr->output_message = outputMessage;

		memset( cinfo, 0, sizeof(jpeg_decompress_struct) );
		cinfo->err = jpeg_std_error( errmgr );
		jpeg_create_decompress( cinfo );

		jpeg_pix_src( cinfo, m_in );
		jpeg_read_header( cinfo, TRUE );
			
		// <set decompression param here>

		jpeg_start_decompress( cinfo );

		// setup header info
		m_width = cinfo->output_width;
		m_height = cinfo->output_height;
		m_bitsPerPixel = cinfo->output_components*8;
		m_pitch = cinfo->output_components * cinfo->output_width;
		m_surfaces = 1;
		m_bottomUp = false;
		m_fmt = SurfaceFormat::SURFACE_B8G8R8;
		m_palfmt = SurfaceFormat::SURFACE_UNKNOWN;

		if ( 3 != cinfo->output_components )
			throwError( IOException( Format("Grayscale JPG ({0}) is not supported", m_in->toString()) ) );
	}
	catch ( Exception& )
	{
		throwError( IOException( Format("JPEG {0} reading failed", m_in->toString()) ) );
	}
}

void ImageReader::readScanline_jpg( void* buffer )
{
	assert( sizeof(JpgData) < sizeof(m_ffbuffer) );
	JpgData* ffdata = reinterpret_cast<JpgData*>( m_ffbuffer );
	jpeg_decompress_struct* cinfo = &ffdata->cinfo;

	jpeg_read_scanlines( cinfo, reinterpret_cast<JSAMPARRAY>(&buffer), 1 );
}

void ImageReader::readSurface_jpg( void* bits, int pitch, int w, int h, SurfaceFormat fmt,
	const void* pal, SurfaceFormat palfmt )
{
	try
	{
		readScanlines( bits, pitch, w, h, fmt, pal, palfmt );
		finish_jpg();
	}
	catch ( Exception& )
	{
		finish_jpg();
		throwError( IOException( Format("JPEG {0} reading failed", m_in->toString()) ) );
	}
}

void ImageReader::finish_jpg()
{
	JpgData* ffdata = reinterpret_cast<JpgData*>( m_ffbuffer );
	jpeg_decompress_struct* cinfo = &ffdata->cinfo;
	jpeg_finish_decompress( cinfo );
	jpeg_destroy_decompress( cinfo );
}


END_NAMESPACE() // img

#else // !IMG_JPG_SUPPORT

#include <img/ImageReader.h>
#include <io/InputStream.h>
#include <io/IOException.h>

void img::ImageReader::readHeader_jpg()
{
	lang::throwError( io::IOException( lang::Format("JPEG file support is not enabled (file {0})", m_in->toString()) ) );
}

void img::ImageReader::readSurface_jpg( void*, int, int, int, gr::SurfaceFormat,
	const void*, gr::SurfaceFormat )
{
	lang::throwError( io::IOException( lang::Format("JPEG file support is not enabled (file {0})", m_in->toString()) ) );
}

void img::ImageReader::readScanline_jpg( void* buffer )
{
	lang::throwError( io::IOException( lang::Format("JPEG file support is not enabled (file {0})", m_in->toString()) ) );
}

#endif // IMG_JPG_SUPPORT

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
