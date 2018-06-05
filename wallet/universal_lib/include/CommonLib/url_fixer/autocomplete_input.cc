// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "autocomplete_input.h"

#include "url_fixer.h"
#include "../tld/registry_controlled_domain.h"
#include "../url/url_canon_ip.h"
#include "../url/url_util.h"
#include "../String/Convert.h"

namespace {

// Hardcode constant to avoid any dependencies on content/.
const char kViewSourceScheme[] = "view-source";
const char kFileScheme[] = "file";
const char kFileSystemScheme[] = "filesystem";
const char kHttpScheme[] = "http";
const char kHttpsScheme[] = "https";
const char kFtpScheme[] = "ftp";
const char kAboutScheme[] = "about";
const char kMailToScheme[] = "mailto";
const char kStandardSchemeSeparator[] = "://";

AutocompleteInput::Type GetInputTypeForScheme(
    const std::string& scheme) {
  // This doesn't check the preference but check some chrome-ish schemes.
  const char* kKnownURLSchemes[] = {
    kFileScheme, kAboutScheme, kFtpScheme, "blob", kHttpScheme, kHttpsScheme, "ws", "wss",
    kFileSystemScheme, "view-source", "javascript", "chrome", "chrome-ui", kMailToScheme,
  };
  for (size_t i = 0; i < ARRAYSIZE(kKnownURLSchemes); ++i) {
    if (scheme == kKnownURLSchemes[i])
      return AutocompleteInput::URL;
  }

  return AutocompleteInput::INVALID;
}


void AdjustCursorPositionIfNecessary(size_t num_leading_chars_removed,
                                     size_t* cursor_position) {
  if (*cursor_position == base::string16::npos)
    return;
  if (num_leading_chars_removed < *cursor_position)
    *cursor_position -= num_leading_chars_removed;
  else
    *cursor_position = 0;
}

inline bool IsHostCharAlphanumeric(char c) {
  // We can just check lowercase because uppercase characters have already been
  // normalized.
  return ((c >= 'a') && (c <= 'z')) || ((c >= '0') && (c <= '9'));
}

bool IsCanonicalizedHostCompliant(const std::string& host,
                                  const std::string& desired_tld) {
  if (host.empty())
    return false;

  bool in_component = false;
  bool most_recent_component_started_alphanumeric = false;
  bool last_char_was_underscore = false;

  for (std::string::const_iterator i(host.begin()); i != host.end(); ++i) {
    const char c = *i;
    if (!in_component) {
      most_recent_component_started_alphanumeric = IsHostCharAlphanumeric(c);
      if (!most_recent_component_started_alphanumeric && (c != '-'))
        return false;
      in_component = true;
    } else {
      if (c == '.') {
        if (last_char_was_underscore)
          return false;
        in_component = false;
      } else if (IsHostCharAlphanumeric(c) || (c == '-')) {
        last_char_was_underscore = false;
      } else if (c == '_') {
        last_char_was_underscore = true;
      } else {
        return false;
      }
    }
  }

  return most_recent_component_started_alphanumeric ||
      (!desired_tld.empty() && IsHostCharAlphanumeric(desired_tld[0]));
}

std::string CanonicalizeHost(const std::string& host,
                             url_canon::CanonHostInfo* host_info) {
  // Try to canonicalize the host.
  const url_parse::Component raw_host_component(0, static_cast<int>(host.length()));
  std::string canon_host;
  url_canon::StdStringCanonOutput canon_host_output(&canon_host);
  url_canon::CanonicalizeHostVerbose(host.c_str(), raw_host_component,
                               &canon_host_output, host_info);

  if (host_info->out_host.is_nonempty() &&
      host_info->family != url_canon::CanonHostInfo::BROKEN) {
    // Success!  Assert that there's no extra garbage.
    canon_host_output.Complete();
    DCHECK(host_info->out_host.len == static_cast<int>(canon_host.length()));
  } else {
    // Empty host, or canonicalization failed.  We'll return empty.
    canon_host.clear();
  }

  return canon_host;
}

}  // namespace

AutocompleteInput::AutocompleteInput()
    : cursor_position_(base::string16::npos),
      type_(INVALID) {
}

AutocompleteInput::AutocompleteInput(
    const base::string16& text,
    size_t cursor_position,
    const base::string16& desired_tld)
    : cursor_position_(cursor_position) {
  DCHECK(cursor_position <= text.length() ||
         cursor_position == base::string16::npos);
  // None of the providers care about leading white space so we always trim it.
  // Providers that care about trailing white space handle trimming themselves.
  if ((url_fixer::TrimWhitespace(text, url_fixer::TRIM_LEADING, &text_) &
       url_fixer::TRIM_LEADING) != 0)
    AdjustCursorPositionIfNecessary(text.length() - text_.length(),
                                    &cursor_position_);

  GURL canonicalized_url;
  type_ = Parse(text_, desired_tld, &parts_, &scheme_,
                &canonicalized_url);

  if (type_ == INVALID)
    return;

  if (((type_ == UNKNOWN) ||
       (type_ == URL)) &&
      canonicalized_url.is_valid() &&
      (!canonicalized_url.IsStandard() || canonicalized_url.SchemeIsFile() ||
       canonicalized_url.SchemeIsFileSystem() ||
       !canonicalized_url.host().empty()))
    canonicalized_url_ = canonicalized_url;

  size_t chars_removed = RemoveForcedQueryStringIfNecessary(type_, &text_);
  AdjustCursorPositionIfNecessary(chars_removed, &cursor_position_);
  if (chars_removed) {
    // Remove spaces between opening question mark and first actual character.
    base::string16 trimmed_text;
    if ((url_fixer::TrimWhitespace(text_, url_fixer::TRIM_LEADING, &trimmed_text) &
         url_fixer::TRIM_LEADING) != 0) {
      AdjustCursorPositionIfNecessary(text_.length() - trimmed_text.length(),
                                      &cursor_position_);
      text_ = trimmed_text;
    }
  }
}

AutocompleteInput::~AutocompleteInput() {
}

// static
size_t AutocompleteInput::RemoveForcedQueryStringIfNecessary(
    Type type,
    base::string16* text) {
  if ((type != FORCED_QUERY) || text->empty() ||
      (*text)[0] != L'?')
    return 0;
  // Drop the leading '?'.
  text->erase(0, 1);
  return 1;
}

// static
std::string AutocompleteInput::TypeToString(
    Type type) {
  switch (type) {
    case INVALID:      return "invalid";
    case UNKNOWN:      return "unknown";
    case DEPRECATED_REQUESTED_URL:
      return "deprecated-requested-url";
    case URL:          return "url";
    case QUERY:        return "query";
    case FORCED_QUERY: return "forced-query";
  }
  return std::string();
}

// static
AutocompleteInput::Type AutocompleteInput::Parse(
    const base::string16& text,
    const base::string16& desired_tld,
    url_parse::Parsed* parts,
    base::string16* scheme,
    GURL* canonicalized_url) {
  size_t first_non_white = text.find_first_not_of(url_fixer::kWhitespaceUTF16, 0);
  if (first_non_white == base::string16::npos)
    return INVALID;  // All whitespace.

  if (text[first_non_white] == L'?') {
    // If the first non-whitespace character is a '?', we magically treat this
    // as a query.
    return FORCED_QUERY;
  }

  // Ask our parsing back-end to help us understand what the user typed.  We
  // use the URLFixerUpper here because we want to be smart about what we
  // consider a scheme.  For example, we shouldn't consider www.google.com:80
  // to have a scheme.
  url_parse::Parsed local_parts;
  if (!parts)
    parts = &local_parts;
  const base::string16 parsed_scheme(url_fixer::SegmentURL(text, parts));
  if (scheme)
    *scheme = parsed_scheme;
  const std::string parsed_scheme_utf8(WToA(parsed_scheme));

  // If we can't canonicalize the user's input, the rest of the autocomplete
  // system isn't going to be able to produce a navigable URL match for it.
  // So we just return QUERY immediately in these cases.
  GURL placeholder_canonicalized_url;
  if (!canonicalized_url)
    canonicalized_url = &placeholder_canonicalized_url;
  *canonicalized_url = url_fixer::FixupURL(WToA(text),
                                           WToA(desired_tld));
  if (!canonicalized_url->is_valid())
    return QUERY;

  if (url_util::LowerCaseEqualsASCII(parsed_scheme_utf8, kFileScheme)) {
    // A user might or might not type a scheme when entering a file URL.  In
    // either case, |parsed_scheme_utf8| will tell us that this is a file URL,
    // but |parts->scheme| might be empty, e.g. if the user typed "C:\foo".
    return URL;
  }

  // If the user typed a scheme, and it's HTTP or HTTPS, we know how to parse it
  // well enough that we can fall through to the heuristics below.  If it's
  // something else, we can just determine our action based on what we do with
  // any input of this scheme.  In theory we could do better with some schemes
  // (e.g. "ftp" or "view-source") but I'll wait to spend the effort on that
  // until I run into some cases that really need it.
  if (parts->scheme.is_nonempty() &&
      !url_util::LowerCaseEqualsASCII(parsed_scheme_utf8, kHttpScheme) &&
      !url_util::LowerCaseEqualsASCII(parsed_scheme_utf8, kHttpsScheme)) {
    Type type =
        GetInputTypeForScheme(parsed_scheme_utf8);
    if (type != INVALID)
      return type;

    // We don't know about this scheme.  It might be that the user typed a
    // URL of the form "username:password@foo.com".
    const base::string16 http_scheme_prefix =
        AToW(std::string(kHttpScheme) +
                           kStandardSchemeSeparator);
    url_parse::Parsed http_parts;
    base::string16 http_scheme;
    GURL http_canonicalized_url;
    Type http_type =
        Parse(http_scheme_prefix + text, desired_tld,
              &http_parts, &http_scheme, &http_canonicalized_url);
    DCHECK(std::string(kHttpScheme) == WToA(http_scheme));

    if ((http_type == URL) &&
        http_parts.username.is_nonempty() &&
        http_parts.password.is_nonempty()) {
      // Manually re-jigger the parsed parts to match |text| (without the
      // http scheme added).
      http_parts.scheme.reset();
      url_parse::Component* components[] = {
        &http_parts.username,
        &http_parts.password,
        &http_parts.host,
        &http_parts.port,
        &http_parts.path,
        &http_parts.query,
        &http_parts.ref,
      };
      for (size_t i = 0; i < ARRAYSIZE(components); ++i) {
        url_fixer::OffsetComponent(
            -static_cast<int>(http_scheme_prefix.length()), components[i]);
      }

      *parts = http_parts;
      if (scheme)
        scheme->clear();
      *canonicalized_url = http_canonicalized_url;

      return URL;
    }

    // We don't know about this scheme and it doesn't look like the user
    // typed a username and password.  It's likely to be a search operator
    // like "site:" or "link:".  We classify it as UNKNOWN so the user has
    // the option of treating it as a URL if we're wrong.
    // Note that SegmentURL() is smart so we aren't tricked by "c:\foo" or
    // "www.example.com:81" in this case.
    return UNKNOWN;
  }

  // Either the user didn't type a scheme, in which case we need to distinguish
  // between an HTTP URL and a query, or the scheme is HTTP or HTTPS, in which
  // case we should reject invalid formulations.

  // If we have an empty host it can't be a valid HTTP[S] URL.  (This should
  // only trigger for input that begins with a colon, which GURL will parse as a
  // valid, non-standard URL; for standard URLs, an empty host would have
  // resulted in an invalid |canonicalized_url| above.)
  if (!parts->host.is_nonempty())
    return QUERY;

  // Sanity-check: GURL should have failed to canonicalize this URL if it had an
  // invalid port.
  DCHECK(url_parse::PORT_INVALID != url_parse::ParsePort(text.c_str(), parts->port));

  // Likewise, the RCDS can reject certain obviously-invalid hosts.  (We also
  // use the registry length later below.)
  const base::string16 host(text.substr(parts->host.begin, parts->host.len));
  const size_t registry_length =
      net::registry_controlled_domains::GetRegistryLength(
          WToA(host),
          net::registry_controlled_domains::EXCLUDE_UNKNOWN_REGISTRIES,
          net::registry_controlled_domains::EXCLUDE_PRIVATE_REGISTRIES);
  if (registry_length == std::string::npos) {
    // Try to append the desired_tld.
    if (!desired_tld.empty()) {
      base::string16 host_with_tld(host);
      if (host[host.length() - 1] != '.')
        host_with_tld += '.';
      host_with_tld += desired_tld;
      const size_t tld_length =
          net::registry_controlled_domains::GetRegistryLength(
              WToA(host_with_tld),
              net::registry_controlled_domains::EXCLUDE_UNKNOWN_REGISTRIES,
              net::registry_controlled_domains::EXCLUDE_PRIVATE_REGISTRIES);
      if (tld_length != std::string::npos) {
        // Something like "99999999999" that looks like a bad IP
        // address, but becomes valid on attaching a TLD.
        return URL;
      }
    }
    // Could be a broken IP address, etc.
    return QUERY;
  }


  // See if the hostname is valid.  While IE and GURL allow hostnames to contain
  // many other characters (perhaps for weird intranet machines), it's extremely
  // unlikely that a user would be trying to type those in for anything other
  // than a search query.
  url_canon::CanonHostInfo host_info;
  const std::string canonicalized_host(CanonicalizeHost(
      WToA(host), &host_info));
  if ((host_info.family == url_canon::CanonHostInfo::NEUTRAL) &&
      !IsCanonicalizedHostCompliant(canonicalized_host,
                                         WToA(desired_tld))) {
    // Invalid hostname.  There are several possible cases:
    // * Our checker is too strict and the user pasted in a real-world URL
    //   that's "invalid" but resolves.  To catch these, we return UNKNOWN when
    //   the user explicitly typed a scheme, so we'll still search by default
    //   but we'll show the accidental search infobar if necessary.
    // * The user is typing a multi-word query.  If we see a space anywhere in
    //   the hostname we assume this is a search and return QUERY.
    // * Our checker is too strict and the user is typing a real-world hostname
    //   that's "invalid" but resolves.  We return UNKNOWN if the TLD is known.
    //   Note that we explicitly excluded hosts with spaces above so that
    //   "toys at amazon.com" will be treated as a search.
    // * The user is typing some garbage string.  Return QUERY.
    //
    // Thus we fall down in the following cases:
    // * Trying to navigate to a hostname with spaces
    // * Trying to navigate to a hostname with invalid characters and an unknown
    //   TLD
    // These are rare, though probably possible in intranets.
    return (parts->scheme.is_nonempty() ||
           ((registry_length != 0) &&
            (host.find(' ') == base::string16::npos))) ?
        UNKNOWN : QUERY;
  }

  // Now that we've ruled out all schemes other than http or https and done a
  // little more sanity checking, the presence of a scheme means this is likely
  // a URL.
  if (parts->scheme.is_nonempty())
    return URL;

  // See if the host is an IP address.
  if (host_info.family == url_canon::CanonHostInfo::IPV6)
    return URL;
  //this part(from here to the next 64 lines) are copied from a new chromium version [mozeng]
  if (host_info.family == url_canon::CanonHostInfo::IPV4) {
    // The host may be a real IP address, or something that looks a bit like it
    // (e.g. "1.2" or "3232235521").  We check whether it was convertible to an
    // IP with a non-zero first octet; IPs with first octet zero are "source
    // IPs" and are almost never navigable as destination addresses.
    //
    // The one exception to this is 0.0.0.0; on many systems, attempting to
    // navigate to this IP actually navigates to localhost.  To support this
    // case, when the converted IP is 0.0.0.0, we go ahead and run the "did the
    // user actually type four components" test in the conditional below, so
    // that we'll allow explicit attempts to navigate to "0.0.0.0".  If the
    // input was anything else (e.g. "0"), we'll fall through to returning QUERY
    // afterwards.
    if ((host_info.address[0] != 0) ||
      ((host_info.address[1] == 0) && (host_info.address[2] == 0) &&
      (host_info.address[3] == 0))) {
      // This is theoretically a navigable IP.  We have four cases.  The first
      // three are:
      // * If the user typed four distinct components, this is an IP for sure.
      // * If the user typed two or three components, this is almost certainly a
      //   query, especially for two components (as in "13.5/7.25"), but we'll
      //   allow navigation for an explicit scheme or trailing slash below.
      // * If the user typed one component, this is likely a query, but could be
      //   a non-dotted-quad version of an IP address.
      // Unfortunately, since we called CanonicalizeHost() on the
      // already-canonicalized host, all of these cases will have been changed
      // to have four components (e.g. 13.2 -> 13.0.0.2), so we have to call
      // CanonicalizeHost() again, this time on the original input, so that we
      // can get the correct number of IP components.
      //
      // The fourth case is that the user typed something ambiguous like ".1.2"
      // that fixup converted to an IP address ("1.0.0.2").  In this case the
      // call to CanonicalizeHost() will return NEUTRAL here.  Since it's not
      // clear what the user intended, we fall back to our other heuristics.
      CanonicalizeHost(WToA(host), &host_info);
      if ((host_info.family == url_canon::CanonHostInfo::IPV4) &&
        (host_info.num_ipv4_components == 4))
        return URL;
    }

    // By this point, if we have an "IP" with first octet zero, we know it
    // wasn't "0.0.0.0", so mark it as non-navigable.
    if (host_info.address[0] == 0)
      return QUERY;
  }

  // Presence of a password means this is likely a URL.  Note that unless the
  // user has typed an explicit "http://" or similar, we'll probably think that
  // the username is some unknown scheme, and bail out in the scheme-handling
  // code above.
  if (parts->password.is_nonempty())
    return URL;

  // Trailing slashes force the input to be treated as a URL.
  if (parts->path.is_nonempty()) {
    char c = text[parts->path.end() - 1];
    if ((c == '\\') || (c == '/'))
      return URL;
  }

  // If there is more than one recognized non-host component, this is likely to
  // be a URL, even if the TLD is unknown (in which case this is likely an
  // intranet URL).
  if (NumNonHostComponents(*parts) > 1)
    return URL;

  // If the host has a known TLD or a port, it's probably a URL, with the
  // following exceptions:
  // * Any "IP addresses" that make it here are more likely searches
  //   (see above).
  // * If we reach here with a username, our input looks like "user@host[.tld]".
  //   Because there is no scheme explicitly specified, we think this is more
  //   likely an email address than an HTTP auth attempt.  Hence, we search by
  //   default and let users correct us on a case-by-case basis.
  // Note that we special-case "localhost" as a known hostname.
  if ((host_info.family != url_canon::CanonHostInfo::IPV4) &&
      ((registry_length != 0) || (host == AToW("localhost") ||
       parts->port.is_nonempty()))) {
    return parts->username.is_nonempty() ? UNKNOWN :
                                           URL;
  }

  // If we reach this point, we know there's no known TLD on the input, so if
  // the user wishes to add a desired_tld, the fixup code will oblige; thus this
  // is a URL.
  if (!desired_tld.empty())
    return URL;

  // No scheme, password, port, path, and no known TLD on the host.
  // This could be:
  // * An "incomplete IP address"; likely a search (see above).
  // * An email-like input like "user@host", where "host" has no known TLD.
  //   It's not clear what the user means here and searching seems reasonable.
  // * A single word "foo"; possibly an intranet site, but more likely a search.
  //   This is ideally an UNKNOWN, and we can let the Alternate Nav URL code
  //   catch our mistakes.
  // * A URL with a valid TLD we don't know about yet.  If e.g. a registrar adds
  //   "xxx" as a TLD, then until we add it to our data file, Chrome won't know
  //   "foo.xxx" is a real URL.  So ideally this is a URL, but we can't really
  //   distinguish this case from:
  // * A "URL-like" string that's not really a URL (like
  //   "browser.tabs.closeButtons" or "java.awt.event.*").  This is ideally a
  //   QUERY.  Since this is indistinguishable from the case above, and this
  //   case is much more likely, claim these are UNKNOWN, which should default
  //   to the right thing and let users correct us on a case-by-case basis.
  return UNKNOWN;
}

// static
void AutocompleteInput::ParseForEmphasizeComponents(
    const base::string16& text,
    url_parse::Component* scheme,
    url_parse::Component* host) {
  url_parse::Parsed parts;
  base::string16 scheme_str;
  Parse(text, base::string16(), &parts, &scheme_str, NULL);

  *scheme = parts.scheme;
  *host = parts.host;

  int after_scheme_and_colon = parts.scheme.end() + 1;
  // For the view-source scheme, we should emphasize the scheme and host of the
  // URL qualified by the view-source prefix.
  if (url_util::LowerCaseEqualsASCII(scheme_str, kViewSourceScheme) &&
      (static_cast<int>(text.length()) > after_scheme_and_colon)) {
    // Obtain the URL prefixed by view-source and parse it.
    base::string16 real_url(text.substr(after_scheme_and_colon));
    url_parse::Parsed real_parts;
    AutocompleteInput::Parse(real_url, base::string16(),
                             &real_parts, NULL, NULL);
    if (real_parts.scheme.is_nonempty() || real_parts.host.is_nonempty()) {
      if (real_parts.scheme.is_nonempty()) {
        *scheme = url_parse::Component(
            after_scheme_and_colon + real_parts.scheme.begin,
            real_parts.scheme.len);
      } else {
        scheme->reset();
      }
      if (real_parts.host.is_nonempty()) {
        *host = url_parse::Component(after_scheme_and_colon + real_parts.host.begin,
                               real_parts.host.len);
      } else {
        host->reset();
      }
    }
  } else if (url_util::LowerCaseEqualsASCII(scheme_str, kFileSystemScheme) &&
             parts.inner_parsed() && parts.inner_parsed()->scheme.is_valid()) {
    *host = parts.inner_parsed()->host;
  }
}

// static
base::string16 AutocompleteInput::FormattedStringWithEquivalentMeaning(
    const GURL& url,
    const base::string16& formatted_url) {
/*
  if (!net::CanStripTrailingSlash(url))
    return formatted_url;
*/
  const base::string16 url_with_path(formatted_url + base::char16('/'));
  return (AutocompleteInput::Parse(formatted_url, base::string16(),
                                   NULL, NULL, NULL) ==
          AutocompleteInput::Parse(url_with_path, base::string16(),
                                   NULL, NULL, NULL)) ?
      formatted_url : url_with_path;
}

// static
int AutocompleteInput::NumNonHostComponents(const url_parse::Parsed& parts) {
  int num_nonhost_components = 0;
  if (parts.scheme.is_nonempty())
    ++num_nonhost_components;
  if (parts.username.is_nonempty())
    ++num_nonhost_components;
  if (parts.password.is_nonempty())
    ++num_nonhost_components;
  if (parts.port.is_nonempty())
    ++num_nonhost_components;
  if (parts.path.is_nonempty())
    ++num_nonhost_components;
  if (parts.query.is_nonempty())
    ++num_nonhost_components;
  if (parts.ref.is_nonempty())
    ++num_nonhost_components;
  return num_nonhost_components;
}

// static
bool AutocompleteInput::HasHTTPScheme(const base::string16& input) {
  std::string utf8_input(WToA(input));
  url_parse::Component scheme;
  if (url_util::FindAndCompareScheme(utf8_input, kViewSourceScheme, &scheme)) {
    utf8_input.erase(0, scheme.end() + 1);
  }
  return url_util::FindAndCompareScheme(utf8_input, kHttpScheme, NULL);
}

void AutocompleteInput::UpdateText(const base::string16& text,
                                   size_t cursor_position,
                                   const url_parse::Parsed& parts) {
  DCHECK(cursor_position <= text.length() ||
         cursor_position == base::string16::npos);
  text_ = text;
  cursor_position_ = cursor_position;
  parts_ = parts;
}

void AutocompleteInput::Clear() {
  text_.clear();
  cursor_position_ = base::string16::npos;
  type_ = INVALID;
  parts_ = url_parse::Parsed();
  scheme_.clear();
  canonicalized_url_ = GURL();
}
