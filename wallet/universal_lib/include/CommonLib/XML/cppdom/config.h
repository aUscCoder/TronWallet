/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil c-basic-offset: 3 -*- */
// vim:cindent:ts=3:sw=3:et:tw=80:sta:
/*************************************************************** cppdom-cpr beg
 * 
 * cppdom was forked from the original xmlpp version 0.6 under the LGPL. This
 * new, branched xmlpp is under the same LGPL (of course) and is being
 * maintained by:
 *      Kevin Meinert   <subatomic@users.sourceforge.net>
 *      Allen Bierbaum  <allenb@users.sourceforge.net>
 *      Ben Scott       <nonchocoboy@users.sourceforge.net>
 *
 * -----------------------------------------------------------------
 *
 * xmlpp - an xml parser and validator written in C++
 * copyright (c) 2000-2001 Michael Fink
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * -----------------------------------------------------------------
 * File:          $RCSfile$
 * Date modified: $Date: 2007-08-01 10:22:12 -0500 (Wed, 01 Aug 2007) $
 * Version:       $Revision: 623 $
 * -----------------------------------------------------------------
 *
 ************************************************************ cppdom-cpr-end */
/*! \file config.h

  system dependent stuff

*/

#ifndef CPPDOM_CONFIG_H
#define CPPDOM_CONFIG_H

// -----------------------------------
// win32
#if defined(WIN32) || defined(WIN64)

// switch some warnings off
#  pragma warning( disable: 4786 4275 4251 )

// export/import #define's for building a win32 dll
#  ifdef CPPDOM_EXPORTS
//#     define CPPDOM_EXPORT(ret)  __declspec(dllexport) ret __stdcall
#     define CPPDOM_EXPORT(ret)  __declspec(dllexport) ret
#     define CPPDOM_CLASS        __declspec(dllexport)
#  else
// Only define the dllimport bits if dynamic linking has been requested.
#     if defined(CPPDOM_DYN_LINK)
//#        define CPPDOM_EXPORT(ret)  __declspec(dllimport) ret __stdcall
#        define CPPDOM_EXPORT(ret)  __declspec(dllimport) ret
#        define CPPDOM_CLASS        __declspec(dllimport)
#     else
#        define CPPDOM_EXPORT(ret) ret
#        define CPPDOM_CLASS
#     endif

// Use automatic linking when building with Visual C++ and when requested to
// do so. Define either CPPDOM_AUTO_LINK or CPPDOM_DYN_LINK to enable automatic
// linking. The latter specifies that the dynamic library is the one to link
// against automatically. If CPPDOM_DYN_LINK is not defined when using
// automatic linking, then the static library is used.
#     if defined(_MSC_VER) && \
            (defined(CPPDOM_AUTO_LINK) || defined(CPPDOM_DYN_LINK))
#        include "version.h"

#        define CPPDOM_STRINGIZE(X) CPPDOM_DO_STRINGIZE(X)
#        define CPPDOM_DO_STRINGIZE(X) #X
#        define CPPDOM_VERSION_STR CPPDOM_STRINGIZE(CPPDOM_VERSION_MAJOR) "_" \
                                   CPPDOM_STRINGIZE(CPPDOM_VERSION_MINOR) "_" \
                                   CPPDOM_STRINGIZE(CPPDOM_VERSION_PATCH)

// Defining _DEBUG implies the need for CPPDOM_DEBUG.
#        if defined(_DEBUG) && ! defined(CPPDOM_DEBUG)
#           define CPPDOM_DEBUG
#        endif

#        if defined(CPPDOM_DEBUG) && defined(_DEBUG)
#           define CPPDOM_LIB_RT_OPT "_d"
#        else
#           define CPPDOM_LIB_RT_OPT ""
#        endif

#        if defined(CPPDOM_DYN_LINK)
#           define CPPDOM_LIB_LINK_OPT ""
#        else
#           define CPPDOM_LIB_LINK_OPT "_s"
#        endif

#        pragma comment(lib, "cppdom" CPPDOM_LIB_RT_OPT CPPDOM_LIB_LINK_OPT "-" CPPDOM_VERSION_STR ".lib")

#        undef CPPDOM_LIB_LINK_OPT
#        undef CPPDOM_LIB_RT_OPT
#        undef CPPDOM_VERSION_STR
#        undef CPPDOM_DO_STRINGIZE
#        undef CPPDOM_STRINGIZE
#     endif /* defined(_MSC_VER) && (defined(CPPDOM_AUTO_LINK) || defined(CPPDOM_DYN_LINK)) */
#  endif /* defined(CPPDOM_EXPORTS) */

// Non-Windows handling.
#else
#  define CPPDOM_EXPORT(ret) ret
#  define CPPDOM_CLASS
#endif

// -----------------------------------
#endif
