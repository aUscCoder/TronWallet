#include "file_util.h"
#include <fstream>
#include "logging.h"

std::vector<std::string> base::file::ReadFileToLines(const std::string& filepath, const std::string& separators) noexcept
{
  std::vector<std::string> lines;
  bool success = false;
  std::string content;
  std::tie(success, content) = base::file::ReadFileToString(filepath);
  if (success && !content.empty())
    lines = base::SplitString(content, separators, base::SEPARATOR_ALL, base::TRIM_WHITESPACE, base::SPLIT_WANT_ALL);
  return lines;
}

std::tuple<bool, std::string> base::file::ReadFileToString(const std::string& filepath)
{
  std::string content;
  std::ifstream fin(filepath, std::ios::in | std::ios::binary);
  bool ok = fin.is_open();
  if (ok)
  {
    
    fin.seekg(0, std::ios::end);
    content.reserve((int)fin.tellg());
    fin.seekg(0, std::ios::beg);

    content.assign((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
    fin.close();
  }

  return std::make_tuple(ok, content);
}

bool base::file::WriteFile(const std::string& filepath, const std::string& content)
{
  std::ofstream fout(filepath, std::ios::out | std::ios::binary);
  bool ok = fout.is_open();
  if (ok) {
      fout.write(content.c_str(), content.length());
      fout.close();
  }
  return ok;
}

