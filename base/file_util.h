#pragma once
#include <string>
#include <tuple>
#include <vector>

namespace base
{
  namespace file
  {
    std::tuple<bool, std::string> ReadFileToString(const std::string& filepath);

    // add by wujian
    // SplitString的参数是 base::SEPARATOR_ALL, base::TRIM_WHITESPACE, base::SPLIT_WANT_ALL
    // 这个用的最多，有别的需求再说
    std::vector<std::string> ReadFileToLines(const std::string& filepath, const std::string& separators = "\n") noexcept;
    
    bool WriteFile(const std::string& filepath, const std::string& content);
  }
}
