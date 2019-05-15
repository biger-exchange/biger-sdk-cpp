/***************************************************************************
**
** Copyright (c) 2018 UCEX
**
**    Author        : bd@biger.in
**    File          : config.hpp
**    Date          : 2018/06/13 16:15:44
**    Description   :
**
***************************************************************************/

#ifndef BIGER_COMMON_CONFIG_CONFIG_HPP
#define BIGER_COMMON_CONFIG_CONFIG_HPP

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <boost/filesystem.hpp>
#include <nlohmann/json.hpp>

namespace biger::common::config
{
struct Config
{
  Config(const std::string& config_path) : config_path{config_path}
  {
    PreCheck();
    Parse();
  }

  void Parse()
  {
    try
    {
      std::ifstream ifs{config_path};
      std::stringstream ssm;
      ssm << ifs.rdbuf();
      data = nlohmann::json::parse(ssm.str());
    }
    catch (const std::exception& e)
    {
      std::cout << "Error: failed to load json conf file, reason: " << e.what()
                << std::endl;
      throw std::invalid_argument{"config load error"};
    }
  }

  void PreCheck() const
  {
    if (!boost::filesystem::exists(config_path))
    {
      std::cout << "Error: config file not found: " << config_path << std::endl;
      throw std::invalid_argument{"config file not found"};
    }
  }

  inline auto ToString() const
  {
    return data.dump();
  }

  nlohmann::json data;
  const std::string config_path;
};

} // namespace biger::common::config

#endif // BIGER_COMMON_CONFIG_CONFIGURATION_HPP
