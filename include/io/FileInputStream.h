#ifndef _IO_FILEINPUTSTREAM_H
#define _IO_FILEINPUTSTREAM_H


#include <io/InputStream.h>


#ifdef PLATFORM_NGI
#include <standardtypes.h>
#include <fileaccess.h>
#elif defined(PLATFORM_BREW)
#include <AEEShell.h>
#include <AEEFile.h>
#endif

BEGIN_NAMESPACE(io) 


/**
 * FileInputStream reads bytes from a standard file.
 * 
 * @ingroup io
 */
class FileInputStream :
	public InputStream
{
public:
	/** 
	 * Opens a file input stream. 
	 */
	explicit FileInputStream( const NS(lang,String)& filename );

	///
	~FileInputStream();

	/**
	 * Tries to read specified number of bytes from the stream.
	 * Doesn't block the caller if specified number of bytes isn't available.
	 *
	 * @return Number of bytes actually read.
	 */
	int				read( void* data, int size );

	/** 
	 * Returns the number of bytes that can be read from the stream without blocking.
	 */
	int				available() const;

	/**
	 * Returns name and seek position of the file.
	 */
	NS(lang,String)	toString() const;

private:

#ifdef PLATFORM_NGI
	// TODO: Changed NGI Implementation to use STD implementation
	// until performance issues have been resolved.
	//NS(ngi,uint32)		m_fileSize;
	//NS(ngi,uint32)		m_curPos;
	//NS(ngi,IFile)*		m_fh;
	void*			m_fh;
#elif defined(PLATFORM_PS2)
	int				m_fh;
#elif defined(PLATFORM_BREW)
	IFile *  m_fh;
#else
	void*			m_fh;
#endif
	NS(lang,String)	m_filename;

	FileInputStream();
	FileInputStream( const FileInputStream& );
	FileInputStream& operator=( const FileInputStream& );
};


END_NAMESPACE() // io


#endif // _IO_FILEINPUTSTREAM_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
