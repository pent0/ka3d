#include <lang/UTFConverter.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <config.h>


BEGIN_NAMESPACE(lang) 


static bool decode_ASCII7( const uint8_t* src, int srcsize, int* srcbytes, int* buffer )
{
	const uint8_t*	src0	= src;
	int				err		= 0;
	int		cp		= int(-1);

	if ( srcsize >= 1 )
	{
		cp = *src++;
	}
	else
	{
		// ERROR: Not enough encoded bytes available
		err = 4;
	}

	if ( !err )
		*buffer = cp;
	*srcbytes = (src-src0);
	return !err;
}

static bool decode_UTF8( const uint8_t* src, int srcsize, int* srcbytes, int* buffer )
{
	const uint8_t*	src0	= src;
	int				err		= 0;
	int				cp		= int(-1);

	if ( srcsize >= 1 )
	{
		uint8_t first = *src++;
		if ( 0 == (first & 0x80) )
		{
			// ok, single byte ASCII-7 (US-ASCII) code
			cp = first;
		}
		else
		{
			// multibyte character code

			// read remaining byte count and 
			// most signifigant bits of the character code
			int bytes = 1;
			uint8_t bytecountmask = 0x40;
			int codeMask = 0x3F;
			while ( 0 != (first & bytecountmask) )
			{
				++bytes;
				bytecountmask >>= 1;
				codeMask >>= 1;
			}
			if ( bytes < 2 || bytes > 4 )
			{
				// ERROR: Invalid number of following bytes
				err = 1;
			}
			else
			{
				if ( srcsize < bytes )
				{
					// ERROR: Not enough encoded bytes available
					err = 4;
				}
				else
				{
					// read remaining bytes of the character code
					cp = first & codeMask;
					for ( int i = 1 ; i < bytes ; ++i )
					{
						cp <<= 6;
						cp |= ( 0x3F & (int)*src++ );
					}
				}
			}
		}
	}
	else
	{
		// ERROR: Not enough encoded bytes available
		err = 4;
	}

	if ( !err )
		*buffer = cp;
	*srcbytes = (src-src0);
	return !err;
}

static bool decode_UTF16( const uint8_t* src, int srcsize, int* srcbytes, int* buffer, bool bigendian )
{
	const uint8_t*	src0	= src;
	int				err		= 0;
	int			cp		= int(-1);

	if ( srcsize >= 2 )
	{
		if ( bigendian )
			cp = (int(src[0])<<8) + int(src[1]);
		else
			cp = int(src[0]) + (int(src[1])<<8);
		src += 2;

		if ( 0xD800 == (cp&0xFFFFFC00) )
		{
			if ( srcsize >= 4 )
			{
				int ch2;
				if ( bigendian )
					ch2 = (int(src[0])<<8) + int(src[1]);
				else
					ch2 = int(src[0]) + (int(src[1])<<8);
				src += 2;

				cp = (cp<<10) + ch2 - ((0xd800<<10UL)+0xdc00-0x10000);
			}
			else
			{
				// ERROR: Not enough encoded bytes available
				err = 4;
			}
		}
	}
	else
	{
		// ERROR: Not enough encoded bytes available
		err = 4;
	}

	if ( !err )
		*buffer = cp;
	*srcbytes = (src-src0);
	return !err;
}

static bool decode_UTF32( const uint8_t* src, int srcsize, int* srcbytes, int* buffer, bool bigendian )
{
	const uint8_t*	src0	= src;
	int				err		= 0;
	int			cp		= int(-1);

	if ( srcsize >= 4 )
	{
		if ( bigendian )
		{
			cp = 0;
			for ( int i = 0 ; i < 4 ; ++i )
			{
				cp <<= 8;
				cp += src[i];
			}
		}
		else // little endian
		{
			cp = 0;
			for ( int i = 4 ; i > 0 ; )
			{
				--i;
				cp <<= 8;
				cp += src[i];
			}
		}
		src += 4;
	}
	else
	{
		// ERROR: Not enough encoded bytes available
		err = 4;
	}

	if ( !err )
		*buffer = cp;
	*srcbytes = (src-src0);
	return !err;
}

static bool encode_ASCII7( uint8_t* dst, int dstsize, int* dstbytes, int cp )
{
	const uint8_t*	dst0	= dst;
	int				err		= 0;

	if ( dstsize >= 1 )
	{
		if ( cp >= 128 )
		{
			// ERROR: Out-of-range ASCII-7 code
			err = 1;
		}
		else
		{
			*dst++ = (uint8_t)cp;
		}
	}
	else
	{
		// ERROR: Not enough buffer space
		err = 5;
		cp = int(-1);
	}

	*dstbytes = (dst-dst0);
	return !err;
}

static bool encode_UTF8( uint8_t* dst, int dstsize, int* dstbytes, int cp )
{
	const uint8_t*	dst0	= dst;
	int				err		= 0;

	if (cp < 0x80) 
	{
		if ( dstsize < 1 )
		{
			// ERROR: Not enough buffer space.
			err = 5;
		}
		else
		{
			*dst++ = (uint8_t)cp;
		}
	}
	else if (cp < 0x800) 
	{
		if ( dstsize < 2 )
		{
			// ERROR: Not enough buffer space.
			err = 5;
		}
		else
		{
			*dst++ = (uint8_t)( 0xC0 | (cp>>6) );
			*dst++ = (uint8_t)( 0x80 | (cp&0x3F) );
		}
	}
	else if (cp < 0x10000) 
	{
		if ( dstsize < 3 )
		{
			// ERROR: Not enough buffer space.
			err = 5;
		}
		else
		{
			*dst++ = (uint8_t)( 0xE0 | (cp>>12) );
			*dst++ = (uint8_t)( 0x80 | ( (cp>>6) &0x3F) );
			*dst++ = (uint8_t)( 0x80 | (cp&0x3F) );
		}
	}
	else if (cp < 0x200000) 
	{
		if ( dstsize < 4 )
		{
			// ERROR: Not enough buffer space.
			err = 5;
		}
		else
		{
			*dst++ = (uint8_t)( 0xF0 | (cp>>18) );
			*dst++ = (uint8_t)( 0x80 | ( (cp>>12) &0x3F) );
			*dst++ = (uint8_t)( 0x80 | ( (cp>>6) &0x3F) );
			*dst++ = (uint8_t)( 0x80 | (cp&0x3F) );
		}
	}
	else
	{
		// ERROR: Invalid Unicode scalar value
		err = 2;
	}

	*dstbytes = (dst-dst0);
	return !err;
}

static bool encode_UTF16( uint8_t* dst, int dstsize, int* dstbytes, int cp, bool bigendian )
{
	const uint8_t*	dst0	= dst;
	int				err		= 0;

	// encode
	uint16_t codes[2];
	int codecount = 0;
	if ( cp >= 0x10000 )
	{
		codes[codecount++] = uint16_t( ((cp-0x10000)>>10) + 0xD800 );
		codes[codecount++] = uint16_t( ((cp-0x10000)&1023) + 0xDC00 );
	}
	else
	{
		codes[codecount++] = uint16_t( cp );
	}

	// write
	int codeSize = unsigned(codecount) * 2U;
	if ( dstsize < codeSize )
	{
		// Error: Not enough buffer space
		err = 5;
	}
	else
	{
		for ( int i = 0 ; i < codecount ; ++i )
		{
			uint16_t code = codes[i];
			if ( bigendian )
			{
				*dst++ = uint8_t(code >> 8);
				*dst++ = uint8_t(code);
			}
			else
			{
				*dst++ = uint8_t(code);
				*dst++ = uint8_t(code >> 8);
			}
		}
	}
	
	*dstbytes = (dst-dst0);
	return !err;
}

static bool encode_UTF32( uint8_t* dst, int dstsize, int* dstbytes, int cp, bool bigendian )
{
	const uint8_t*	dst0	= dst;
	int				err		= 0;

	// write
	int codecount = 1;
	int codeSize = unsigned(codecount) * 4U;
	if ( dstsize < codeSize )
	{
		// Error: Not enough buffer space
		err = 5;
	}
	else
	{
		int code = cp;
		if ( bigendian )
		{
			*dst++ = uint8_t(code >> 24);
			*dst++ = uint8_t(code >> 16);
			*dst++ = uint8_t(code >> 8);
			*dst++ = uint8_t(code);
		}
		else
		{
			*dst++ = uint8_t(code);
			*dst++ = uint8_t(code >> 8);
			*dst++ = uint8_t(code >> 16);
			*dst++ = uint8_t(code >> 24);
		}
	}
	
	*dstbytes = (dst-dst0);
	return !err;
}

inline static bool littleEndian()
{
	int x = 1;
	return *reinterpret_cast<char*>(&x) == 1;
}


UTFConverter::UTFConverter( EncodingType encoding ) :
	m_type(encoding)
{
	if ( ENCODING_UTF16 == encoding )
	{
		if ( littleEndian() )
			m_type = ENCODING_UTF16LE;
		else
			m_type = ENCODING_UTF16BE;
	}
	else if ( ENCODING_UTF32 == encoding )
	{
		if ( littleEndian() )
			m_type = ENCODING_UTF32LE;
		else
			m_type = ENCODING_UTF32BE;
	}
}

bool UTFConverter::decode( const void* src, const void* srcend, int* srcbytes, int* dst ) const
{
	const uint8_t*	bsrc		= reinterpret_cast<const uint8_t*>( src );
	const uint8_t*	bsrcend		= reinterpret_cast<const uint8_t*>( srcend );
	const int		srcsize		= (bsrcend - bsrc);

	switch ( EncodingType(m_type) )
	{
	case ENCODING_UNKNOWN:	return false;
	case ENCODING_ASCII7:	return decode_ASCII7( bsrc, srcsize, srcbytes, dst );
	case ENCODING_UTF8:		return decode_UTF8	( bsrc, srcsize, srcbytes, dst );
	case ENCODING_UTF16BE:	return decode_UTF16	( bsrc, srcsize, srcbytes, dst, true );
	case ENCODING_UTF16LE:	return decode_UTF16	( bsrc, srcsize, srcbytes, dst, false );
	case ENCODING_UTF32BE:	return decode_UTF32	( bsrc, srcsize, srcbytes, dst, true );
	case ENCODING_UTF32LE:	return decode_UTF32	( bsrc, srcsize, srcbytes, dst, false );
	case ENCODING_UTF16:
	case ENCODING_UTF32:	return false; // shouldn't end up here because platform endianess is in ctor
	}
	return false;
}

bool UTFConverter::encode( void* dst, void* dstend, int* dstbytes, int src ) const
{
	uint8_t*	bdst	= reinterpret_cast<uint8_t*>( dst );
	uint8_t*	bdstend	= reinterpret_cast<uint8_t*>( dstend );
	int			dstsize	= (bdstend - bdst);

	switch ( EncodingType(m_type) )
	{
	case ENCODING_UNKNOWN:	return false;
	case ENCODING_ASCII7:	return encode_ASCII7( bdst, dstsize, dstbytes, src );
	case ENCODING_UTF8:		return encode_UTF8	( bdst, dstsize, dstbytes, src );
	case ENCODING_UTF16BE:	return encode_UTF16	( bdst, dstsize, dstbytes, src, true );
	case ENCODING_UTF16LE:	return encode_UTF16	( bdst, dstsize, dstbytes, src, false );
	case ENCODING_UTF32BE:	return encode_UTF32	( bdst, dstsize, dstbytes, src, true );
	case ENCODING_UTF32LE:	return encode_UTF32	( bdst, dstsize, dstbytes, src, false );
	case ENCODING_UTF16:
	case ENCODING_UTF32:	return false; // shouldn't end up here because platform endianess is in ctor
	}
	return false;
}


END_NAMESPACE() // lang

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
