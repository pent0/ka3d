#include <img/ImageWriter.h>
#include <config.h>


BEGIN_NAMESPACE(img)


uint32_t ImageWriter::getBytesLE( const uint8_t* src, int bytesperpixel )
{
	uint32_t d = uint32_t(src[0]);
	switch ( bytesperpixel )
	{
	case 4:
		d += uint32_t(src[3])<<24;
	case 3:
		d += uint32_t(src[2])<<16;
	case 2:
		d += uint32_t(src[1])<<8;
	}
	return d;
}

void ImageWriter::setBytesLE( uint8_t* dst, uint32_t c, int bytesperpixel )
{
	switch ( bytesperpixel )
	{
	case 4:
		dst[3] = uint8_t(c>>24);
	case 3:
		dst[2] = uint8_t(c>>16);
	case 2:
		dst[1] = uint8_t(c>>8);
	}
	dst[0] = uint8_t( c );
}


END_NAMESPACE()
