// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "url_fixer.h"

#include <algorithm>
#include "../String/Convert.h"
#include "../tld/registry_controlled_domain.h"
#include "../url/url_file.h"
#include "../url/url_parse.h"
#include "../url/url_util.h"
#include "../url/url_util_internal.h"
#include "../Misc.h"

const char* url_fixer::home_directory_override = NULL;

#define WHITESPACE_UNICODE \
  0x0009, /* CHARACTER TABULATION */      \
  0x000A, /* LINE FEED (LF) */            \
  0x000B, /* LINE TABULATION */           \
  0x000C, /* FORM FEED (FF) */            \
  0x000D, /* CARRIAGE RETURN (CR) */      \
  0x0020, /* SPACE */                     \
  0x0085, /* NEXT LINE (NEL) */           \
  0x00A0, /* NO-BREAK SPACE */            \
  0x1680, /* OGHAM SPACE MARK */          \
  0x2000, /* EN QUAD */                   \
  0x2001, /* EM QUAD */                   \
  0x2002, /* EN SPACE */                  \
  0x2003, /* EM SPACE */                  \
  0x2004, /* THREE-PER-EM SPACE */        \
  0x2005, /* FOUR-PER-EM SPACE */         \
  0x2006, /* SIX-PER-EM SPACE */          \
  0x2007, /* FIGURE SPACE */              \
  0x2008, /* PUNCTUATION SPACE */         \
  0x2009, /* THIN SPACE */                \
  0x200A, /* HAIR SPACE */                \
  0x2028, /* LINE SEPARATOR */            \
  0x2029, /* PARAGRAPH SEPARATOR */       \
  0x202F, /* NARROW NO-BREAK SPACE */     \
  0x205F, /* MEDIUM MATHEMATICAL SPACE */ \
  0x3000, /* IDEOGRAPHIC SPACE */         \
  0

namespace url_fixer {
const wchar_t kWhitespaceWide[] = {
  WHITESPACE_UNICODE
};

const base::char16 kWhitespaceUTF16[] = {
  WHITESPACE_UNICODE
};

const char kWhitespaceASCII[] = {
  0x09,    // CHARACTER TABULATION
  0x0A,    // LINE FEED (LF)
  0x0B,    // LINE TABULATION
  0x0C,    // FORM FEED (FF)
  0x0D,    // CARRIAGE RETURN (CR)
  0x20,    // SPACE
  0
};



template<typename STR>
TrimPositions TrimStringT(const STR& input,
                          const typename STR::value_type trim_chars[],
                          TrimPositions positions,
                          STR* output) {
  // Find the edges of leading/trailing whitespace as desired.
  const typename STR::size_type last_char = input.length() - 1;
  const typename STR::size_type first_good_char = (positions & TRIM_LEADING) ?
      input.find_first_not_of(trim_chars) : 0;
  const typename STR::size_type last_good_char = (positions & TRIM_TRAILING) ?
      input.find_last_not_of(trim_chars) : last_char;

  // When the string was all whitespace, report that we stripped off whitespace
  // from whichever position the caller was interested in.  For empty input, we
  // stripped no whitespace, but we still need to clear |output|.
  if (input.empty() ||
      (first_good_char == STR::npos) || (last_good_char == STR::npos)) {
    bool input_was_empty = input.empty();  // in case output == &input
    output->clear();
    return input_was_empty ? TRIM_NONE : positions;
  }

  // Trim the whitespace.
  *output =
      input.substr(first_good_char, last_good_char - first_good_char + 1);

  // Return where we trimmed from.
  return static_cast<TrimPositions>(
      ((first_good_char == 0) ? TRIM_NONE : TRIM_LEADING) |
      ((last_good_char == last_char) ? TRIM_NONE : TRIM_TRAILING));
}

bool TrimString(const base::string16& input,
                const base::char16 trim_chars[],
                base::string16* output) {
  return TrimStringT(input, trim_chars, TRIM_ALL, output) != TRIM_NONE;
}

bool TrimString(const std::string& input,
                const char trim_chars[],
                std::string* output) {
  return TrimStringT(input, trim_chars, TRIM_ALL, output) != TRIM_NONE;
}

TrimPositions TrimWhitespace(const base::string16& input,
                             TrimPositions positions,
                             base::string16* output) {
  return TrimStringT(input, kWhitespaceUTF16, positions, output);
}

TrimPositions TrimWhitespaceASCII(const std::string& input,
                                  TrimPositions positions,
                                  std::string* output) {
  return TrimStringT(input, kWhitespaceASCII, positions, output);
}

// This function is only for backward-compatibility.
// To be removed when all callers are updated.
TrimPositions TrimWhitespace(const std::string& input,
                             TrimPositions positions,
                             std::string* output) {
  return TrimWhitespaceASCII(input, positions, output);
}

TrimPositions TrimWhitespaceUTF8(const std::string& input,
                                 TrimPositions positions,
                                 std::string* output) {
  // This implementation is not so fast since it converts the text encoding
  // twice. Please feel free to file a bug if this function hurts the
  // performance of Chrome.
  base::string16 input16 = AToW(input);
  base::string16 output16;
  TrimPositions result =
      TrimWhitespace(input16, positions, &output16);
  *output = WToA(output16);
  return result;
}

template <typename Char>
inline bool IsHexDigit(Char c) {
  return (c >= '0' && c <= '9') ||
    (c >= 'A' && c <= 'F') ||
    (c >= 'a' && c <= 'f');
}

template <typename Char>
inline Char HexDigitToInt(Char c) {
  DCHECK(IsHexDigit(c));
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  return 0;
}


template<typename STR>
bool UnescapeUnsignedCharAtIndex(const STR& escaped_text,
  size_t index,
  unsigned char* value) {
  if ((index + 2) >= escaped_text.size())
    return false;
  if (escaped_text[index] != '%')
    return false;
  const typename STR::value_type most_sig_digit(
    static_cast<typename STR::value_type>(escaped_text[index + 1]));
  const typename STR::value_type least_sig_digit(
    static_cast<typename STR::value_type>(escaped_text[index + 2]));
  if (IsHexDigit(most_sig_digit) && IsHexDigit(least_sig_digit)) {
    *value = HexDigitToInt(most_sig_digit) * 16 +
      HexDigitToInt(least_sig_digit);
    return true;
  }
  return false;
}

const char kUrlUnescape[128] = {
  //   NULL, control chars...
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  //  ' ' !  "  #  $  %  &  '  (  )  *  +  ,  -  .  /
  0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
  //   0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ?
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0,
  //   @  A  B  C  D  E  F  G  H  I  J  K  L  M  N  O
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  //   P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,
  //   `  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  //   p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~  <NBSP>
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0
};

template<typename STR>
STR UnescapeURLWithAdjustmentsImpl(
  const STR& escaped_text,
  UnescapeRule::Type rules) {
  // Do not unescape anything, return the |escaped_text| text.
  if (rules == UnescapeRule::NONE)
    return escaped_text;

  // The output of the unescaping is always smaller than the input, so we can
  // reserve the input size to make sure we have enough buffer and don't have
  // to allocate in the loop below.
  STR result;
  result.reserve(escaped_text.length());

  // Locations of adjusted text.
  for (size_t i = 0, max = escaped_text.size(); i < max; ++i) {
    if (static_cast<unsigned char>(escaped_text[i]) >= 128) {
      // Non ASCII character, append as is.
      result.push_back(escaped_text[i]);
      continue;
    }

    unsigned char first_byte;
    if (UnescapeUnsignedCharAtIndex(escaped_text, i, &first_byte)) {
      // Per http://tools.ietf.org/html/rfc3987#section-4.1, the following BiDi
      // control characters are not allowed to appear unescaped in URLs:
      //
      // U+200E LEFT-TO-RIGHT MARK         (%E2%80%8E)
      // U+200F RIGHT-TO-LEFT MARK         (%E2%80%8F)
      // U+202A LEFT-TO-RIGHT EMBEDDING    (%E2%80%AA)
      // U+202B RIGHT-TO-LEFT EMBEDDING    (%E2%80%AB)
      // U+202C POP DIRECTIONAL FORMATTING (%E2%80%AC)
      // U+202D LEFT-TO-RIGHT OVERRIDE     (%E2%80%AD)
      // U+202E RIGHT-TO-LEFT OVERRIDE     (%E2%80%AE)
      //
      // Additionally, the Unicode Technical Report (TR9) as referenced by RFC
      // 3987 above has since added some new BiDi control characters.
      // http://www.unicode.org/reports/tr9
      //
      // U+061C ARABIC LETTER MARK         (%D8%9C)
      // U+2066 LEFT-TO-RIGHT ISOLATE      (%E2%81%A6)
      // U+2067 RIGHT-TO-LEFT ISOLATE      (%E2%81%A7)
      // U+2068 FIRST STRONG ISOLATE       (%E2%81%A8)
      // U+2069 POP DIRECTIONAL ISOLATE    (%E2%81%A9)

      unsigned char second_byte;
      // Check for ALM.
      if ((first_byte == 0xD8) &&
        UnescapeUnsignedCharAtIndex(escaped_text, i + 3, &second_byte) &&
        (second_byte == 0x9c)) {
        result.append(escaped_text, i, 6);
        i += 5;
        continue;
      }

      // Check for other BiDi control characters.
      if ((first_byte == 0xE2) &&
        UnescapeUnsignedCharAtIndex(escaped_text, i + 3, &second_byte) &&
        ((second_byte == 0x80) || (second_byte == 0x81))) {
        unsigned char third_byte;
        if (UnescapeUnsignedCharAtIndex(escaped_text, i + 6, &third_byte) &&
          ((second_byte == 0x80) ?
          ((third_byte == 0x8E) || (third_byte == 0x8F) ||
          ((third_byte >= 0xAA) && (third_byte <= 0xAE))) :
          ((third_byte >= 0xA6) && (third_byte <= 0xA9)))) {
          result.append(escaped_text, i, 9);
          i += 8;
          continue;
        }
      }

      if (first_byte >= 0x80 ||  // Unescape all high-bit characters.
        // For 7-bit characters, the lookup table tells us all valid chars.
        (kUrlUnescape[first_byte] ||
        // ...and we allow some additional unescaping when flags are set.
        (first_byte == ' ' && (rules & UnescapeRule::SPACES)) ||
        // Allow any of the prohibited but non-control characters when
        // we're doing "special" chars.
        (first_byte > ' ' && (rules & UnescapeRule::URL_SPECIAL_CHARS)) ||
        // Additionally allow control characters if requested.
        (first_byte < ' ' && (rules & UnescapeRule::CONTROL_CHARS)))) {
        // Use the unescaped version of the character.
        result.push_back(first_byte);
        i += 2;
      }
      else {
        // Keep escaped. Append a percent and we'll get the following two
        // digits on the next loops through.
        result.push_back('%');
      }
    }
    else if ((rules & UnescapeRule::REPLACE_PLUS_WITH_SPACE) &&
      escaped_text[i] == '+') {
      result.push_back(' ');
    }
    else {
      // Normal case for unescaped characters.
      result.push_back(escaped_text[i]);
    }
  }
  return result;
}

std::string UnescapeURLComponent(const std::string& escaped_text,
  UnescapeRule::Type rules) {
  return UnescapeURLWithAdjustmentsImpl(escaped_text, rules);
}

std::wstring FileURLToFilePath(const GURL& url) {
  std::wstring file_path_str;

  do
  {
    std::string path;
	if (!url.is_valid())
      break;
    std::string host = url.host();
	if (host.empty()) {
      // URL contains no host, the path is the filename. In this case, the path
      // will probably be preceeded with a slash, as in "/C:/foo.txt", so we
      // trim out that here.
      path = url.path();
      size_t first_non_slash = path.find_first_not_of("/\\");
      if (first_non_slash != std::string::npos && first_non_slash > 0)
        path.erase(0, first_non_slash);
    }
    else {
      // URL contains a host: this means it's UNC. We keep the preceeding slash
      // on the path.
      path = "\\\\";
      path.append(host);
      path.append(url.path());
    }
    std::replace(path.begin(), path.end(), '/', '\\');

    if (path.empty())
      break;

    // GURL stores strings as percent-encoded 8-bit, this will undo if possible.
    path = UnescapeURLComponent(path, UnescapeRule::SPACES | UnescapeRule::URL_SPECIAL_CHARS);
    file_path_str = AToW(path);
  } while (false);

  return file_path_str;
}

} // namespace url_fixer

namespace {

// Returns true if it's a whitespace character.
inline bool IsWhitespace(wchar_t c) {
  return wcschr(url_fixer::kWhitespaceWide, c) != NULL;
}

// Hardcode these constants to avoid dependences on //chrome and //content.
const char kAboutScheme[] = "about";
const char kChromeUIScheme[] = "chrome";
const char kChromeUIDefaultHost[] = "version";
const char kViewSourceScheme[] = "view-source";
const char kFtpScheme[] = "ftp";
const char kHttpScheme[] = "http";
const char kStandardSchemeSeparator[] = "://";

// TODO(estade): Remove these ugly, ugly functions. They are only used in
// SegmentURL. A url_parse::Parsed object keeps track of a bunch of indices into
// a url string, and these need to be updated when the URL is converted from
// UTF8 to UTF16. Instead of this after-the-fact adjustment, we should parse it
// in the correct string format to begin with.
url_parse::Component UTF8ComponentToUTF16Component(
    const std::string& text_utf8,
    const url_parse::Component& component_utf8) {
  if (component_utf8.len == -1)
    return url_parse::Component();

  std::string before_component_string =
      text_utf8.substr(0, component_utf8.begin);
  std::string component_string =
      text_utf8.substr(component_utf8.begin, component_utf8.len);
  base::string16 before_component_string_16 =
      AToW(before_component_string);
  base::string16 component_string_16 = AToW(component_string);
  url_parse::Component component_16(before_component_string_16.length(),
                              component_string_16.length());
  return component_16;
}

void UTF8PartsToUTF16Parts(const std::string& text_utf8,
                           const url_parse::Parsed& parts_utf8,
                           url_parse::Parsed* parts) {
  bool is_string_ascii = true;
  for (size_t i = 0; i < text_utf8.length(); i++) {
    if (((unsigned char)text_utf8[i]) > 0x7F) {
      is_string_ascii = false;
      break;
    }
  }
  if (is_string_ascii) {
    *parts = parts_utf8;
    return;
  }

  parts->scheme = UTF8ComponentToUTF16Component(text_utf8, parts_utf8.scheme);
  parts->username =
      UTF8ComponentToUTF16Component(text_utf8, parts_utf8.username);
  parts->password =
      UTF8ComponentToUTF16Component(text_utf8, parts_utf8.password);
  parts->host = UTF8ComponentToUTF16Component(text_utf8, parts_utf8.host);
  parts->port = UTF8ComponentToUTF16Component(text_utf8, parts_utf8.port);
  parts->path = UTF8ComponentToUTF16Component(text_utf8, parts_utf8.path);
  parts->query = UTF8ComponentToUTF16Component(text_utf8, parts_utf8.query);
  parts->ref = UTF8ComponentToUTF16Component(text_utf8, parts_utf8.ref);
}

template <typename Char>
inline bool IsAsciiDigit(Char c) {
  return c >= '0' && c <= '9';
}

template<class StringType>
void DoReplaceSubstringsAfterOffset(StringType* str,
                                    typename StringType::size_type start_offset,
                                    const StringType& find_this,
                                    const StringType& replace_with,
                                    bool replace_all) {
  if ((start_offset == StringType::npos) || (start_offset >= str->length()))
    return;

  DCHECK(!find_this.empty());
  for (typename StringType::size_type offs(str->find(find_this, start_offset));
      offs != StringType::npos; offs = str->find(find_this, offs)) {
    str->replace(offs, find_this.length(), replace_with);
    offs += replace_with.length();

    if (!replace_all)
      break;
  }
}

void ReplaceSubstringsAfterOffset(base::string16* str,
                                  base::string16::size_type start_offset,
                                  const base::string16& find_this,
                                  const base::string16& replace_with) {
  DoReplaceSubstringsAfterOffset(str, start_offset, find_this, replace_with,
                                 true);  // replace all instances
}
// what we prepend to get a file URL
static const wchar_t kFileURLPrefix[] = L"file:///";

GURL FilePathToFileURL(const std::wstring& path) {
  // Produce a URL like "file:///C:/foo" for a regular file, or
  // "file://///server/path" for UNC. The URL canonicalizer will fix up the
  // latter case to be the canonical UNC form: "file://server/path"
  std::wstring url_string(kFileURLPrefix);
  url_string.append(path);

  // Now do replacement of some characters. Since we assume the input is a
  // literal filename, anything the URL parser might consider special should
  // be escaped here.

  // must be the first substitution since others will introduce percents as the
  // escape character
  ReplaceSubstringsAfterOffset(&url_string, 0, L"%", L"%25");

  // semicolon is supposed to be some kind of separator according to RFC 2396
  ReplaceSubstringsAfterOffset(&url_string, 0, L";", L"%3B");

  ReplaceSubstringsAfterOffset(&url_string, 0, L"#", L"%23");

  ReplaceSubstringsAfterOffset(&url_string, 0, L"?", L"%3F");

  return GURL(url_string);
}

// does some basic fixes for input that we want to test for file-ness
void PrepareStringForFileOps(std::wstring& text,
                             std::wstring* output) {
  url_fixer::TrimWhitespace(text, url_fixer::TRIM_ALL, output);
  replace(output->begin(), output->end(), '/', '\\');
}

// Tries to create a file: URL from |text| if it looks like a filename, even if
// it doesn't resolve as a valid path or to an existing file.  Returns a
// (possibly invalid) file: URL in |fixed_up_url| for input beginning
// with a drive specifier or "\\".  Returns the unchanged input in other cases
// (including file: URLs: these don't look like filenames).
std::string FixupPath(const std::string& text) {
  DCHECK(!text.empty());

  std::wstring filename;
  PrepareStringForFileOps(AToW(text), &filename);

  // Fixup Windows-style drive letters, where "C:" gets rewritten to "C|".
  if (filename.length() > 1 && filename[1] == '|')
    filename[1] = ':';

  // Here, we know the input looks like a file.
  GURL file_url = FilePathToFileURL(filename);
  if (file_url.is_valid()) {
    return file_url.spec();
  }

  // Invalid file URL, just return the input.
  return text;
}

// Checks |domain| to see if a valid TLD is already present.  If not, appends
// |desired_tld| to the domain, and prepends "www." unless it's already present.
void AddDesiredTLD(const std::string& desired_tld, std::string* domain) {
  if (desired_tld.empty() || domain->empty())
    return;

  // Check the TLD.  If the return value is positive, we already have a TLD, so
  // abort.  If the return value is std::string::npos, there's no valid host,
  // but we can try to append a TLD anyway, since the host may become valid once
  // the TLD is attached -- for example, "999999999999" is detected as a broken
  // IP address and marked invalid, but attaching ".com" makes it legal.  When
  // the return value is 0, there's a valid host with no known TLD, so we can
  // definitely append the user's TLD.  We disallow unknown registries here so
  // users can input "mail.yahoo" and hit ctrl-enter to get
  // "www.mail.yahoo.com".
  const size_t registry_length =
      net::registry_controlled_domains::GetRegistryLength(
          *domain,
          net::registry_controlled_domains::EXCLUDE_UNKNOWN_REGISTRIES,
          net::registry_controlled_domains::EXCLUDE_PRIVATE_REGISTRIES);
  if ((registry_length != 0) && (registry_length != std::string::npos))
    return;

  // Add the suffix at the end of the domain.
  const size_t domain_length(domain->length());
  DCHECK(domain_length > 0U);
  DCHECK(desired_tld[0] <= '.');
  if ((*domain)[domain_length - 1] != '.')
    domain->push_back('.');
  domain->append(desired_tld);

  // Now, if the domain begins with "www.", stop.
  const std::string prefix("www.");
  if (domain->compare(0, prefix.length(), prefix) != 0) {
    // Otherwise, add www. to the beginning of the URL.
    domain->insert(0, prefix);
  }
}

inline void FixupUsername(const std::string& text,
                          const url_parse::Component& part,
                          std::string* url) {
  if (!part.is_valid())
    return;

  // We don't fix up the username at the moment.
  url->append(text, part.begin, part.len);
  // Do not append the trailing '@' because we might need to include the user's
  // password.  FixupURL itself will append the '@' for us.
}

inline void FixupPassword(const std::string& text,
                          const url_parse::Component& part,
                          std::string* url) {
  if (!part.is_valid())
    return;

  // We don't fix up the password at the moment.
  url->append(":");
  url->append(text, part.begin, part.len);
}

void FixupHost(const std::string& text,
               const url_parse::Component& part,
               bool has_scheme,
               const std::string& desired_tld,
               std::string* url) {
  if (!part.is_valid())
    return;

  // Make domain valid.
  // Strip all leading dots and all but one trailing dot, unless the user only
  // typed dots, in which case their input is totally invalid and we should just
  // leave it unchanged.
  std::string domain(text, part.begin, part.len);
  const size_t first_nondot(domain.find_first_not_of('.'));
  if (first_nondot != std::string::npos) {
    domain.erase(0, first_nondot);
    size_t last_nondot(domain.find_last_not_of('.'));
    DCHECK(last_nondot != std::string::npos);
    last_nondot += 2;  // Point at second period in ending string
    if (last_nondot < domain.length())
      domain.erase(last_nondot);
  }

  // Add any user-specified TLD, if applicable.
  AddDesiredTLD(desired_tld, &domain);

  url->append(domain);
}

void FixupPort(const std::string& text,
               const url_parse::Component& part,
               std::string* url) {
  if (!part.is_valid())
    return;

  // We don't fix up the port at the moment.
  url->append(":");
  url->append(text, part.begin, part.len);
}

inline void FixupPath(const std::string& text,
                      const url_parse::Component& part,
                      std::string* url) {
  if (!part.is_valid() || part.len == 0) {
    // We should always have a path.
    url->append("/");
    return;
  }

  // Append the path as is.
  url->append(text, part.begin, part.len);
}

inline void FixupQuery(const std::string& text,
                       const url_parse::Component& part,
                       std::string* url) {
  if (!part.is_valid())
    return;

  // We don't fix up the query at the moment.
  url->append("?");
  url->append(text, part.begin, part.len);
}

inline void FixupRef(const std::string& text,
                     const url_parse::Component& part,
                     std::string* url) {
  if (!part.is_valid())
    return;

  // We don't fix up the ref at the moment.
  url->append("#");
  url->append(text, part.begin, part.len);
}

bool HasPort(const std::string& original_text,
             const url_parse::Component& scheme_component) {
  // Find the range between the ":" and the "/".
  size_t port_start = scheme_component.end() + 1;
  size_t port_end = port_start;
  while ((port_end < original_text.length()) &&
         !url_parse::IsAuthorityTerminator(original_text[port_end]))
    ++port_end;
  if (port_end == port_start)
    return false;

  // Scan the range to see if it is entirely digits.
  for (size_t i = port_start; i < port_end; ++i) {
    if (!IsAsciiDigit(original_text[i]))
      return false;
  }

  return true;
}

// Try to extract a valid scheme from the beginning of |text|.
// If successful, set |scheme_component| to the text range where the scheme
// was located, and fill |canon_scheme| with its canonicalized form.
// Otherwise, return false and leave the outputs in an indeterminate state.
bool GetValidScheme(const std::string& text,
                    url_parse::Component* scheme_component,
                    std::string* canon_scheme) {
  canon_scheme->clear();

  // Locate everything up to (but not including) the first ':'
  if (!url_parse::ExtractScheme(
          text.data(), static_cast<int>(text.length()), scheme_component)) {
    return false;
  }

  // Make sure the scheme contains only valid characters, and convert
  // to lowercase.  This also catches IPv6 literals like [::1], because
  // brackets are not in the whitelist.
  url_canon::StdStringCanonOutput canon_scheme_output(canon_scheme);
  url_parse::Component canon_scheme_component;
  if (!url_canon::CanonicalizeScheme(text.data(),
                               *scheme_component,
                               &canon_scheme_output,
                               &canon_scheme_component)) {
    return false;
  }

  // Strip the ':', and any trailing buffer space.
  DCHECK(0 == canon_scheme_component.begin);
  canon_scheme->erase(canon_scheme_component.len);

  // We need to fix up the segmentation for "www.example.com:/".  For this
  // case, we guess that schemes with a "." are not actually schemes.
  if (canon_scheme->find('.') != std::string::npos)
    return false;

  // We need to fix up the segmentation for "www:123/".  For this case, we
  // will add an HTTP scheme later and make the URL parser happy.
  // TODO(pkasting): Maybe we should try to use GURL's parser for this?
  if (HasPort(text, *scheme_component))
    return false;

  // Everything checks out.
  return true;
}

// Performs the work for url_fixer::SegmentURL. |text| may be modified on
// output on success: a semicolon following a valid scheme is replaced with a
// colon.
std::string SegmentURLInternal(std::string* text, url_parse::Parsed* parts) {
  // Initialize the result.
  *parts = url_parse::Parsed();

  std::string trimmed;
  url_fixer::TrimWhitespaceUTF8(*text, url_fixer::TRIM_ALL, &trimmed);
  if (trimmed.empty())
    return std::string();  // Nothing to segment.

  int trimmed_length = static_cast<int>(trimmed.length());
  if (url_parse::DoesBeginWindowsDriveSpec(trimmed.data(), 0, trimmed_length) ||
      url_parse::DoesBeginUNCPath(trimmed.data(), 0, trimmed_length, true))
    return "file";

  // Otherwise, we need to look at things carefully.
  std::string scheme;
  if (!GetValidScheme(*text, &parts->scheme, &scheme)) {
    // Try again if there is a ';' in the text. If changing it to a ':' results
    // in a scheme being found, continue processing with the modified text.
    bool found_scheme = false;
    size_t semicolon = text->find(';');
    if (semicolon != 0 && semicolon != std::string::npos) {
      (*text)[semicolon] = ':';
      if (GetValidScheme(*text, &parts->scheme, &scheme))
        found_scheme = true;
      else
        (*text)[semicolon] = ';';
    }
    if (!found_scheme) {
      // Couldn't determine the scheme, so just pick one.
      parts->scheme.reset();
      scheme = _strnicmp(text->c_str(), "ftp.", 4) == 0 ? kFtpScheme
                                                        : kHttpScheme;
    }
  }

  // Proceed with about and chrome schemes, but not file or nonstandard schemes.
  if ((scheme != kAboutScheme) && (scheme != kChromeUIScheme) &&
      ((scheme == url_util::kFileScheme) ||
       !url_util::IsStandard(
           scheme.c_str(),
           url_parse::Component(0, static_cast<int>(scheme.length()))))) {
    return scheme;
  }

  if (scheme == url_util::kFileSystemScheme) {
    // Have the GURL parser do the heavy lifting for us.
    url_parse::ParseFileSystemURL(
        text->data(), static_cast<int>(text->length()), parts);
    return scheme;
  }

  if (parts->scheme.is_valid()) {
    // Have the GURL parser do the heavy lifting for us.
    url_parse::ParseStandardURL(
        text->data(), static_cast<int>(text->length()), parts);
    return scheme;
  }

  // We need to add a scheme in order for ParseStandardURL to be happy.
  // Find the first non-whitespace character.
  std::string::iterator first_nonwhite = text->begin();
  while ((first_nonwhite != text->end()) && IsWhitespace(*first_nonwhite))
    ++first_nonwhite;

  // Construct the text to parse by inserting the scheme.
  std::string inserted_text(scheme);
  inserted_text.append(kStandardSchemeSeparator);
  std::string text_to_parse(text->begin(), first_nonwhite);
  text_to_parse.append(inserted_text);
  text_to_parse.append(first_nonwhite, text->end());

  // Have the GURL parser do the heavy lifting for us.
  url_parse::ParseStandardURL(
      text_to_parse.data(), static_cast<int>(text_to_parse.length()), parts);

  // Offset the results of the parse to match the original text.
  const int offset = -static_cast<int>(inserted_text.length());
  url_fixer::OffsetComponent(offset, &parts->scheme);
  url_fixer::OffsetComponent(offset, &parts->username);
  url_fixer::OffsetComponent(offset, &parts->password);
  url_fixer::OffsetComponent(offset, &parts->host);
  url_fixer::OffsetComponent(offset, &parts->port);
  url_fixer::OffsetComponent(offset, &parts->path);
  url_fixer::OffsetComponent(offset, &parts->query);
  url_fixer::OffsetComponent(offset, &parts->ref);

  return scheme;
}

// Adjusts all the components of |parsed| by |delta|, except for the scheme.
void AdjustAllComponentsButScheme(int delta, url_parse::Parsed* parsed) {
  url_fixer::OffsetComponent(delta, &(parsed->username));
  url_fixer::OffsetComponent(delta, &(parsed->password));
  url_fixer::OffsetComponent(delta, &(parsed->host));
  url_fixer::OffsetComponent(delta, &(parsed->port));
  url_fixer::OffsetComponent(delta, &(parsed->path));
  url_fixer::OffsetComponent(delta, &(parsed->query));
  url_fixer::OffsetComponent(delta, &(parsed->ref));
}

// Helper for FormatUrlWithOffsets().
base::string16 FormatViewSourceUrl(
    const GURL& url,
    const std::string& languages,
    url_parse::Parsed* new_parsed,
    size_t* prefix_end) {
  DCHECK(new_parsed);
  const char kViewSource[] = "view-source:";
  const size_t kViewSourceLength = ARRAYSIZE(kViewSource) - 1;

  // Format the underlying URL and record adjustments.
  const std::string& url_str(url.possibly_invalid_spec());
  base::string16 result(AToW(kViewSource) +
      url_fixer::FormatUrl(GURL(url_str.substr(kViewSourceLength)),
                                languages, new_parsed, prefix_end));

  // Adjust positions of the parsed components.
  if (new_parsed->scheme.is_nonempty()) {
    // Assume "view-source:real-scheme" as a scheme.
    new_parsed->scheme.len += kViewSourceLength;
  } else {
    new_parsed->scheme.begin = 0;
    new_parsed->scheme.len = kViewSourceLength - 1;
  }
  AdjustAllComponentsButScheme(kViewSourceLength, new_parsed);

  if (prefix_end)
    *prefix_end += kViewSourceLength;

  return result;
}

class AppendComponentTransform {
 public:
  AppendComponentTransform() {}
  virtual ~AppendComponentTransform() {}

  virtual base::string16 Execute(
      const std::string& component_text) const = 0;

  // NOTE: No DISALLOW_COPY_AND_ASSIGN here, since gcc < 4.3.0 requires an
  // accessible copy constructor in order to call AppendFormattedComponent()
  // with an inline temporary (see http://gcc.gnu.org/bugs/#cxx%5Frvalbind ).
};

class HostComponentTransform : public AppendComponentTransform {
 public:
  explicit HostComponentTransform(const std::string& languages)
      : languages_(languages) {
  }

 private:
  virtual base::string16 Execute(
      const std::string& component_text) const OVERRIDE {
    typedef int (WINAPI *IdnToUnicode)(DWORD dwFlags,
                                       LPCWSTR lpASCIICharStr,
                                       int cchASCIIChar,
                                       LPWSTR lpUnicodeCharStr,
                                       int cchUnicodeChar);
    static IdnToUnicode s_idnToUnicode = NULL;
    std::wstring component = AToW(component_text);
    if (s_idnToUnicode == NULL) {
      HMODULE hModule = GetModuleHandle(L"kernel32");
      if (!hModule)
        hModule = ::LoadLibraryW(L"kernel32");
      if (hModule)
        s_idnToUnicode = (IdnToUnicode)::GetProcAddress(hModule, "IdnToUnicode");
      if (!s_idnToUnicode) {
        hModule = LoadLibrary(L"Normaliz");
        if (hModule)
          s_idnToUnicode = (IdnToUnicode)::GetProcAddress(hModule, "IdnToUnicode");
      }
      if (!s_idnToUnicode) {
        s_idnToUnicode = (IdnToUnicode)-1;
        return component;
      }
    }
    else if (s_idnToUnicode == (IdnToUnicode)-1) {
      return component;
    }
	
    //
    // http://crash.sogou-inc.com/ 49595 6.0.5.17414 20150716
    // 初始化 s_idnToUnicode 存在多线程问题，用简单的方法进行修复，yangqi，20150717
    //
    IdnToUnicode idnToUnicode = s_idnToUnicode;
    if (idnToUnicode != NULL && idnToUnicode != (IdnToUnicode)-1) {
      int output_length = idnToUnicode(0, component.c_str(), component.length(), NULL, 0);
	  wchar_t* output_string = new wchar_t[output_length];
	  output_length = idnToUnicode(0, component.c_str(), component.length(), output_string, output_length);
	  component = std::wstring(output_string, output_length);
	  delete[] output_string;
    }
    return component;
  }

  const std::string& languages_;
};

class NonHostComponentTransform : public AppendComponentTransform {
 public:
 explicit NonHostComponentTransform(url_fixer::UnescapeRule::Type eType = url_fixer::UnescapeRule::NONE)
	  : unescape_rules_(eType) { }

 private:
  virtual base::string16 Execute(
      const std::string& component_text) const OVERRIDE {
	  std::string strOut = component_text;
	  if (unescape_rules_ != url_fixer::UnescapeRule::NONE)
		  strOut = url_fixer::UnescapeURLWithAdjustmentsImpl(component_text, unescape_rules_);
	  return AToW(Universal::Misc::DetectEncoding(strOut.c_str()) == CP_Utf8 ? strOut : component_text);
  }

  const url_fixer::UnescapeRule::Type unescape_rules_;
};

// Transforms the portion of |spec| covered by |original_component| according to
// |transform|.  Appends the result to |output|.  If |output_component| is
// non-NULL, its start and length are set to the transformed component's new
// start and length.  If |adjustments| is non-NULL, appends adjustments (if
// any) that reflect the transformation the original component underwent to
// become the transformed value appended to |output|.
void AppendFormattedComponent(const std::string& spec,
                              const url_parse::Component& original_component,
                              const AppendComponentTransform& transform,
                              base::string16* output,
                              url_parse::Component* output_component) {
  DCHECK(output);
  if (original_component.is_nonempty()) {
    size_t original_component_begin =
        static_cast<size_t>(original_component.begin);
    size_t output_component_begin = output->length();
    std::string component_str(spec, original_component_begin,
                              static_cast<size_t>(original_component.len));

    // Transform |component_str| and modify |adjustments| appropriately.
    output->append(transform.Execute(component_str));

    // Set positions of the parsed component.
    if (output_component) {
      output_component->begin = static_cast<int>(output_component_begin);
      output_component->len =
          static_cast<int>(output->length() - output_component_begin);
    }
  } else if (output_component) {
    output_component->reset();
  }
}

}  // namespace

std::string url_fixer::SegmentURL(const std::string& text, url_parse::Parsed* parts) {
  std::string mutable_text(text);
  return SegmentURLInternal(&mutable_text, parts);
}

base::string16 url_fixer::SegmentURL(const base::string16& text,
                                     url_parse::Parsed* parts) {
  std::string text_utf8 = WToA(text);
  url_parse::Parsed parts_utf8;
  std::string scheme_utf8 = SegmentURL(text_utf8, &parts_utf8);
  UTF8PartsToUTF16Parts(text_utf8, parts_utf8, parts);
  return AToW(scheme_utf8);
}

GURL url_fixer::FixupURL(const std::string& text,
                         const std::string& desired_tld) {
  std::string trimmed;
  TrimWhitespaceUTF8(text, TRIM_ALL, &trimmed);
  if (trimmed.empty())
    return GURL();  // Nothing here.

  // Segment the URL.
  url_parse::Parsed parts;
  std::string scheme(SegmentURLInternal(&trimmed, &parts));

  // For view-source: URLs, we strip "view-source:", do fixup, and stick it back
  // on.  This allows us to handle things like "view-source:google.com".
  if (scheme == kViewSourceScheme) {
    // Reject "view-source:view-source:..." to avoid deep recursion.
    std::string view_source(kViewSourceScheme + std::string(":"));
    if (_strnicmp(text.c_str(), (view_source + view_source).c_str(), 24)) {
      return GURL(kViewSourceScheme + std::string(":") +
                  FixupURL(trimmed.substr(scheme.length() + 1), desired_tld)
                      .possibly_invalid_spec());
    }
  }

  // We handle the file scheme separately.
  if (scheme == url_util::kFileScheme)
    return GURL(parts.scheme.is_valid() ? text : FixupPath(text));

  // We handle the filesystem scheme separately.
  if (scheme == url_util::kFileSystemScheme) {
    if (parts.inner_parsed() && parts.inner_parsed()->scheme.is_valid())
      return GURL(text);
    return GURL();
  }

  // Parse and rebuild about: and chrome: URLs, except about:blank.
  bool chrome_url =
      _stricmp(trimmed.c_str(), "about:blank") &&
      ((scheme == kAboutScheme) || (scheme == kChromeUIScheme));

  // For some schemes whose layouts we understand, we rebuild it.
  if (chrome_url ||
      url_util::IsStandard(scheme.c_str(),
                      url_parse::Component(0, static_cast<int>(scheme.length())))) {
    // Replace the about: scheme with the chrome: scheme.
    std::string url(chrome_url ? kChromeUIScheme : scheme);
    url.append(kStandardSchemeSeparator);

    // We need to check whether the |username| is valid because it is our
    // responsibility to append the '@' to delineate the user information from
    // the host portion of the URL.
    if (parts.username.is_valid()) {
      FixupUsername(trimmed, parts.username, &url);
      FixupPassword(trimmed, parts.password, &url);
      url.append("@");
    }

    FixupHost(trimmed, parts.host, parts.scheme.is_valid(), desired_tld, &url);
    if (chrome_url && !parts.host.is_valid())
      url.append(kChromeUIDefaultHost);
    FixupPort(trimmed, parts.port, &url);
    FixupPath(trimmed, parts.path, &url);
    FixupQuery(trimmed, parts.query, &url);
    FixupRef(trimmed, parts.ref, &url);

    return GURL(url);
  }

  // In the worst-case, we insert a scheme if the URL lacks one.
  if (!parts.scheme.is_valid()) {
    std::string fixed_scheme(scheme);
    fixed_scheme.append(kStandardSchemeSeparator);
    trimmed.insert(0, fixed_scheme);
  }

  return GURL(trimmed);
}

void url_fixer::OffsetComponent(int offset, url_parse::Component* part) {
  DCHECK(part);

  if (part->is_valid()) {
    // Offset the location of this component.
    part->begin += offset;

    // This part might not have existed in the original text.
    if (part->begin < 0)
      part->reset();
  }
}

bool url_fixer::IsEquivalentScheme(const std::string& scheme1,
                                   const std::string& scheme2) {
  return scheme1 == scheme2 ||
      (scheme1 == kAboutScheme && scheme2 == kChromeUIScheme) ||
      (scheme1 == kChromeUIScheme && scheme2 == kAboutScheme);
}

base::string16 url_fixer::FormatUrl(const GURL& url,
                                    const std::string& languages,
                                    url_parse::Parsed* new_parsed,
                                    size_t* prefix_end,
									UnescapeRule::Type eType) {
  url_parse ::Parsed parsed_temp;
  if (!new_parsed)
    new_parsed = &parsed_temp;
  else
    *new_parsed = url_parse::Parsed();
  // Special handling for view-source:.  Don't use content::kViewSourceScheme
  // because this library shouldn't depend on chrome.
  const char* const kViewSource = "view-source";
  // Reject "view-source:view-source:..." to avoid deep recursion.
  const char* const kViewSourceTwice = "view-source:view-source:";
  if (url.SchemeIs(kViewSource) &&
      _strnicmp(url.possibly_invalid_spec().c_str(), kViewSourceTwice, 24) != 0) {
    return FormatViewSourceUrl(url, languages,
                               new_parsed, prefix_end);
  }

  // We handle both valid and invalid URLs (this will give us the spec
  // regardless of validity).
  const std::string& spec = url.possibly_invalid_spec();
  const url_parse::Parsed& parsed = url.parsed_for_possibly_invalid_spec();

  // Scheme & separators.  These are ASCII.
  base::string16 url_string;
  url_string.insert(
      url_string.end(), spec.begin(),
      spec.begin() + parsed.CountCharactersBefore(url_parse::Parsed::USERNAME, true));

  // Username & password.
  AppendFormattedComponent(spec, parsed.username,
                           NonHostComponentTransform(eType),
                           &url_string, &new_parsed->username);
  if (parsed.password.is_valid())
    url_string.push_back(':');
  AppendFormattedComponent(spec, parsed.password,
                           NonHostComponentTransform(eType),
                           &url_string, &new_parsed->password);
  if (parsed.username.is_valid() || parsed.password.is_valid())
    url_string.push_back('@');

  if (prefix_end)
    *prefix_end = static_cast<size_t>(url_string.length());

  // Host.
  AppendFormattedComponent(spec, parsed.host, HostComponentTransform(languages),
                           &url_string, &new_parsed->host);

  // Port.
  if (parsed.port.is_nonempty()) {
    url_string.push_back(':');
    new_parsed->port.begin = url_string.length();
    url_string.insert(url_string.end(),
                      spec.begin() + parsed.port.begin,
                      spec.begin() + parsed.port.end());
    new_parsed->port.len = url_string.length() - new_parsed->port.begin;
  } else {
    new_parsed->port.reset();
  }

  // Path & query.  Both get the same general unescape & convert treatment.
  AppendFormattedComponent(spec, parsed.path,
                           NonHostComponentTransform(eType),
                           &url_string, &new_parsed->path);

  if (parsed.query.is_valid())
    url_string.push_back('?');
  AppendFormattedComponent(spec, parsed.query,
                           NonHostComponentTransform(eType),
                           &url_string, &new_parsed->query);

  // Ref.  This is valid, unescaped UTF-8, so we can just convert.
  if (parsed.ref.is_valid())
    url_string.push_back('#');
  AppendFormattedComponent(spec, parsed.ref,
                           NonHostComponentTransform(),
                           &url_string, &new_parsed->ref);

  return url_string;
}

base::string16 url_fixer::TransformHost(const std::string& host, const std::string& languages) {
  base::string16 format_host;
  AppendComponentTransform* transform = new HostComponentTransform(languages);
  format_host.append(transform->Execute(host));
  delete transform;
  return format_host;
}

std::string url_fixer::UnescapeURLComponentByDefault(const std::string& url)
{
	return UnescapeURLComponent(url.c_str(), url_fixer::UnescapeRule::NORMAL);
}
