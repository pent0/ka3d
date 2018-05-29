#ifndef WIN32
#error FindFile.cpp Win32 implementation but no WIN32 defined
#endif

#include <io/FindFile.h>
#include <lang/Array.h>
#include <lang/String.h>
#include <io.h>
#include <string.h>
#include <config.h>


USING_NAMESPACE(lang)


BEGIN_NAMESPACE(io) 


class FindFile::Impl :
	public Object
{
public:
	Impl( String filter, int flags ) :
		m_filter( String(PathName(filter).basename()) + String(PathName(filter).suffix()) ),
		m_path( PathName(filter).parent().toString() ),
		m_flags( flags ),
		m_subdirs(),
		m_data(),
		m_fd(),
		m_fh( -1 )
	{
		if ( flags & FIND_RECURSE )
			listSubDirs( m_path );

		m_subdirs.add( m_path );
		next();
	}

	~Impl()
	{
		if ( m_fh != -1 )
			_findclose( m_fh );
	}

	void next()
	{
		if ( m_fh != -1 && 0 == _findnext(m_fh,&m_fd) )
		{
			updateData();
		}
		else
		{
			if ( m_fh != -1 )
			{
				_findclose( m_fh );
				m_fh = -1;
			}

			while ( m_subdirs.size() > 0 && m_fh == -1 )
			{
				m_path = m_subdirs.last();
				m_subdirs.resize( m_subdirs.size()-1 );

				char buf[PathName::MAXLEN];
				String::cpy( buf, sizeof(buf), PathName(m_path,m_filter).toString() );
				m_fh = _findfirst( buf, &m_fd );

				updateData();
			}
		}
	}

	bool more() const
	{
		return m_fh != -1 || m_subdirs.size() > 0;
	}

	const Data&	data() const
	{
		return m_data;
	}

private:
	String			m_filter;
	String			m_path;
	int				m_flags;
	Array<String>	m_subdirs;
	Data			m_data;
	_finddata_t		m_fd;
	intptr_t		m_fh;


	void updateData()
	{
		if ( m_fh == -1 )
			return;

		m_data.attrib = 0;
		if ( (m_fd.attrib & _A_ARCH) == _A_ARCH )
			m_data.attrib |= ATTRIB_ARCHIVE;
		if ( (m_fd.attrib & _A_HIDDEN) == _A_HIDDEN )
			m_data.attrib |= ATTRIB_HIDDEN;
		if ( (m_fd.attrib & _A_RDONLY) == _A_RDONLY )
			m_data.attrib |= ATTRIB_READONLY;
		if ( (m_fd.attrib & _A_SUBDIR) == _A_SUBDIR )
			m_data.attrib |= ATTRIB_SUBDIR;
		if ( (m_fd.attrib & _A_SYSTEM) == _A_SYSTEM )
			m_data.attrib |= ATTRIB_SYSTEM;

		m_data.writeTime = m_fd.time_write;
		m_data.size = m_fd.size;
		m_data.path = PathName(m_path,m_fd.name);
	}

	void listSubDirs( String path )
	{
		char buf[PathName::MAXLEN];
		String::cpy( buf, sizeof(buf), PathName(path,"*.*").toString() );

		_finddata_t fd;
		intptr_t fh = _findfirst( buf, &fd );

		if ( fh != -1 )
		{
			const int oldsubdirs = m_subdirs.size();
			do
			{
				if ( (fd.attrib & _A_SUBDIR) != 0 && 
					strcmp(".",fd.name) != 0 && 
					strcmp("..",fd.name) != 0 )
				{
					m_subdirs.add( PathName(path,fd.name).toString() );
				}
			} while ( 0 == _findnext(fh,&fd) );
			_findclose( fh );

			const int lastsubdir = m_subdirs.size();
			for ( int i = oldsubdirs ; i < lastsubdir ; ++i )
				listSubDirs( m_subdirs[i] );
		}
	}
};


FindFile::FindFile( const String& filter, int flags )
{
	m_impl = new Impl( filter, flags );
}

FindFile::~FindFile()
{
}

void FindFile::next()
{
	m_impl->next();
}

bool FindFile::more() const
{
	return m_impl->more();
}

const FindFile::Data&	FindFile::data() const
{
	return m_impl->data();
}

bool FindFile::isFileExist( const String& filter, int flags )
{
	FindFile ff( filter, flags );
	return ff.more();
}


END_NAMESPACE() // io
