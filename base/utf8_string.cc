#include "utf8_string.h"
#include "icu_utf.h"
#include "logging.h"
#include "utf_string_conversion_utils.h"

namespace base {

  
size_t utf8_string::counter()
{
  const char* data = this->data();
  int32_t length = static_cast<int32_t>(this->length());
  int32_t char_index = 0;
  size_t counter = 0;
  while (char_index < length)
  {
    base_icu::UChar32 code_point = 0;
    CBU8_NEXT(data, char_index, length, code_point);
    if (IsValidCharacter(code_point) && IsValidCodepoint(code_point))
      ++counter;
  }
  return counter;
}


std::string utf8_string::substr(size_type _Off, size_type _Count)
{
  size_t length = this->length();
  if (length == 0 || _Off > length)
    return std::string();

  const char* data = this->data();
  int32_t char_index = 0;
  size_t counter = 0;
  size_t begin_index = 0;
  size_t end_index = length;

  while (char_index < (int32_t)length)
  {
    if (_Off == counter)
    {
      begin_index = char_index;
      if (_Count > length - char_index)
        break;
    }

    base_icu::UChar32 code_point = 0;
    CBU8_NEXT(data, char_index, length, code_point);
    if (IsValidCharacter(code_point) && IsValidCodepoint(code_point))
      ++counter;

    if (_Off + _Count == counter)
    {
      end_index = char_index;
      break;
    }
  }

  if (_Off != 0 && begin_index == 0)
    return std::string();

  return std::string(data + begin_index, data + end_index);
}

}