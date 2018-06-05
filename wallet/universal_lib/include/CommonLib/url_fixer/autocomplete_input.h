// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_OMNIBOX_AUTOCOMPLETE_INPUT_H_
#define COMPONENTS_OMNIBOX_AUTOCOMPLETE_INPUT_H_

#include <string>

#include "../url/gurl.h"
#include "../url/url_parse.h"

// The user input for an autocomplete query.  Allows copying.
class AutocompleteInput {
 public:
  AutocompleteInput();
  // |text| and |cursor_position| represent the input query and location of
  // the cursor with the query respectively.  |cursor_position| may be set to
  // base::string16::npos if the input |text| doesn't come directly from the
  // user's typing.
  //
  // |desired_tld| is the user's desired TLD, if one is not already present in
  // the text to autocomplete.  When this is non-empty, it also implies that
  // "www." should be prepended to the domain where possible. The |desired_tld|
  // should not contain a leading '.' (use "com" instead of ".com").
  //
  AutocompleteInput(const base::string16& text,
                    size_t cursor_position,
                    const base::string16& desired_tld);
  ~AutocompleteInput();

  enum Type {
    INVALID = 0,
    UNKNOWN = 1,
    DEPRECATED_REQUESTED_URL = 2,
    URL = 3,
    QUERY = 4,
    FORCED_QUERY = 5
  };

  // If type is |FORCED_QUERY| and |text| starts with '?', it is removed.
  // Returns number of leading characters removed.
  static size_t RemoveForcedQueryStringIfNecessary(
      Type type,
      base::string16* text);

  // Converts |type| to a string representation.  Used in logging.
  static std::string TypeToString(Type type);

  // Parses |text| (including an optional |desired_tld|) and returns the type of
  // input this will be interpreted as.  |scheme_classifier| is used to check
  // the scheme in |text| is known and registered in the current environment.
  // The components of the input are stored in the output parameter |parts|, if
  // it is non-NULL. The scheme is stored in |scheme| if it is non-NULL. The
  // canonicalized URL is stored in |canonicalized_url|; however, this URL is
  // not guaranteed to be valid, especially if the parsed type is, e.g., QUERY.
  static Type Parse(
      const base::string16& text,
      const base::string16& desired_tld,
      url_parse::Parsed* parts,
      base::string16* scheme,
      GURL* canonicalized_url);

  // Parses |text| and fill |scheme| and |host| by the positions of them.
  // The results are almost as same as the result of Parse(), but if the scheme
  // is view-source, this function returns the positions of scheme and host
  // in the URL qualified by "view-source:" prefix.
  static void ParseForEmphasizeComponents(
      const base::string16& text,
      url_parse::Component* scheme,
      url_parse::Component* host);

  // Code that wants to format URLs with a format flag including
  // net::kFormatUrlOmitTrailingSlashOnBareHostname risk changing the meaning if
  // the result is then parsed as AutocompleteInput.  Such code can call this
  // function with the URL and its formatted string, and it will return a
  // formatted string with the same meaning as the original URL (i.e. it will
  // re-append a slash if necessary).  Because this uses Parse() under the hood
  // to determine the meaning of the different strings, callers need to supply a
  // |scheme_classifier| to pass to Parse().
  static base::string16 FormattedStringWithEquivalentMeaning(
      const GURL& url,
      const base::string16& formatted_url);

  // Returns the number of non-empty components in |parts| besides the host.
  static int NumNonHostComponents(const url_parse::Parsed& parts);

  // Returns whether |text| begins "http:" or "view-source:http:".
  static bool HasHTTPScheme(const base::string16& text);

  // User-provided text to be completed.
  const base::string16& text() const { return text_; }

  // Returns 0-based cursor position within |text_| or base::string16::npos if
  // not used.
  size_t cursor_position() const { return cursor_position_; }

  // Use of this setter is risky, since no other internal state is updated
  // besides |text_|, |cursor_position_| and |parts_|.  Only callers who know
  // that they're not changing the type/scheme/etc. should use this.
  void UpdateText(const base::string16& text,
                  size_t cursor_position,
                  const url_parse::Parsed& parts);

  // The type of input supplied.
  Type type() const { return type_; }

  // Returns parsed URL components.
  const url_parse::Parsed& parts() const { return parts_; }

  // The scheme parsed from the provided text; only meaningful when type_ is
  // URL.
  const base::string16& scheme() const { return scheme_; }

  // The input as an URL to navigate to, if possible.
  const GURL& canonicalized_url() const { return canonicalized_url_; }

  // Resets all internal variables to the null-constructed state.
  void Clear();

 private:
  // NOTE: Whenever adding a new field here, please make sure to update Clear()
  // method.
  base::string16 text_;
  size_t cursor_position_;
  Type type_;
  url_parse::Parsed parts_;
  base::string16 scheme_;
  GURL canonicalized_url_;
};

#endif  // COMPONENTS_OMNIBOX_AUTOCOMPLETE_INPUT_H_
