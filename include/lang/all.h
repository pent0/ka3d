#ifndef _LANG_ALL_H
#define _LANG_ALL_H


/**
 * @defgroup lang Language support library
 *
 * To provide better support for Unicode, typical game requirements and 
 * to avoid excessive compile-time dependencies, a custom low-level language 
 * support library lang is used instead of using C++ STL. The library uses 
 * only standard C functions in its implementation. Some of the classes are 
 * designed loosely after java.lang.* counterparts, which is also source 
 * for the name of the library.
 *
 * Most used class are template flexible size array class Array, Unicode 
 * string class String, reference counted base class Object and reference 
 * count updating pointer Ptr (or more commonly used macro P which maps to the NS(lang,Ptr))
 * Note that Object class is not thread safe, so you
 * cannot share any Object derived classes between threads.
 *
 * Array class is resizable linear container. Arrays are meant to be used
 * as part of higher abstraction level objects so 
 * they don't have functionality for reference counting.
 *
 * String class is a Unicode string, which uses UTF-8 data and internal string pool. 
 * This makes it possible to create/destroy variable length Unicode (UTF-8) strings 
 * without any run-time memory allocations in the application. String
 * is internally reference counted.
 *
 * Frequently used class is Debug too, which provides support for debug output.
 * Don't confuse this with the on-screen text rendering provided by NS(hgr,Console).
 *
 * Debug output can be viewed with IDE debugger or stand-alone debug output
 * viewer -- there are multiple available for free on Internet. This kind
 * of debug output has several benefits over traditional log files:
 * <ol>
 * <li>You get debug output visible already while running application without file access / reload issues.
 * <li>You always get up-to-date output, instead of accidentally looking at old debug output files.
 * <li>Debug output does not clutter up hard disk unless debug output viewer is active and logging enabled from the viewer.
 * <li>Debug output can be filtered, color coded, etc., marked, depending on debug output viewer application's features.
 * <li>Debug output viewer can provide support for remote debug output viewing on remote PC.
 * <ol>
 *
 * Performance profiling is important part of any game development. 
 * Profile is a light-weight and high resolution performance profiling class.
 *
 * Usage example:
 * \begin{verbatim}
 
   void myFunc() {
     PROFILE(myFunc);
     ...
   }
 
   main() {
     NS(Profile,beginFrame)();
     for ( int i = 0 ; i < 100000 ; ++i )
       myFunc();
     NS(Profile,endFrame)();
   
     for ( int i = 0 ; i < NS(Profile,count)() ; ++i )
       printf( "%s: x%d, %g%%\n", NS(Profile,getName)(i), NS(Profile,getCount)(i), NS(Profile,getPercent)(i) );
   }
 \end{verbatim}
 
 * 
 * Format is a class for constructing messages that keep parameters 
 * separately from the formatting string. This is useful 
 * to be used for example in translation.
 *
 * Syntax of format string (=first parameter of every constructor):
 * <ul>
 * <li>{0} = replaced with 1st argument formatted to string.
 * <li>{1} = replaced with 2nd argument formatted to string.
 * <li>{n} = replaced with nth argument formatted to string.
 * <li>{0,x} = replaced with 1st argument formatted to hex number.
 * <li>{0,#.##} = replaced with 1st argument formatted to number using max 2 digits, e.g. 1.2=1.2 but 1.211=1.21.
 * <li>{0,#.00} = replaced with 1st argument formatted to number using exactly 2 digits, e.g. 1.2=1.20 and 1.211=1.21.
 * <li>{0,000} = replaced with 1st argument formatted to number using at least 3 numbers, padded with zeros, e.g. 3=003.
 * </ul>
 * 
 * After constructing a Format object, it can be either formatted 
 * directly to text by using format() method, or the format string 
 * can be accessed or changed separately for translation.
 *
 * @{
 */

#include <lang/pp.h>
#include <lang/assert.h>
#include <lang/System.h>
#include <lang/MemoryPool.h>
#include <lang/Object.h>
#include <lang/Math.h>
#include <lang/String.h>
#include <lang/Array.h>
#include <lang/Ptr.h>
#include <lang/Float.h>
#include <lang/Integer.h>
#include <lang/Huffman16.h>
#include <lang/SingleLinkedList.h>
#include <lang/Throwable.h>
#include <lang/Exception.h>
#include <lang/Format.h>
#include <lang/MemoryPool.h>
#include <lang/Formattable.h>
#include <lang/Character.h>
#include <lang/Converter.h>
#include <lang/UTFConverter.h>
#include <lang/OutOfMemoryException.h>
#include <lang/FormatException.h>
#include <lang/Debug.h>
#include <lang/Profile.h>

/** @} */


#endif // _LANG_ALL_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
