#include "config_service.h"

#include <atomic>

#include "build_config.h"
#include "logging.h"
#include "json_file_value_serializer.h"

#if defined(OS_WIN)
#include <windows.h>
#include "win/current_module.h"
#endif

#include "sys_string_conversions.h"
#include "macros.h"
#include "string_number_conversions.h"
#include "json_string_value_serializer.h"

static std::atomic<base::ConfigService*> s_config_service;

base::ConfigService* base::ConfigService::GetInstance() {
  if (s_config_service == (base::ConfigService*)nullptr) {
    s_config_service = new ConfigService();
  }
  return s_config_service;
}

bool base::ConfigService::OverrideConfig(const std::string& config_string) {
  bool override = (s_config_service != (base::ConfigService*)nullptr);
  s_config_service = new ConfigService(config_string, false);
  return override;
}

bool base::ConfigService::GetBoolean(const std::string& path,
                                     bool default_value) const {
  if (config_json_) {
    config_json_->GetBoolean(path, &default_value);
  }
  return default_value;
}

int base::ConfigService::GetInteger(const std::string& path,
                                    int default_value) const {
  if (config_json_) {
    config_json_->GetInteger(path, &default_value);
  }
  return default_value;
}

double base::ConfigService::GetDouble(const std::string & path,
                                      double default_value) const {
  if (config_json_) {
    config_json_->GetDouble(path, &default_value);
  }
  return default_value;
}

std::string base::ConfigService::GetString(const std::string& path,
                                           const std::string& default_value) const {
  std::string result_value = default_value;
  if (config_json_) {
    config_json_->GetString(path, &result_value);
  }
  return result_value;
}

std::vector<int> base::ConfigService::GetIntList(const std::string& path,
                                                 int default_value) const {
  std::vector<int> result;
  if (config_json_) {
    base::ListValue* list_value = nullptr;
    if (config_json_->GetList(path, &list_value)) {
      for (size_t i = 0; i < list_value->GetSize(); i++) {
        int int_value = -1;
        if (list_value->GetInteger(i, &int_value)) {
          result.push_back(int_value);
        }
      }
    }
  }
  // Add default value if the result is empty.
  if (result.empty())
    result.push_back(default_value);
  return result;
}

bool base::ConfigService::GetIntList(const std::string& path,
                                     std::vector<int>& int_list) const {
  if (config_json_) {
    base::ListValue* list_value = nullptr;
    if (config_json_->GetList(path, &list_value)) {
      for (size_t i = 0; i < list_value->GetSize(); i++) {
        int int_value = -1;
        if (list_value->GetInteger(i, &int_value)) {
          int_list.push_back(int_value);
        }
      }
    }
  }
  return !int_list.empty();
}

std::vector<double> base::ConfigService::GetDoubleList(const std::string& path,
                                                       double default_value) const {
  std::vector<double> result;
  if (config_json_) {
    base::ListValue* list_value = nullptr;
    if (config_json_->GetList(path, &list_value)) {
      for (size_t i = 0; i < list_value->GetSize(); i++) {
        double double_value = -1;
        if (list_value->GetDouble(i, &double_value)) {
          result.push_back(double_value);
        }
      }
    }
  }
  // Add default value if the result is empty.
  if (result.empty())
    result.push_back(default_value);
  return result;
}

bool base::ConfigService::GetDoubleList(const std::string& path,
                                        std::vector<double>& double_list) const {
  if (config_json_) {
    base::ListValue* list_value = nullptr;
    if (config_json_->GetList(path, &list_value)) {
      for (size_t i = 0; i < list_value->GetSize(); i++) {
        double double_value = -1;
        if (list_value->GetDouble(i, &double_value)) {
          double_list.push_back(double_value);
        }
      }
    }
  }
  return !double_list.empty();
}

std::vector<std::string> base::ConfigService::GetStringArray(const std::string& path,
                                                             const std::string& default_value) const {
  std::vector<std::string> result;
  if (config_json_) {
    base::ListValue* list_value = nullptr;
    if (config_json_->GetList(path, &list_value)) {
      for (size_t i = 0; i < list_value->GetSize(); i++) {
        std::string string_value;
        if (list_value->GetString(i, &string_value)) {
          result.push_back(string_value);
        }
      }
    }
  }
  // Add default value if the result is empty.
  if (result.empty())
    result.push_back(default_value);
  return result;
}

//
// format should be YYYY-MM-DD HH:MM:SS
//
std::chrono::system_clock::time_point base::ConfigService::StrToTimePoint(const std::string& timeString)
{
  std::chrono::system_clock::time_point tp; // default to now
  auto tokens = base::SplitString(timeString, " ", base::SEPARATOR_ALL, base::KEEP_WHITESPACE, base::SPLIT_WANT_ALL);

  auto getThreeNumber = [](const std::string& source, const std::string& seperator, int& first, int& second, int& third) mutable
  {
    auto tokens = base::SplitString(source, seperator, base::SEPARATOR_ALL, base::TRIM_WHITESPACE, base::SPLIT_WANT_ALL);
    CHECK(tokens.size() == 3);
    CHECK(base::StringToInt(tokens[0], &first));
    CHECK(base::StringToInt(tokens[1], &second));
    CHECK(base::StringToInt(tokens[2], &third));
  };

  if (tokens.size() > 0)
  {
    tm time = { 0 };

    // deal with YYYY-MM-DD
    const std::string& YMD = tokens[0];
    getThreeNumber(YMD, "-", time.tm_year, time.tm_mon, time.tm_mday);
    time.tm_year -= 1900;
    time.tm_mon -= 1;

    if (tokens.size() > 1)
    {
      // deal with HH:MM:SS
      const std::string& HMS = tokens[1];
      getThreeNumber(HMS, ":", time.tm_hour, time.tm_min, time.tm_sec);
    }

    tp = std::chrono::system_clock::from_time_t(mktime(&time));
  }
  return tp;
}

base::ConfigService::ConfigService(const std::string& config_path, bool is_path) {
  if (!is_path) {
    JSONStringValueDeserializer config_json_serializer(config_path);

    // Load the config.
    int error_code = 0;
    std::string error_message;
    std::unique_ptr<base::Value> value = config_json_serializer.Deserialize(&error_code, &error_message);
    if (value && value->IsType(base::Value::TYPE_DICTIONARY)) {
      config_json_.reset(static_cast<base::DictionaryValue*>(value.release()));
    }
    LOG(INFO) << "Load config buffer. err_code = " << error_code << ", err_msg = " << error_message;
  } else {
    std::string module_directory;
#if defined(OS_WIN)
    if (config_path.find(":") == std::string::npos) {
      char module_name[MAX_PATH];
      GetModuleFileNameA(CURRENT_MODULE(), module_name, MAX_PATH);

      module_directory = module_name;
      size_t last_backslash = module_directory.rfind('\\', module_directory.size());
      if (last_backslash != std::string::npos)
        module_directory.erase(last_backslash + 1);
    }
#endif

    std::string config_json_path(module_directory + config_path);

    JSONFileValueDeserializer config_json_serializer(config_json_path);

    // Load the config.
    int error_code = 0;
    std::string error_message;
    std::unique_ptr<base::Value> value = config_json_serializer.Deserialize(&error_code, &error_message);
    if (value && value->IsType(base::Value::TYPE_DICTIONARY)) {
      config_json_.reset(static_cast<base::DictionaryValue*>(value.release()));
    }
#ifndef OS_ANDROID
    LOG(INFO) << "Load config file. File = " << base::WideToUTF8(base::SysNativeMBToWide(config_json_path))
              << ", err_code = " << error_code << ", err_msg = " << error_message;
#else
    LOG(INFO) << "Load config file. File = " << config_json_path
              << ", err_code = " << error_code << ", err_msg = " << error_message;
    
#endif
  }
}
