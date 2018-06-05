#include "macros.h"

#include <algorithm>
#include <string.h>

#include "build_config.h"
#include "icu_utf.h"
#include "utf_string_conversion_utils.h"

namespace {

// Assuming that a pointer is the size of a "machine word", then
// uintptr_t is an integer type that is also a machine word.
typedef uintptr_t MachineWord;
const uintptr_t kMachineWordAlignmentMask = sizeof(MachineWord) - 1;

inline bool IsAlignedToMachineWord(const void* pointer) {
  return !(reinterpret_cast<MachineWord>(pointer) & kMachineWordAlignmentMask);
}

template<typename T> inline T* AlignToMachineWord(T* pointer) {
  return reinterpret_cast<T*>(reinterpret_cast<MachineWord>(pointer) &
                              ~kMachineWordAlignmentMask);
}

template<size_t size, typename CharacterType> struct NonASCIIMask;
template<> struct NonASCIIMask<4, char16_t> {
    static inline uint32_t value() { return 0xFF80FF80U; }
};
template<> struct NonASCIIMask<4, char> {
    static inline uint32_t value() { return 0x80808080U; }
};
template<> struct NonASCIIMask<8, char16_t> {
    static inline uint64_t value() { return 0xFF80FF80FF80FF80ULL; }
};
template<> struct NonASCIIMask<8, char> {
    static inline uint64_t value() { return 0x8080808080808080ULL; }
};
template<> struct NonASCIIMask<4, wchar_t> {
    static inline uint32_t value() { return 0xFFFFFF80U; }
};
template<> struct NonASCIIMask<8, wchar_t> {
    static inline uint64_t value() { return 0xFFFFFF80FFFFFF80ULL; }
};

}

namespace base {

void* AlignedAlloc(size_t size, size_t alignment)
{
  void* ptr = NULL;
#if defined(COMPILER_MSVC)
  ptr = _aligned_malloc(size, alignment);
  // Android technically supports posix_memalign(), but does not expose it in
  // the current version of the library headers used by Chrome.  Luckily,
  // memalign() on Android returns pointers which can safely be used with
  // free(), so we can use it instead.  Issue filed to document this:
  // http://code.google.com/p/android/issues/detail?id=35391
#elif defined(OS_ANDROID)
  ptr = memalign(alignment, size);
#else
  if (posix_memalign(&ptr, alignment, size))
    ptr = NULL;
#endif
  return ptr;
}

template<typename Str>
TrimPositions TrimStringT(const Str& input,
                          const Str& trim_chars,
                          TrimPositions positions,
                          Str* output) {
  // Find the edges of leading/trailing whitespace as desired. Need to use
  // a StringPiece version of input to be able to call find* on it with the
  // StringPiece version of trim_chars (normally the trim_chars will be a
  // constant so avoid making a copy).
  const size_t last_char = input.length() - 1;
  const size_t first_good_char = (positions & TRIM_LEADING) ?
    input.find_first_not_of(trim_chars) : 0;
  const size_t last_good_char = (positions & TRIM_TRAILING) ?
    input.find_last_not_of(trim_chars) : last_char;

  // When the string was all trimmed, report that we stripped off characters
  // from whichever position the caller was interested in. For empty input, we
  // stripped no characters, but we still need to clear |output|.
  if (input.empty() ||
    (first_good_char == Str::npos) || (last_good_char == Str::npos)) {
    bool input_was_empty = input.empty();  // in case output == &input
    output->clear();
    return input_was_empty ? TRIM_NONE : positions;
  }

  // Trim.
  *output =
    input.substr(first_good_char, last_good_char - first_good_char + 1);

  // Return where we trimmed from.
  return static_cast<TrimPositions>(
    ((first_good_char == 0) ? TRIM_NONE : TRIM_LEADING) |
    ((last_good_char == last_char) ? TRIM_NONE : TRIM_TRAILING));
}

bool TrimString(const std::wstring& input,
                const std::wstring& trim_chars,
                std::wstring* output) {
  return TrimStringT(input, trim_chars, TRIM_ALL, output) != TRIM_NONE;
}

bool TrimString(const std::u16string& input,
  const std::u16string& trim_chars,
  std::u16string* output) {
  return TrimStringT(input, trim_chars, TRIM_ALL, output) != TRIM_NONE;
}


bool TrimString(const std::string& input,
                const std::string& trim_chars,
                std::string* output) {
  return TrimStringT(input, trim_chars, TRIM_ALL, output) != TRIM_NONE;
}

template<typename Str>
Str TrimStringPieceT(const Str& input,
                     const Str& trim_chars,
                     TrimPositions positions) {
  size_t begin = (positions & TRIM_LEADING) ?
      input.find_first_not_of(trim_chars) : 0;
  size_t end = (positions & TRIM_TRAILING) ?
      input.find_last_not_of(trim_chars) + 1 : input.size();
  if (begin > input.size()) begin = input.size();
  if (end > input.size()) end = input.size();
  return input.substr(begin, end - begin);
}

template<typename StringType>
StringType ToLowerASCIIImpl(const StringType& str) {
  StringType ret;
  ret.reserve(str.size());
  for (size_t i = 0; i < str.size(); i++)
    ret.push_back(ToLowerASCII(str[i]));
  return ret;
}

std::wstring TrimString(const std::wstring& input,
                        const std::wstring& trim_chars,
                        TrimPositions positions) {
  return TrimStringPieceT(input, trim_chars, positions);
}

std::u16string TrimString(const std::u16string& input,
  const std::u16string& trim_chars,
  TrimPositions positions) {
  return TrimStringPieceT(input, trim_chars, positions);
}

std::string TrimString(const std::string& input,
                       const std::string& trim_chars,
                       TrimPositions positions) {
  return TrimStringPieceT(input, trim_chars, positions);
}

std::string ToLowerASCII(const std::string& str) {
  return ToLowerASCIIImpl<std::string>(str);
}

std::wstring ToLowerASCII(const std::wstring& str) {
  return ToLowerASCIIImpl<std::wstring>(str);
}

std::u16string ToLowerASCII(const std::u16string& str) {
    return ToLowerASCIIImpl<std::u16string>(str);
}

template<class StringType>
int CompareCaseInsensitiveASCIIT(StringType a,
                                 StringType b) {
  // Find the first characters that aren't equal and compare them.  If the end
  // of one of the strings is found before a nonequal character, the lengths
  // of the strings are compared.
  size_t i = 0;
  while (i < a.length() && i < b.length()) {
    typename StringType::value_type lower_a = ToLowerASCII(a[i]);
    typename StringType::value_type lower_b = ToLowerASCII(b[i]);
    if (lower_a < lower_b)
      return -1;
    if (lower_a > lower_b)
      return 1;
    i++;
  }

  // End of one string hit before finding a different character. Expect the
  // common case to be "strings equal" at this point so check that first.
  if (a.length() == b.length())
    return 0;

  if (a.length() < b.length())
    return -1;
  return 1;
}

int CompareCaseInsensitiveASCII(const std::string& a, const std::string& b) {
  return CompareCaseInsensitiveASCIIT<std::string>(a, b);
}

int CompareCaseInsensitiveASCII(const std::wstring& a, const std::wstring& b) {
  return CompareCaseInsensitiveASCIIT<std::wstring>(a, b);
}

bool EqualsCaseInsensitiveASCII(const std::string& a, const std::string& b) {
  if (a.length() != b.length())
    return false;
  return CompareCaseInsensitiveASCIIT<std::string>(a, b) == 0;
}

bool EqualsCaseInsensitiveASCII(const std::wstring& a, const std::wstring& b) {
  if (a.length() != b.length())
    return false;
  return CompareCaseInsensitiveASCIIT<std::wstring>(a, b) == 0;
}


int CompareCaseInsensitiveRemovePuncASCII(const std::string& a, const std::string& b) {
  std::string a_nopunc, b_nopunc;
  RemovePunctuationChars(a, &a_nopunc);
  RemovePunctuationChars(b, &b_nopunc);
  return CompareCaseInsensitiveASCIIT<std::string>(a_nopunc, b_nopunc);
}

int CompareCaseInsensitiveRemovePuncASCII(const std::wstring& a, const std::wstring& b) {
  std::wstring a_nopunc, b_nopunc;
  RemovePunctuationChars(a, &a_nopunc);
  RemovePunctuationChars(b, &b_nopunc);
  return CompareCaseInsensitiveASCIIT<std::wstring>(a_nopunc, b_nopunc);
}


template <class Char>
inline bool DoIsStringASCII(const Char* characters, size_t length) {
  MachineWord all_char_bits = 0;
  const Char* end = characters + length;

  // Prologue: align the input.
  while (!IsAlignedToMachineWord(characters) && characters != end) {
    all_char_bits |= *characters;
    ++characters;
  }

  // Compare the values of CPU word size.
  const Char* word_end = AlignToMachineWord(end);
  const size_t loop_increment = sizeof(MachineWord) / sizeof(Char);
  while (characters < word_end) {
    all_char_bits |= *(reinterpret_cast<const MachineWord*>(characters));
    characters += loop_increment;
  }

  // Process the remaining bytes.
  while (characters != end) {
    all_char_bits |= *characters;
    ++characters;
  }

  MachineWord non_ascii_bit_mask =
      NonASCIIMask<sizeof(MachineWord), Char>::value();
  return !(all_char_bits & non_ascii_bit_mask);
}

bool IsStringASCII(const std::string& str) {
  return DoIsStringASCII(str.data(), str.length());
}

bool IsStringASCII(const std::u16string& str) {
  return DoIsStringASCII(str.data(), str.length());
}

bool IsStringASCII(const std::wstring& str) {
  return DoIsStringASCII(str.data(), str.length());
}

bool IsStringUTF8(const std::string& str) {
  const char *src = str.data();
  int32_t src_len = static_cast<int32_t>(str.length());
  int32_t char_index = 0;

  while (char_index < src_len) {
    int32_t code_point;
    CBU8_NEXT(src, char_index, src_len, code_point);
    if (!IsValidCharacter(code_point))
      return false;
  }
  return true;
}

// Returns either the ASCII or UTF-16 whitespace.
template<typename Str> Str WhitespaceForType();
template<> std::wstring WhitespaceForType<std::wstring>() {
  return kWhitespaceUTF16;
}
template<> std::u16string WhitespaceForType<std::u16string>() {
  return kWhitespaceU16;
}
template<> std::string WhitespaceForType<std::string>() {
  return kWhitespaceASCII;
}

// Optimize the single-character case to call find() on the string instead,
// since this is the common case and can be made faster. This could have been
// done with template specialization too, but would have been less clear.
//
// There is no corresponding FindFirstNotOf because StringPiece already
// implements these different versions that do the optimized searching.
size_t FindFirstOf(const std::string& piece, char c, size_t pos, bool find_all) {
  return piece.find(c, pos);
}
size_t FindFirstOf(const std::wstring& piece, wchar_t c, size_t pos, bool find_all) {
  return piece.find(c, pos);
}
size_t FindFirstOf(const std::u16string& piece, char16_t c, size_t pos, bool find_all) {
  return piece.find(c, pos);
}
size_t FindFirstOf(const std::string& piece, const std::string& one_of, size_t pos, bool find_all) {
  return find_all ? piece.find(one_of, pos) : piece.find_first_of(one_of, pos);
}
size_t FindFirstOf(const std::wstring& piece, const std::wstring& one_of, size_t pos, bool find_all) {
  return find_all ? piece.find(one_of, pos) : piece.find_first_of(one_of, pos);
}
size_t FindFirstOf(const std::u16string& piece, const std::u16string& one_of, size_t pos, bool find_all) {
  return find_all ? piece.find(one_of, pos) : piece.find_first_of(one_of, pos);
}

size_t GetAdvanceLen(char c, bool find_all) {
  return 1;
}

size_t GetAdvanceLen(wchar_t c, bool find_all) {
  return 1;
}

size_t GetAdvanceLen(char16_t c, bool find_all) {
  return 1;
}


size_t GetAdvanceLen(const std::string& one_of, bool find_all) {
  return find_all ? one_of.size() : 1;
}

size_t GetAdvanceLen(const std::wstring& one_of, bool find_all) {
  return find_all ? one_of.size() : 1;
}

size_t GetAdvanceLen(const std::u16string& one_of, bool find_all) {
  return find_all ? one_of.size() : 1;
}

// General string splitter template. Can take 8- or 16-bit input, can produce
// the corresponding string or StringPiece output, and can take single- or
// multiple-character delimiters.
//
// DelimiterType is either a character (Str::value_type) or a string piece of
// multiple characters (BasicStringPiece<Str>). StringPiece has a version of
// find for both of these cases, and the single-character version is the most
// common and can be implemented faster, which is why this is a template.
template<typename Str, typename OutputStringType, typename DelimiterType>
static std::vector<OutputStringType> SplitStringT(
    const Str& str,
    const DelimiterType& delimiter,
    SeparatorType separator_type,
    WhitespaceHandling whitespace,
    SplitResult result_type) {
  std::vector<OutputStringType> result;
  if (str.empty())
    return result;

  size_t start = 0;
  while (start != Str::npos) {
    size_t end = FindFirstOf(str, delimiter, start, separator_type == SEPARATOR_ALL);

    Str piece;
    if (end == Str::npos) {
      piece = str.substr(start);
      start = Str::npos;
    } else {
      piece = str.substr(start, end - start);
      start = end + GetAdvanceLen(delimiter, separator_type == SEPARATOR_ALL);
    }

    if (whitespace == TRIM_WHITESPACE)
      piece = TrimString(piece, WhitespaceForType<Str>(), TRIM_ALL);

    if (result_type == SPLIT_WANT_ALL || !piece.empty())
      result.push_back(piece);
  }
  return result;
}

std::vector<std::string> SplitString(const std::string& input,
                                     const std::string& separators,
                                     SeparatorType separator_type,
                                     WhitespaceHandling whitespace,
                                     SplitResult result_type) {
  if (separators.size() == 1) {
    return SplitStringT<std::string, std::string, char>(
        input, separators[0], SEPARATOR_ONE, whitespace, result_type);
  }
  return SplitStringT<std::string, std::string, std::string>(
      input, separators, separator_type, whitespace, result_type);
}

std::vector<std::wstring> SplitString(const std::wstring& input,
                                      const std::wstring& separators,
                                      SeparatorType separator_type,
                                      WhitespaceHandling whitespace,
                                      SplitResult result_type) {
  if (separators.size() == 1) {
    return SplitStringT<std::wstring, std::wstring, wchar_t>(
        input, separators[0], SEPARATOR_ONE, whitespace, result_type);
  }
  return SplitStringT<std::wstring, std::wstring, std::wstring>(
      input, separators, separator_type, whitespace, result_type);
}

std::vector<std::u16string> SplitString(const std::u16string& input,
  const std::u16string& separators,
  SeparatorType separator_type,
  WhitespaceHandling whitespace,
  SplitResult result_type) {
  if (separators.size() == 1) {
    return SplitStringT<std::u16string, std::u16string, char16_t>(
      input, separators[0], SEPARATOR_ONE, whitespace, result_type);
  }
  return SplitStringT<std::u16string, std::u16string, std::u16string>(
    input, separators, separator_type, whitespace, result_type);
}


template<typename STR>
static STR JoinStringT(const std::vector<STR>& parts,
                       STR sep) {
  if (parts.empty())
    return STR();

  STR result(parts[0]);
  auto iter = parts.begin();
  ++iter;

  for (; iter != parts.end(); ++iter) {
    result += sep;
    result += *iter;
  }

  return result;
}

std::string JoinString(const std::vector<std::string>& parts,
                       const std::string& separator) {
  return JoinStringT(parts, separator);
}

std::wstring JoinString(const std::vector<std::wstring>& parts,
                        const std::wstring& separator) {
  return JoinStringT(parts, separator);
}

std::u16string JoinString(const std::vector<std::u16string>& parts,
                          const std::u16string& separator) {
    return JoinStringT(parts, separator);
}

// static
bool IsNonCoalescingHeader(std::string::const_iterator name_begin,
                           std::string::const_iterator name_end) {
  // NOTE: "set-cookie2" headers do not support expires attributes, so we don't
  // have to list them here.
  const char* const kNonCoalescingHeaders[] = {
    "date",
    "expires",
    "last-modified",
    "location",  // See bug 1050541 for details
    "retry-after",
    "set-cookie",
    // The format of auth-challenges mixes both space separated tokens and
    // comma separated properties, so coalescing on comma won't work.
    "www-authenticate",
    "proxy-authenticate",
    // STS specifies that UAs must not process any STS headers after the first
    // one.
    "strict-transport-security"
  };
  for (size_t i = 0; i < arraysize(kNonCoalescingHeaders); ++i) {
    if (EqualsCaseInsensitiveASCII(std::string(name_begin, name_end),
                                   kNonCoalescingHeaders[i]))
      return true;
  }
  return false;
}

bool IsNonCoalescingHeader(const std::string& name) {
  return IsNonCoalescingHeader(name.begin(), name.end());
}

bool IsLWS(char c) {
  return strchr(HTTP_LWS, c) != NULL;
}

void TrimLWS(std::string::const_iterator* begin,
             std::string::const_iterator* end) {
  // leading whitespace
  while (*begin < *end && IsLWS((*begin)[0]))
    ++(*begin);

  // trailing whitespace
  while (*begin < *end && IsLWS((*end)[-1]))
    --(*end);
}

// Helper used by AssembleRawHeaders, to skip past leading LWS.
const char* FindFirstNonLWS(const char* begin, const char* end) {
  for (const char* cur = begin; cur != end; ++cur) {
    if (!IsLWS(*cur))
      return cur;
  }
  return end;  // Not found.
}

template<typename STR>
bool ReplaceCharsT(const STR& input,
                   const STR& replace_chars,
                   const STR& replace_with,
                   STR* output) {
  bool removed = false;
  size_t replace_length = replace_with.length();

  *output = input;

  size_t found = output->find_first_of(replace_chars);
  while (found != STR::npos) {
    removed = true;
    output->replace(found, 1, replace_with);
    found = output->find_first_of(replace_chars, found + replace_length);
  }

  return removed;
}

bool RemovePunctuationChars(const std::wstring& input, std::wstring* output) {
  return RemoveChars(input, L".,?!#%$&+*-/;:=<?>@[]\\_`{}|~。，？！'\" ‘’“”《》·", output);
}

bool RemovePunctuationChars(const std::string& input, std::string* output) {
  return RemoveChars(input, u8".,?!#%$&+*-/;:=<?>@[]\\_`{}|~。，？！'\" ‘’“”《》·", output);
}

bool ReplaceChars(const std::wstring& input,
                  const std::wstring& replace_chars,
                  const std::wstring& replace_with,
                  std::wstring* output) {
  return ReplaceCharsT(input, replace_chars, replace_with, output);
}

bool ReplaceChars(const std::string& input,
                  const std::string& replace_chars,
                  const std::string& replace_with,
                  std::string* output) {
  return ReplaceCharsT(input, replace_chars, replace_with, output);
}

bool ReplaceChars(const std::u16string& input,
                  const std::u16string& replace_chars,
                  const std::u16string& replace_with,
                  std::u16string* output) {
  return ReplaceCharsT(input, replace_chars, replace_with, output);
}

bool RemoveChars(const std::wstring& input,
                 const std::wstring& remove_chars,
                 std::wstring* output) {
  return ReplaceChars(input, remove_chars, std::wstring(), output);
}

bool RemoveChars(const std::string& input,
                 const std::string& remove_chars,
                 std::string* output) {
  return ReplaceChars(input, remove_chars, std::string(), output);
}

bool RemoveChars(const std::u16string& input,
                 const std::u16string& remove_chars,
                 std::u16string* output) {
  return ReplaceChars(input, remove_chars, std::u16string(), output);
}

}
