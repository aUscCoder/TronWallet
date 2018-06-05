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
 * Date modified: $Date: 2007-07-25 17:02:45 -0500 (Wed, 25 Jul 2007) $
 * Version:       $Revision: 619 $
 * -----------------------------------------------------------------
 *
 ************************************************************ cppdom-cpr-end */
/** \file xmlparser.h

  definitions for the parsing classes

*/

// prevent multiple includes
#ifndef CPPDOM_XML_PARSER_H
#define CPPDOM_XML_PARSER_H

// needed includes
#include "cppdom.h"
#include "xmltokenizer.h"

// namespace declaration
namespace cppdom
{

   /** xml parser implementation class */
   class Parser
   {
   public:
      /** ctor */
      Parser(const char *lpszData, Location& loc);

      /** parses the node as the document root */
      bool parseDocument(Document& doc, ContextPtr& context);

      /** parses a node, without processing instructions */
      bool parseNode(Node& node, ContextPtr& context);

   protected:
      /** parses xml header, such as processing instructions, doctype etc. */
      bool parseHeader(Document& doc, ContextPtr& context);

      /** parses an xml tag attribute list */
      bool parseAttributes(Attributes& attr);

      /**
       * parses a <!-- --> comment
       *
       * @pre The <!-- part of the comment has already been consumed.
       */
      void parseComment(ContextPtr& context);   

   protected:

      /** stream iterator */
      xmlstream_iterator mTokenizer;
   };
}

#endif
