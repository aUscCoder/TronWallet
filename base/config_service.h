#pragma once

#include <memory>
#include <chrono>
#include "values.h"

namespace base {

class ConfigService {
 public:
  static ConfigService* GetInstance();
  static bool OverrideConfig(const std::string& config_string);
  ConfigService(const std::string& config_path, bool is_path = true);
  virtual ~ConfigService() {}
  
  bool GetBoolean(const std::string& path,
                  bool default_value) const;
  int GetInteger(const std::string& path,
                 int default_value) const;
  double GetDouble(const std::string& path,
                   double default_value) const;
  std::string GetString(const std::string& path,
                        const std::string& default_value) const;

  std::vector<int> GetIntList(const std::string& path,
                              int default_value) const;

  bool GetIntList(const std::string& path, std::vector<int>& int_list) const;

  std::vector<double> GetDoubleList(const std::string& path,
                                    double default_value) const;

  bool GetDoubleList(const std::string& path, std::vector<double>& double_list) const;

  std::vector<std::string> GetStringArray(const std::string& path,
                                          const std::string& default_value) const;

  // default = now 
  static std::chrono::system_clock::time_point StrToTimePoint(const std::string& timeString);

 protected:
  ConfigService() { new (this)ConfigService("config.json"); }

 public:
  std::unique_ptr<base::DictionaryValue> config_json_;
};

}