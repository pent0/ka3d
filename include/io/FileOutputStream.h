#ifndef _IO_FILEOUTPUTSTREAM_H
#define _IO_FILEOUTPUTSTREAM_H


#include <io/OutputStream.h>

#ifdef PLATFORM_NGI
#include <standardtypes.h>
#include <fileaccess.h>
#endif

BEGIN_NAMESPACE(io) 


/**
 * FileOutputStream writes bytes to a file in a file system.
 * 
 * @ingroup io
 */
class FileOutputStream :
	public OutputStream
{
public:
	/** 
	 * Opens a file output stream. 
	 * @exception IOException
	 */
	explicit FileOutputStream( const NS(lang,String)& filename );

	///
	~FileOutputStream();

	/**
	 * Writes specified number of bytes to the stream.
	 * @exception IOException
	 */
	void			write( const void* data, int size );

	/** Returns name of the file. */
	NS(lang,String)	toString() const;

private:

#ifdef PLATFORM_NGI
	// TODO: Changed NGI Implementation to use STD implementation
	// until performance issues have been resolved.
	//NS(ngi,IFile)*		m_fh;
	void*			m_fh;
#elif defined(PLATFORM_PS2)
	int				m_fh;
#else
	void*			m_fh;
#endif
	NS(lang,String) 	m_filename;

	FileOutputStream();
	FileOutputStream( const FileOutputStream& );
	FileOutputStream& operator=( const FileOutputStream& );
};


END_NAMESPACE() // io


#endif // _IO_FILEOUTPUTSTREAM_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
