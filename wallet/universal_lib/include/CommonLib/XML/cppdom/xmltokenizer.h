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
 * Date modified: $Date: 2003-01-06 17:53:55 -0600 (Mon, 06 Jan 2003) $
 * Version:       $Revision: 218 $
 * -----------------------------------------------------------------
 *
 ************************************************************ cppdom-cpr-end */
/*! \file xmltokenizer.h

  the stream tokenizer class

*/

// prevent multiple includes
#ifndef CPPDOM_XML_TOKENIZER_H
#define CPPDOM_XML_TOKENIZER_H

// needed includes
#include <string>
#include <stack>
#include <iosfwd>


// namespace declaration
namespace cppdom
{
   /// xml token
   /** an Token is a representation for a literal character or a
       generic std::string (not recognized as a literal) */
   class Token
   {
      friend class xmlstream_iterator;
   public:
      Token();
      Token(char ch);
      Token(const std::string& str);

      /// returns if token is a literal
      bool isLiteral() const;

      /// returns if it is and end of xml stream token
      bool isEndOfStream() const;

      /// returns literal char
      char getLiteral() const;

      /// returns generic std::string
      const std::string& getGeneric() const;

      // operators

      /// compare operator for literals
      bool operator==(char ch) const;

      /// compare operator for literals
      bool operator!=(char ch) const;

      /// compare operator for a generic std::string
      bool operator==(const std::string& str) const;

      /// compare operator for a generic std::string
      bool operator!=(const std::string& str) const;

      /// set generic std::string
      Token& operator=(const std::string& str);

      /// set literal char
      Token& operator=(char ch);

   protected:
      /// indicates if token is a literal char
      bool mIsLiteral;

      /// literal
      char mLiteral;

      /// pointer to std::string
      std::string mGeneric;
   };


   /// base tokenizer class
   /** base class for iterating through Token */
   class Tokenizer
   {
   public:
      /** constructor */
      Tokenizer(const char *lpszData, Location& loc);
      virtual ~Tokenizer();

      /// dereference operator
      Token& operator*();

      /// pointer access operator
      const Token* operator->();

      /// advances in the xml stream
      Tokenizer& operator++();

      /// advances in the xml stream
      Tokenizer& operator++(int);

      /// returns current token
      Token& get();

      /// puts the token back into the stream
      void putBack(Token& token);

      /// puts the last token back into the stream
      void putBack();

   protected:
      /// internal: parses the next token
      virtual void getNext() = 0;

      // data members

      /** input stream */
	  const char *m_lpszData;
	  int m_iPosition;

      /** location in the stream */
      Location& mLocation;

      /** current token */
      Token mCurToken;

      /** stack for put_back()'ed tokens */
      std::stack<Token> mTokenStack;
   };

   /**
    * xml input stream iterator
    * an iterator through all Token contained in the xml input stream
    */
   class xmlstream_iterator : public Tokenizer
   {
   public:
      /** ctor */
      xmlstream_iterator(const char *lpszData, Location& loc);

   protected:
      void getNext();

      // internally used to recognize chars in the stream
      bool isLiteral(char c);
      bool isWhiteSpace(char c);
      bool isNewLine(char c);
      bool isStringDelimiter(char c); // start-/endchar of a std::string

      /** cdata-mode doesn't care for whitespaces in generic strings */
      bool mCdataMode;

      /** char which was put back internally */
      char mPutbackChar;
   };

   /**
    * dtd input stream iterator
    * an iterator through a dtd input stream
    */
   class xmldtd_iterator: public Tokenizer
   {
   public:
      /** ctor */
      xmldtd_iterator(std::istream& in, Location& loc);

   protected:
      void getNext(){}
   };
}

#endif
