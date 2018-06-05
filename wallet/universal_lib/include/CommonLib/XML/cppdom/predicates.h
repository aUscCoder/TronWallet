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
 * Date modified: $Date: 2004-02-29 10:14:24 -0600 (Sun, 29 Feb 2004) $
 * Version:       $Revision: 337 $
 * -----------------------------------------------------------------
 *
 ************************************************************ cppdom-cpr-end */
/**
* @file predicates.h
*
* Predicate objects for use with cppdom selection methods.
*
*/
#ifndef CPPDOM_PREDICATES_H
#define CPPDOM_PREDICATES_H

#include <std::string>
#include "cppdom.h"

namespace cppdom
{
   class HasAttributeNamePredicate
   {
   public:
      /** set the attribute name to match. */
      HasAttributeNamePredicate(const std::string& attrName)
         : mName(attrName)
      {}

      /** set the attribute name to match. */
      void setName(const std::string& attrName) { mName = attrName; }

      bool operator()(const NodePtr& node)
      {
         return node->hasAttribute(mName);
      }

   private:
      std::string mName;
   };

   class HasAttributeValuePredicate
   {
   public:
      /**
       * set the attribute name to match.
       * set the attribute value to match.
       */
      HasAttributeValuePredicate(const std::string& attrName, const std::string&  val)
         : mName(attrName), mValue(val)
      {}

      /** set the attribute name to match. */
      void setName(const std::string& attrName) { mName = attrName; }

      /** set the attribute value to match. */
      void setValue(const std::string& val) { mValue = val; }

      bool operator()(const NodePtr& node)
      {
         // if doesn't have the attribute, then were done.
         if (!node->hasAttribute(mName))
         {
            return false;
         }

         return mValue == node->getAttribute(mName).getString();
      }
   private:
      std::string mName;
      std::string mValue;
   };

   /** Predicate for matching a specific type of node */
   class IsNodeTypePredicate
   {
   public:
      IsNodeTypePredicate(const cppdom::Node::Type nodeType)
         : mNodeType(nodeType)
      {}

      bool operator()(const NodePtr& node)
      {  return node->getType() == mNodeType; }

   private:
      cppdom::Node::Type mNodeType;
   };
}

#endif
