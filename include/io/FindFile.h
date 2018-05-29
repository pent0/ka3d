#ifndef _IO_FINDFILE_H
#define _IO_FINDFILE_H


#include <io/PathName.h>
#include <lang/Object.h>
#include <time.h>


BEGIN_NAMESPACE(lang) 
	class String;END_NAMESPACE()


BEGIN_NAMESPACE(io) 


/**
 * FindFile iterates path names in a single directory or recursively in sub-directories. The class can be used to find files or directories by user-specified name or by user specified properties.
 * @ingroup io
 */
class FindFile :
	public NS(lang,Object)
{
public:
	/**
	 * Options for file finding.
	 */
	enum FindFlags
	{
		/** Recurses into subdirectories. This is default. */
		FIND_RECURSE	= 1,
		/** Directory names are not returned in iteration. This is default. */
		FIND_FILESONLY	= 2,
		/** Default options. */
		FIND_DEFAULT	= FIND_RECURSE|FIND_FILESONLY,
		
		/** Directories only. */
		FIND_DIRECTORIESONLY = 4
	};

	/**
	 * File attributes.
	 */
	enum AttribFlags
	{
		/** Normal. File has no other attributes set, and can be read or written to without restriction. */
		ATTRIB_NORMAL	= 0,
		/** Archive. Set whenever the file is changed. */
		ATTRIB_ARCHIVE	= 1,
		/** Hidden file. */ 
		ATTRIB_HIDDEN	= 2,
		/** Read-only. File cannot be written to, and a file with the same name cannot be created. */
		ATTRIB_READONLY	= 4,
		/** Subdirectory. */
		ATTRIB_SUBDIR	= 8,
		/** System file. */
		ATTRIB_SYSTEM	= 16,
	};

	/** 
	 * Description of a file.
	 */
	struct Data
	{
		/** File attributes, see AttribFlags. */
		int			attrib;
		/** Time when the file was last modified. */
		time_t		writeTime;
		/** Size of the file in bytes. */
		size_t		size;
		/** Path name of the file. */
		PathName	path;
	};

	/**
	 * Starts iterating through files with specified filter.
	 * Filter can include optional path name and wildcards in the filename,
	 * as in for example "D:/data/*.txt".
	 * If no files can be found then more() will return false.
	 * @param filter File name filter with optional initial (root) path.
	 * @param flags Filter flags, see FindFlags.
	 */
	explicit FindFile( const NS(lang,String)& filter, int flags=FIND_DEFAULT );

	/**
	 * Ends iteration.
	 */
	~FindFile();

	/**
	 * Iterates to the next file description if any.
	 */
	void			next();

	/**
	 * Returns true if there is more file descriptions available.
	 */
	bool			more() const;

	/**
	 * Returns description of current file in iteration.
	 */
	const Data&		data() const;

	/**
	 * Returns true if specified file exists. Accepts wild-cards.
	 * @param filter File name filter with optional initial (root) path.
	 * @param flags Filter flags, see FindFlags.
	 */
	static bool		isFileExist( const NS(lang,String)& filter, int flags=FIND_DEFAULT );

private:
	class Impl;
	P(Impl)	m_impl;

	FindFile( const FindFile& );
	FindFile& operator=( const FindFile& );
};


END_NAMESPACE() // io


#endif // _IO_FINDFILE_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
