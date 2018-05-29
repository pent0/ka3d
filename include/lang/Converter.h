#ifndef _LANG_CONVERTER_H
#define _LANG_CONVERTER_H


#include <lang/pp.h>
#include <lang/Object.h>


BEGIN_NAMESPACE(lang) 


/**
 * Character encoding/decoding interface.
 * Converts to/from Unicode code points.
 *
 * 
 * @ingroup lang
 */
class Converter :
	public NS(lang,Object)
{
public:
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
	virtual bool	decode( const void* src, const void* srcend, int* srcbytes, int* dst ) const = 0;

	/** 
	 * Encodes single Unicode code point to bytes. 
	 *
	 * @param dst Destination buffer which receives encoded byte sequence.
	 * @param dstend End of the destination buffer.
	 * @param dstbytes [out] Receives number of bytes encoded.
	 * @param src Unicode code point to encode.
	 * @return false if the code point couldn't be encoded.
	 */
	virtual bool	encode( void* dst, void* dstend, int* dstbytes, int src ) const = 0;
};


END_NAMESPACE() // lang


#endif // _LANG_CONVERTER_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
