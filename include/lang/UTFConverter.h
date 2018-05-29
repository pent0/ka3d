#ifndef _LANG_UTFCONVERTER_H
#define _LANG_UTFCONVERTER_H


#include <lang/pp.h>
#include <lang/Converter.h>


BEGIN_NAMESPACE(lang) 


/**
 * Unicode UTF-data encoding/decoding helper class.
 * Supported encoding schemes are 
 * ASCII-7, UTF-8, UTF-16BE, UTF-16LE, UTF-32BE, UTF-32LE.
 * 
 * @ingroup lang
 */
class UTFConverter :
	public Converter
{
public:
	/** Encoding/decoding scheme. */
	enum EncodingType
	{
		/** Unsupported/unknown encoding. */
		ENCODING_UNKNOWN,
		/** US/ASCII */
		ENCODING_ASCII7,
		/** UTF-8 */
		ENCODING_UTF8,
		/** UTF-16, platform endian */
		ENCODING_UTF16,
		/** UTF-16 Big Endian */
		ENCODING_UTF16BE,
		/** UTF-16 Little Endian */
		ENCODING_UTF16LE,
		/** UTF-32, platform endian */
		ENCODING_UTF32,
		/** UTF-32 Big Endian */
		ENCODING_UTF32BE,
		/** UTF-32 Little Endian */
		ENCODING_UTF32LE
	};
	
	/** 
	 * Creates an UTF-encoding converter with specified encoding.
	 */
	explicit UTFConverter( EncodingType encoding );

	/** 
	 * Decodes bytes to single Unicode code point. 
	 * Ignores invalid input byte sequences.
	 *
	 * @param src Ptr to the source data.
	 * @param srcend Ptr to the end of source data.
	 * @param srcbytes [out] Receives number of bytes decoded.
	 * @param dst [out] Receives decoded Unicode code point (if byte sequence was not malformed).
	 * @return false if decoded byte sequence was malformed.
	 */
	bool	decode( const void* src, const void* srcend, int* srcbytes, int* dst ) const;

	/** 
	 * Encodes single Unicode code point to bytes. 
	 *
	 * @param dst Destination buffer which receives encoded byte sequence.
	 * @param dstend End of the destination buffer.
	 * @param dstbytes [out] Receives number of bytes encoded.
	 * @param src Unicode code point to encode.
	 * @return false if the code point couldn't be encoded.
	 */
	bool	encode( void* dst, void* dstend, int* dstbytes, int src ) const;

private:
	EncodingType	m_type;
};


END_NAMESPACE() // lang


#endif // _LANG_UTFCONVERTER_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
