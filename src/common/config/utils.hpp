/****************************************************************************
**
**  Copyright (c) 2019 Du Yang
**
**    Author        : bd@biger.in
**    File          : utils.hpp
**    Date          : 2019-04-09 15:09:45
**    Description   :
**
****************************************************************************/

#ifndef BIGER_COMMON_CONFIG_UTILS_HPP
#define BIGER_COMMON_CONFIG_UTILS_HPP

#include <sstream>
#include <string>

namespace biger::common::config
{
inline auto& PutHeader(
    std::ostringstream& oss, int32_t indent, const std::string& key)
{
  oss << std::string(indent, ' ') << key << ":\n";
  return oss;
}

template <class... Args>
inline auto& Put(std::ostringstream& oss, const Args&... args)
{
  (oss << ... << args);
  return oss;
}

template <class... Args>
inline auto& PutLine(
    std::ostringstream& oss, int32_t indent, const std::string& key,
    const Args&... values)
{
  oss << std::string(indent, ' ') << " - " << key << ": ";
  (oss << ... << values);
  oss << "\n";
  return oss;
}
} // namespace biger::common::config

#endif
