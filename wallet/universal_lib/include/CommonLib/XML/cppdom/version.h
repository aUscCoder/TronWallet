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
 * Date modified: $Date: 2010-03-06 11:08:16 -0600 (Sat, 06 Mar 2010) $
 * Version:       $Revision: 648 $
 * -----------------------------------------------------------------
 *
 ************************************************************ cppdom-cpr-end */
#ifndef CPPDOM_VERSION_H
#define CPPDOM_VERSION_H

/** @file version.h
 * This file contains two useful items.
 *   - 1. The preprocessor friendly CPPDOM_VERSION "std::string". It is in the form
 *       <major><minor><patch> where each part has exactly 3 digits.
 *   - 2. The C++ friendly variable, version, that contains the version as a
 *       std::string. It is in the form of <major>.<minor>.<patch> where each part
 *       has anywhere from 1 to 3 digits.
 */

/**
 * This is the "human-readable" CPPDOM version _string_. It is of the form
 * <major><minor><patch>. Each part has exactly 3 digits.
 */
// The major/minor/patch version (up to 3 digits each).
#define CPPDOM_VERSION_MAJOR     1
#define CPPDOM_VERSION_MINOR     0
#define CPPDOM_VERSION_PATCH     1

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// To update the version number, do not modify anything below this line!!!!
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
// Define the helper macros
//--------------------------------------------------------------------------

// These helper macros are used build up the CPPDOM_VERSION macro
#define CPPDOM_GLUE(a,b)         a ## b
#define CPPDOM_XGLUE(a,b)        CPPDOM_GLUE(a,b)

// These helpr macros are used to facilitate a zero left fill
#define CPPDOM_ZEROFILL(a)       0 ## a
#define CPPDOM_XZEROFILL(a)      CPPDOM_ZEROFILL(a)

// Fix up the major version by doing a zero left fill
#if CPPDOM_VERSION_MAJOR < 10
#  define CPPDOM_VERSION_MAJOR_FILLED \
               CPPDOM_XZEROFILL(CPPDOM_XZEROFILL(CPPDOM_VERSION_MAJOR))
#elif CPPDOM_VERSION_MAJOR < 100
#  define CPPDOM_VERSION_MAJOR_FILLED CPPDOM_XZEROFILL(CPPDOM_VERSION_MAJOR)
#else
#  define CPPDOM_VERSION_MAJOR_FILLED CPPDOM_VERSION_MAJOR
#endif

// Fix up the minor version by doing a zero left fill
#if CPPDOM_VERSION_MINOR < 10
#  define CPPDOM_VERSION_MINOR_FILLED \
               CPPDOM_XZEROFILL(CPPDOM_XZEROFILL(CPPDOM_VERSION_MINOR))
#elif CPPDOM_VERSION_MINOR < 100
#  define CPPDOM_VERSION_MINOR_FILLED CPPDOM_XZEROFILL(CPPDOM_VERSION_MINOR)
#else
#  define CPPDOM_VERSION_MINOR_FILLED CPPDOM_VERSION_MINOR
#endif

// Fix up the patch version by doing a zero left fill
#if CPPDOM_VERSION_PATCH < 10
#  define CPPDOM_VERSION_PATCH_FILLED \
               CPPDOM_XZEROFILL(CPPDOM_XZEROFILL(CPPDOM_VERSION_PATCH))
#elif CPPDOM_VERSION_PATCH < 100
#  define CPPDOM_VERSION_PATCH_FILLED CPPDOM_XZEROFILL(CPPDOM_VERSION_PATCH)
#else
#  define CPPDOM_VERSION_PATCH_FILLED CPPDOM_VERSION_PATCH
#endif

//--------------------------------------------------------------------------
// Define the CPPDOM_VERSION macro
//--------------------------------------------------------------------------

// Build up the CPPDOM_VERSION macro by pasting the individual parts together
/**
 * The is the preprocessor-friendly version std::string. It is in the form of
 * <major><minor><patch>. Each part has exactly 3 digits.
 */
#define CPPDOM_VERSION \
   CPPDOM_XGLUE( \
      CPPDOM_XGLUE(CPPDOM_VERSION_MAJOR_FILLED, CPPDOM_VERSION_MINOR_FILLED), \
      CPPDOM_VERSION_PATCH_FILLED \
   )

//--------------------------------------------------------------------------
// Cleanup after ourselves and undef all internal macros.
//--------------------------------------------------------------------------

// Undef the all helper macros
#undef CPPDOM_XGLUE
#undef CPPDOM_GLUE
#undef CPPDOM_ZEROFILL
#undef CPPDOM_XZEROFILL

// Undef the XXX_FILLED temporary macros
#undef CPPDOM_VERSION_MAJOR_FILLED
#undef CPPDOM_VERSION_MINOR_FILLED
#undef CPPDOM_VERSION_PATCH_FILLED

#endif
