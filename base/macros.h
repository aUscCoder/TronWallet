// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file contains macros and macro-like constructs (e.g., templates) that
// are commonly used throughout Chromium source. (It may also contain things
// that are closely related to things that are commonly used that belong in this
// file.)

#ifndef BASE_MACROS_H_
#define BASE_MACROS_H_

#include <algorithm>
#include <atomic>
#include <memory>
#include <string>
#include <stddef.h>  // For size_t.
#include <vector>
#include <stdlib.h>  // for free()

#include "build_config.h"

// Put this in the declarations for a class to be uncopyable.
#define DISALLOW_COPY(TypeName) \
  TypeName(const TypeName&) = delete

// Put this in the declarations for a class to be unassignable.
#define DISALLOW_ASSIGN(TypeName) \
  void operator=(const TypeName&) = delete

// A macro to disallow the copy constructor and operator= functions.
// This should be used in the private: declarations for a class.
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&) = delete;      \
  void operator=(const TypeName&) = delete

// A macro to disallow all the implicit constructors, namely the
// default constructor, copy constructor and operator= functions.
//
// This should be used in the private: declarations for a class
// that wants to prevent anyone from instantiating it. This is
// especially useful for classes containing only static methods.
#define DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
  TypeName() = delete;                           \
  DISALLOW_COPY_AND_ASSIGN(TypeName)

// The arraysize(arr) macro returns the # of elements in an array arr.  The
// expression is a compile-time constant, and therefore can be used in defining
// new arrays, for example.  If you use arraysize on a pointer by mistake, you
// will get a compile-time error.  For the technical details, refer to
// http://blogs.msdn.com/b/the1/archive/2004/05/07/128242.aspx.

// This template function declaration is used in defining arraysize.
// Note that the function doesn't need an implementation, as we only
// use its type.
template <typename T, size_t N> char(&ArraySizeHelper(T(&array)[N]))[N];
#define arraysize(array) (sizeof(ArraySizeHelper(array)))

namespace internal {

template <typename Collection>
class HasKeyType {
  template <typename C>
  static std::true_type test(typename C::key_type*);
  template <typename C>
  static std::false_type test(...);

 public:
  static constexpr bool value = decltype(test<Collection>(nullptr))::value;
};

}  // namespace internal

// Test to see if a collection like a vector contains a particular value.
// Returns true if the value is in the collection.
// Don't use this on collections such as sets or maps. This is enforced by
// disabling this method if the collection defines a key_type.
template <typename Collection,
  typename Value,
  typename std::enable_if<!internal::HasKeyType<Collection>::value,
  int>::type = 0>
  bool ContainsValue(const Collection& collection, const Value& value) {
  return std::find(std::begin(collection), std::end(collection), value) !=
    std::end(collection);
}

// Used to explicitly mark the return value of a function as unused. If you are
// really sure you don't want to do anything with the return value of a function
// that has been marked WARN_UNUSED_RESULT, wrap it with this. Example:
//
//   std::unique_ptr<MyType> my_var = ...;
//   if (TakeOwnership(my_var.get()) == SUCCESS)
//     ignore_result(my_var.release());
//
template<typename T>
inline void ignore_result(const T&) {
}

// Annotate a function indicating it should not be inlined.
// Use like:
//   NOINLINE void DoStuff() { ... }
#if defined(COMPILER_GCC)
#define NOINLINE __attribute__((noinline))
#elif defined(COMPILER_MSVC)
#define NOINLINE __declspec(noinline)
#else
#define NOINLINE
#endif

// TODO(avi): do we want to do a unification of character types here?
#if defined(OS_WIN)
typedef wchar_t PathChar;
#else
typedef char PathChar;
#endif

#if defined(OS_WIN)
typedef std::wstring PathString;
#else
typedef std::string PathString;
#endif

// This is a macro to support extending this string literal at compile time.
// Please excuse me polluting your global namespace!
#define HTTP_LWS " \t"

// The following enum should be used only as a constructor argument to indicate
// that the variable has static storage class, and that the constructor should
// do nothing to its state.  It indicates to the reader that it is legal to
// declare a static instance of the class, provided the constructor is given
// the base::LINKER_INITIALIZED argument.  Normally, it is unsafe to declare a
// static variable that has a constructor or a destructor because invocation
// order is undefined.  However, IF the type can be initialized by filling with
// zeroes (which the loader does for static variables), AND the destructor also
// does nothing to the storage, AND there are no virtual methods, then a
// constructor declared as
//       explicit MyClass(base::LinkerInitialized x) {}
// and invoked as
//       static MyClass my_variable_name(base::LINKER_INITIALIZED);
namespace base {

namespace internal {

template <typename First, typename Second>
struct SelectSecond {
    using type = Second;
};

struct Any {
    Any(...);
};

// True case: If |Lvalue| can be assigned to from |Rvalue|, then the return
// value is a true_type.
template <class Lvalue, class Rvalue>
typename internal::SelectSecond<
    decltype((std::declval<Lvalue>() = std::declval<Rvalue>())),
    std::true_type>::type
    IsAssignableTest(Lvalue&&, Rvalue&&);

// False case: Otherwise the return value is a false_type.
template <class Rvalue>
std::false_type IsAssignableTest(internal::Any, Rvalue&&);

// Default case: Neither Lvalue nor Rvalue is void. Uses IsAssignableTest to
// determine the type of IsAssignableImpl.
template <class Lvalue,
    class Rvalue,
    bool = std::is_void<Lvalue>::value || std::is_void<Rvalue>::value>
    struct IsAssignableImpl
    : public std::common_type<decltype(
        internal::IsAssignableTest(std::declval<Lvalue>(),
            std::declval<Rvalue>()))>::type {};

// Void case: Either Lvalue or Rvalue is void. Then the type of IsAssignableTest
// is false_type.
template <class Lvalue, class Rvalue>
struct IsAssignableImpl<Lvalue, Rvalue, true> : public std::false_type {};

// Uses expression SFINAE to detect whether using operator<< would work.
template <typename T, typename = void>
struct SupportsOstreamOperator : std::false_type {};
template <typename T>
struct SupportsOstreamOperator<T,
    decltype(void(std::declval<std::ostream&>()
        << std::declval<T>()))>
    : std::true_type {};

}  // namespace internal

enum LinkerInitialized { LINKER_INITIALIZED };

    // Use these to declare and define a static local variable (static T;) so that
    // it is leaked so that its destructors are not called at exit. If you need
    // thread-safe initialization, use base/lazy_instance.h instead.
#define CR_DEFINE_STATIC_LOCAL(type, name, arguments) \
  static type& name = *new type arguments

// Helper to transfer ownership of a raw pointer to a std::unique_ptr<T>.
// Note that std::unique_ptr<T> has very different semantics from
// std::unique_ptr<T[]>: do not use this helper for array allocations.
template <typename T>
std::unique_ptr<T> WrapUnique(T* ptr) {
  return std::unique_ptr<T>(ptr);
}

template <typename T>
std::shared_ptr<T> WrapShared(T* ptr) {
  return std::shared_ptr<T>(ptr);
}

// std::less<> from C++14.
struct less {
  template <typename T, typename U>
  constexpr auto operator()(T&& lhs, U&& rhs) const
      -> decltype(std::forward<T>(lhs) < std::forward<U>(rhs)) {
    return std::forward<T>(lhs) < std::forward<U>(rhs);
  }

  // You can find more information about transparent comparisons here:
  // http://en.cppreference.com/w/cpp/utility/functional/less_void
  using is_transparent = int;
};

// Deleter for use with unique_ptr. E.g., use as
//   std::unique_ptr<Foo, base::AlignedFreeDeleter> foo;
struct AlignedFreeDeleter {
    inline void operator()(void* ptr) const {
#if defined(COMPILER_MSVC)
        _aligned_free(ptr);
#else
        free(ptr);
#endif
    }
};

void* AlignedAlloc(size_t size, size_t alignment);

enum TrimPositions {
  TRIM_NONE     = 0,
  TRIM_LEADING  = 1 << 0,
  TRIM_TRAILING = 1 << 1,
  TRIM_ALL      = TRIM_LEADING | TRIM_TRAILING,
};

// Removes characters in |trim_chars| from the beginning and end of |input|.
// The 8-bit version only works on 8-bit characters, not UTF-8.
//
// It is safe to use the same variable for both |input| and |output| (this is
// the normal usage to trim in-place).
bool TrimString(const std::wstring& input,
                const std::wstring& trim_chars,
                std::wstring* output);
bool TrimString(const std::string& input,
                const std::string& trim_chars,
                std::string* output);

// std::string versions of the above. The returned pieces refer to the original
// buffer.
std::wstring TrimString(const std::wstring& input,
                        const std::wstring& trim_chars,
                        TrimPositions positions);
std::string TrimString(const std::string& input,
                       const std::string& trim_chars,
                       TrimPositions positions);

// ASCII-specific tolower.  The standard library's tolower is locale sensitive,
// so we don't want to use it here.
inline char ToLowerASCII(char c) {
  return (c >= 'A' && c <= 'Z') ? (c + ('a' - 'A')) : c;
}
inline wchar_t ToLowerASCII(wchar_t c) {
  return (c >= 'A' && c <= 'Z') ? (c + ('a' - 'A')) : c;
}
inline char16_t ToLowerASCII(char16_t c) {
    return (c >= 'A' && c <= 'Z') ? (c + ('a' - 'A')) : c;
}

// Converts the given string to it's ASCII-lowercase equivalent.
std::string ToLowerASCII(const std::string& str);
std::wstring ToLowerASCII(const std::wstring& str);
std::u16string ToLowerASCII(const std::u16string& str);

// Functor for case-insensitive ASCII comparisons for STL algorithms like
// std::search.
//
// Note that a full Unicode version of this functor is not possible to write
// because case mappings might change the number of characters, depend on
// context (combining accents), and require handling UTF-16. If you need
// proper Unicode support, use base::i18n::ToLower/FoldCase and then just
// use a normal operator== on the result.
template<typename Char> struct CaseInsensitiveCompareASCII {
 public:
  bool operator()(Char x, Char y) const {
    return ToLowerASCII(x) == ToLowerASCII(y);
  }
};

// Like strcasecmp for case-insensitive ASCII characters only. Returns:
//   -1  (a < b)
//    0  (a == b)
//    1  (a > b)
// (unlike strcasecmp which can return values greater or less than 1/-1). For
// full Unicode support, use base::i18n::ToLower or base::i18h::FoldCase
// and then just call the normal string operators on the result.
int CompareCaseInsensitiveASCII(const std::string& a, const std::string& b);
int CompareCaseInsensitiveASCII(const std::wstring& a, const std::wstring& b);

// Equality for ASCII case-insensitive comparisons. For full Unicode support,
// use base::i18n::ToLower or base::i18h::FoldCase and then compare with either
// == or !=.
bool EqualsCaseInsensitiveASCII(const std::string& a, const std::string& b);
bool EqualsCaseInsensitiveASCII(const std::wstring& a, const std::wstring& b);

int CompareCaseInsensitiveRemovePuncASCII(const std::string& a, const std::string& b);
int CompareCaseInsensitiveRemovePuncASCII(const std::wstring& a, const std::wstring& b);

// Returns true if the specified string matches the criteria. How can a wide
// string be 8-bit or UTF8? It contains only characters that are < 256 (in the
// first case) or characters that use only 8-bits and whose 8-bit
// representation looks like a UTF-8 string (the second case).
//
// Note that IsStringUTF8 checks not only if the input is structurally
// valid but also if it doesn't contain any non-character codepoint
// (e.g. U+FFFE). It's done on purpose because all the existing callers want
// to have the maximum 'discriminating' power from other encodings. If
// there's a use case for just checking the structural validity, we have to
// add a new function for that.
//
// IsStringASCII assumes the input is likely all ASCII, and does not leave early
// if it is not the case.
bool IsStringUTF8(const std::string& str);
bool IsStringASCII(const std::string& str);
// A convenience adaptor for WebStrings, as they don't convert into
// StringPieces directly.
bool IsStringASCII(const std::u16string& str);
bool IsStringASCII(const std::wstring& str);

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

const wchar_t kWhitespaceUTF16[] = {
  WHITESPACE_UNICODE
};

const char16_t kWhitespaceU16[] = {
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

enum SeparatorType {
  SEPARATOR_ALL,
  SEPARATOR_ONE,
};

enum WhitespaceHandling {
  KEEP_WHITESPACE,
  TRIM_WHITESPACE,
};

enum SplitResult {
  // Strictly return all results.
  //
  // If the input is ",," and the separator is ',' this will return a
  // vector of three empty strings.
  SPLIT_WANT_ALL,

  // Only nonempty results will be added to the results. Multiple separators
  // will be coalesced. Separators at the beginning and end of the input will
  // be ignored. With TRIM_WHITESPACE, whitespace-only results will be dropped.
  //
  // If the input is ",," and the separator is ',', this will return an empty
  // vector.
  SPLIT_WANT_NONEMPTY,
};

// Split the given string on ANY of the given separators, returning copies of
// the result.
//
// To split on either commas or semicolons, keeping all whitespace:
//
//   std::vector<std::string> tokens = base::SplitString(
//       input, ",;", base::KEEP_WHITESPACE, base::SPLIT_WANT_ALL);
std::vector<std::string> SplitString(
    const std::string& input,
    const std::string& separators,
    SeparatorType separator_type,
    WhitespaceHandling whitespace,
    SplitResult result_type);
std::vector<std::wstring> SplitString(
    const std::wstring& input,
    const std::wstring& separators,
    SeparatorType separator_type,
    WhitespaceHandling whitespace,
    SplitResult result_type);
std::vector<std::u16string> SplitString(
  const std::u16string& input,
  const std::u16string& separators,
  SeparatorType separator_type,
  WhitespaceHandling whitespace,
  SplitResult result_type);


// Does the opposite of SplitString().
std::string JoinString(const std::vector<std::string>& parts,
                                   const std::string& separator);
std::wstring JoinString(const std::vector<std::wstring>& parts,
                        const std::wstring& separator);
std::u16string JoinString(const std::vector<std::u16string>& parts,
                          const std::u16string& separator);


// Multiple occurances of some headers cannot be coalesced into a comma-
// separated list since their values are (or contain) unquoted HTTP-date
// values, which may contain a comma (see RFC 2616 section 3.3.1).
bool IsNonCoalescingHeader(std::string::const_iterator name_begin,
                           std::string::const_iterator name_end);
bool IsNonCoalescingHeader(const std::string& name);

// Return true if the character is HTTP "linear white space" (SP | HT).
// This definition corresponds with the HTTP_LWS macro, and does not match
// newlines.
bool IsLWS(char c);

// Trim HTTP_LWS chars from the beginning and end of the string.
void TrimLWS(std::string::const_iterator* begin,
             std::string::const_iterator* end);

// Removes characters in |remove_chars| from anywhere in |input|.  Returns true
// if any characters were removed.  |remove_chars| must be null-terminated.
// NOTE: Safe to use the same variable for both |input| and |output|.
bool RemoveChars(const std::wstring& input,
                 const std::wstring& remove_chars,
                 std::wstring* output);
bool RemoveChars(const std::string& input,
                 const std::string& remove_chars,
                 std::string* output);
bool RemoveChars(const std::u16string& input,
                 const std::u16string& remove_chars,
                 std::u16string* output);


bool RemovePunctuationChars(const std::wstring& input, std::wstring* output);
bool RemovePunctuationChars(const std::string& input, std::string* output);

// Replaces characters in |replace_chars| from anywhere in |input| with
// |replace_with|.  Each character in |replace_chars| will be replaced with
// the |replace_with| string.  Returns true if any characters were replaced.
// |replace_chars| must be null-terminated.
// NOTE: Safe to use the same variable for both |input| and |output|.
bool ReplaceChars(const std::wstring& input,
                  const std::wstring& replace_chars,
                  const std::wstring& replace_with,
                  std::wstring* output);
bool ReplaceChars(const std::string& input,
                  const std::string& replace_chars,
                  const std::string& replace_with,
                  std::string* output);
bool ReplaceChars(const std::u16string& input,
                  const std::u16string& replace_chars,
                  const std::u16string& replace_with,
                  std::u16string* output);

namespace internal {

template <typename T>
struct MakeUniqueResult {
  using Scalar = std::unique_ptr<T>;
};

template <typename T>
struct MakeUniqueResult<T[]> {
  using Array = std::unique_ptr<T[]>;
};

template <typename T, size_t N>
struct MakeUniqueResult<T[N]> {
  using Invalid = void;
};

}  // namespace internal

// Helper to construct an object wrapped in a std::unique_ptr. This is an
// implementation of C++14's std::make_unique that can be used in Chrome.
//
// MakeUnique<T>(args) should be preferred over WrapUnique(new T(args)): bare
// calls to `new` should be treated with scrutiny.
//
// Usage:
//   // ptr is a std::unique_ptr<std::string>
//   auto ptr = MakeUnique<std::string>("hello world!");
//
//   // arr is a std::unique_ptr<int[]>
//   auto arr = MakeUnique<int[]>(5);

// Overload for non-array types. Arguments are forwarded to T's constructor.
template <typename T, typename... Args>
typename internal::MakeUniqueResult<T>::Scalar MakeUnique(Args&&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// Overload for array types of unknown bound, e.g. T[]. The array is allocated
// with `new T[n]()` and value-initialized: note that this is distinct from
// `new T[n]`, which default-initializes.
template <typename T>
typename internal::MakeUniqueResult<T>::Array MakeUnique(size_t size) {
  return std::unique_ptr<T>(new typename std::remove_extent<T>::type[size]());
}

// Overload to reject array types of known bound, e.g. T[n].
template <typename T, typename... Args>
typename internal::MakeUniqueResult<T>::Invalid MakeUnique(Args&&... args) =
    delete;

// Specify memory alignment for structs, classes, etc.
// Use like:
//   class ALIGNAS(16) MyClass { ... }
//   ALIGNAS(16) int array[4];
#if defined(COMPILER_MSVC)
#define ALIGNAS(byte_alignment) __declspec(align(byte_alignment))
#elif defined(COMPILER_GCC)
#define ALIGNAS(byte_alignment) __attribute__((aligned(byte_alignment)))
#endif

namespace subtle {

inline void MemoryBarrier() {
#if defined(__GLIBCXX__)
  // Work around libstdc++ bug 51038 where atomic_thread_fence was declared but
  // not defined, leading to the linker complaining about undefined references.
  __atomic_thread_fence(std::memory_order_seq_cst);
#else
  std::atomic_thread_fence(std::memory_order_seq_cst);
#endif
}

}  // subtle

}  // base

#endif  // BASE_MACROS_H_
