#ifndef _IO_ALL_H
#define _IO_ALL_H


/**
 * @defgroup io Input/output library
 *
 * Input/output stream classes and PathName class for file and directory
 * name string manipulation and FindFile for finding files or directories. 
 * The stream classes have been modeled closely after java.io.* counterparts.
 *
 * Common combination of stream usage is to use FileInputStream class
 * with DataInputStream. For example to read data in platform- and 
 * endianess independent manner:
 *
 * P(FileInputStream) filein = new FileInputStream( "myfile.dat" );<br>
 * P(DataInputStream) datain = new DataInputStream( filein );<br>
 * String mystring = datain->readUTF();<br>
 * int myint = datain->readInt();<br>
 * float myfloat = datain->readFloat();<br>
 * 
 * Writing to the files would go exactly wise versa, so that DataInputStream
 * would be replaced with DataOutputStream, readInt() with writeInt(x), etc.
 *
 * PathName class is convenient for path name manipulation as name suggests.
 * For example if you have a directory name and you want to concatenate
 * a file name to it, you can write PathName(dirname,fname) without
 * need to check does the directory name already end with slash or backslash.
 * You can also retrieve directory name from file name by calling parent()
 * method, for example in a situation where you want to load textures
 * from the same directory as the filename.
 *
 * FindFile iterates path names in a single directory or recursively in sub-directories. The class can be used to find files or directories by user-specified name or by user specified properties.
 *
 * @{
 */

#include <io/ByteArrayInputStream.h>
#include <io/ByteArrayOutputStream.h>
#include <io/CommandLineParser.h>
#include <io/DataInput.h>
#include <io/DataInputStream.h>
#include <io/DataOutput.h>
#include <io/DataOutputStream.h>
#include <io/FileInputStream.h>
#include <io/FileNotFoundException.h>
#include <io/FileOutputStream.h>
#include <io/FilterInputStream.h>
#include <io/FilterOutputStream.h>
#include <io/FindFile.h>
#include <io/InputStream.h>
#include <io/IOException.h>
#include <io/OutputStream.h>
#include <io/PathName.h>
#include <io/PropertyParser.h>

/** @} */


#endif // _IO_ALL_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
