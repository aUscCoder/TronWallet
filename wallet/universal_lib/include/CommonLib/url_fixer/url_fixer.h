// NOLINT(all)
#pragma once

// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_URL_FIXER_URL_FIXER_H_
#define COMPONENTS_URL_FIXER_URL_FIXER_H_

#include <string>

#include "../url/url_export.h"
#include "../url/gurl.h"

namespace url {
struct Component;
struct Parsed;
}

// This object is designed to convert various types of input into URLs that we
// know are valid. For example, user typing in the URL bar or command line
// options. This is NOT the place for converting between different types of URLs
// or parsing them, see net_util.h for that.
namespace url_fixer {

extern const wchar_t kWhitespaceWide[];
extern const base::char16 kWhitespaceUTF16[];
extern const char kWhitespaceASCII[];

// Trims any whitespace from either end of the input string.  Returns where
// whitespace was found.
// The non-wide version has two functions:
// * TrimWhitespaceASCII()
//   This function is for ASCII strings and only looks for ASCII whitespace;
// Please choose the best one according to your usage.
// NOTE: Safe to use the same variable for both input and output.
enum TrimPositions {
  TRIM_NONE     = 0,
  TRIM_LEADING  = 1 << 0,
  TRIM_TRAILING = 1 << 1,
  TRIM_ALL      = TRIM_LEADING | TRIM_TRAILING,
};

class UnescapeRule {
public:
	// 来自chromium exscape.h
	// A combination of the following flags that is passed to the unescaping
	// functions.
	typedef uint32 Type;

	enum {
		// Don't unescape anything at all.
		NONE = 0,

		// Don't unescape anything special, but all normal unescaping will happen.
		// This is a placeholder and can't be combined with other flags (since it's
		// just the absence of them). All other unescape rules imply "normal" in
		// addition to their special meaning. Things like escaped letters, digits,
		// and most symbols will get unescaped with this mode.
		NORMAL = 1,

		// Convert %20 to spaces. In some places where we're showing URLs, we may
		// want this. In places where the URL may be copied and pasted out, then
		// you wouldn't want this since it might not be interpreted in one piece
		// by other applications.
		SPACES = 2,

		// Unescapes various characters that will change the meaning of URLs,
		// including '%', '+', '&', '/', '#'. If we unescaped these characters, the
		// resulting URL won't be the same as the source one. This flag is used when
		// generating final output like filenames for URLs where we won't be
		// interpreting as a URL and want to do as much unescaping as possible.
		URL_SPECIAL_CHARS = 4,

		// Unescapes control characters such as %01. This INCLUDES NULLs. This is
		// used for rare cases such as data: URL decoding where the result is binary
		// data. You should not use this for normal URLs!
		CONTROL_CHARS = 8,

		// URL queries use "+" for space. This flag controls that replacement.
		REPLACE_PLUS_WITH_SPACE = 16,
	};
};


TrimPositions TrimWhitespace(const base::string16& input,
                             TrimPositions positions,
                             base::string16* output);
TrimPositions TrimWhitespaceASCII(const std::string& input,
                                  TrimPositions positions,
                                  std::string* output);

// Deprecated. This function is only for backward compatibility and calls
// TrimWhitespaceASCII().
TrimPositions TrimWhitespace(const std::string& input,
                             TrimPositions positions,
                             std::string* output);

// Segments the given text string into parts of a URL. This is most useful for
// schemes such as http, https, and ftp where |SegmentURL| will find many
// segments. Currently does not segment "file" schemes.
// Returns the canonicalized scheme, or the empty string when |text| is only
// whitespace.
std::string SegmentURL(const std::string& text, url_parse::Parsed* parts);
base::string16 SegmentURL(const base::string16& text, url_parse::Parsed* parts);

// Converts |text| to a fixed-up URL and returns it. Attempts to make some
// "smart" adjustments to obviously-invalid input where possible.
// |text| may be an absolute path to a file, which will get converted to a
// "file:" URL.
//
// The result will be a "more" valid URL than the input. It may still not be
// valid, so check the return value's validity or use possibly_invalid_spec().
//
// Schemes "about" and "chrome" are normalized to "chrome://", with slashes.
// "about:blank" is unaltered, as Webkit allows frames to access about:blank.
// Additionally, if a chrome URL does not have a valid host, as in "about:", the
// returned URL will have the host "version", as in "chrome://version".
//
// If |desired_tld| is non-empty, it represents the TLD the user wishes to
// append in the case of an incomplete domain. We check that this is not a file
// path and there does not appear to be a valid TLD already, then append
// |desired_tld| to the domain and prepend "www." (unless it, or a scheme, are
// already present.)  This TLD should not have a leading '.' (use "com" instead
// of ".com").
GURL FixupURL(const std::string& text, const std::string& desired_tld);

// Offsets the beginning index of |part| by |offset|, which is allowed to be
// negative. In some cases, the desired component does not exist at the given
// offset. For example, when converting from "http://foo" to "foo", the scheme
// component no longer exists. In such a case, the beginning index is set to 0.
// Does nothing if |part| is invalid.
void OffsetComponent(int offset, url_parse::Component* part);

// Returns true if |scheme1| is equivalent to |scheme2|.
// Generally this is true if the two schemes are actually identical, but it's
// also true when one scheme is "about" and the other "chrome".
bool IsEquivalentScheme(const std::string& scheme1, const std::string& scheme2);

// An easy implementation to produce the formatted url for address bar.
base::string16 FormatUrl(const GURL& url,
						 const std::string& languages,
                         url_parse::Parsed* new_parsed,
                         size_t* prefix_end,
						 UnescapeRule::Type eType = UnescapeRule::NONE);

// This is a convenience function for FormatUrl() with
// format_types = kFormatUrlOmitAll and unescape = SPACES.  This is the typical
// set of flags for "URLs to display to the user".  You should be cautious about
// using this for URLs which will be parsed or sent to other applications.
inline base::string16 FormatUrl(const GURL& url, const std::string& languages, UnescapeRule::Type eType = UnescapeRule::NONE) {
	return FormatUrl(url, languages, NULL, NULL, eType);
}

base::string16 TransformHost(const std::string& host, const std::string& languages);
std::string UnescapeURLComponentByDefault(const std::string& url);

// For paths like ~, we use $HOME for the current user's home directory.
// For tests, we allow our idea of $HOME to be overriden by this variable.
extern const char* home_directory_override;

std::wstring FileURLToFilePath(const GURL& url);
}  // namespace url_fixer

#endif  // COMPONENTS_URL_FIXER_URL_FIXER_H_
