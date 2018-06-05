#include <string>

namespace base {
  
class utf8_string : public std::string
{
public:
  utf8_string() = default;
  utf8_string(const std::string& _Right) : std::string(_Right) {}
  utf8_string(const char* _Ptr) : std::string(_Ptr) {}
  size_t counter();
  std::string substr(size_type _Off, size_type _Count);
};

}

