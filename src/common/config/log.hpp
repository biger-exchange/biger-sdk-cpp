/***************************************************************************
**
** Copyright (c) 2018 UCEX
**
**    Author        : bd@biger.in
**    File          : log.hpp
**    Date          : 2018/06/14 11:25:56
**    Description   :
**
***************************************************************************/

#ifndef BIGER_COMMON_CONFIG_LOG_HPP
#define BIGER_COMMON_CONFIG_LOG_HPP

#include <exception>
#include <set>

#include <boost/algorithm/string.hpp>
#include <nlohmann/json.hpp>

#include "common/config/utils.hpp"

namespace biger::common::config
{
struct Log
{
  Log(const nlohmann::json& j)
  {
    Parse(j);
  }

  void Parse(const nlohmann::json& j)
  {
    try
    {
      dir       = j.at("log").at("dir");
      log_level = j.at("log").value("log_level", "debug");
      max_size =
          j.at("log").value("max_size", (uint64_t)128 * 1024 * 1024 * 1024);
      rotation_size =
          j.at("log").value("rotation_size", (uint64_t)4 * 1024 * 1024 * 1024);
      min_free_space =
          j.at("log").value("min_free_space", (uint64_t)100 * 1024 * 1024);
      max_log_files = j.at("log").value("max_log_files", (uint64_t)1024);
      flush_flag    = j.at("log").value("flush_flag", false);
    }
    catch (const std::exception& e)
    {
      const std::string error_info =
          std::string("failed to parse log configuration, reason: ") + e.what();
      throw std::invalid_argument{error_info};
    }
  }

  inline void Validate() const
  {
    static const std::set<std::string> levels = {"debug", "info", "warning",
                                                 "error", "fatal"};
    if (dir.empty())
    {
      throw std::invalid_argument{"invalid log dir: " + dir};
    }

    if (levels.find(boost::algorithm::to_lower_copy(log_level)) == levels.end())
    {
      throw std::invalid_argument{
          "invalid log level: " + log_level +
          ", reason: possible log levels: debug, info, warning, error, fatal"};
    }

    if (max_size < 10 * 1024 * 1024)
    {
      throw std::invalid_argument{
          "invalid max size: " + std::to_string(max_size) +
          ", reason: max size should >= 10M"};
    }

    if (rotation_size < 10 * 1024 * 1024)
    {
      throw std::invalid_argument{
          "invalid rotation size: " + std::to_string(rotation_size) +
          ", reason: rotation size should >= 10M"};
    }

    if (min_free_space < 10 * 1024 * 1024)
    {
      throw std::invalid_argument{
          "invalid min free space: " + std::to_string(min_free_space) +
          ", reason: min free space should >= 10M"};
    }

    if (max_log_files < 1)
    {
      throw std::invalid_argument{
          "invalid max log files: " + std::to_string(max_log_files) +
          ", reason: max log files should >= 1"};
    }
  }

  friend std::ostream& operator<<(std::ostream& os, const Log& conf)
  {
    os << conf.ToString();
    return os;
  }

  std::string ToString(uint32_t indent_chars = 0) const
  {
    std::ostringstream oss;
    PutHeader(oss, indent_chars, "log config");
    PutLine(oss, indent_chars, "directory", dir);
    PutLine(oss, indent_chars, "level", log_level);
    PutLine(oss, indent_chars, "max_size", max_size);
    PutLine(oss, indent_chars, "rotation_size", rotation_size);
    PutLine(oss, indent_chars, "min_free_space", min_free_space);
    PutLine(oss, indent_chars, "max_log_files", max_log_files);
    PutLine(oss, indent_chars, "flush_flag", flush_flag);
    return oss.str();
  }

  std::string dir;
  std::string log_level;
  uint64_t max_size;
  uint64_t rotation_size;
  uint64_t min_free_space;
  uint64_t max_log_files;
  bool flush_flag;
};

} // namespace biger::common::config

#endif // BIGER_COMMON_CONFIG_LOG_HPP
