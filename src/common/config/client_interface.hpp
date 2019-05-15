/****************************************************************************
**
**  Copyright (c) 2018 Du Yang
**
**    Author        : bd@biger.in
**    File          : client_interface.hpp
**    Date          : 2018-11-07 18:37:00
**    Description   :
**
****************************************************************************/

#ifndef BIGER_COMMON_CONFIG_CLIENT_INTERFACE_HPP
#define BIGER_COMMON_CONFIG_CLIENT_INTERFACE_HPP

#include <exception>

#include <nlohmann/json.hpp>

#include "common/config/websocket_client.hpp"

namespace biger::common::config
{
struct ClientInterface : public WebsocketClient
{
  ClientInterface(const nlohmann::json& j, const std::string& type = "biger")
    : WebsocketClient{j.at(type), type}, type{type}
  {
    Parse(j.at(type));
    Validate();
  }

  void Parse(const nlohmann::json& j)
  {
    enabled            = j.value("enabled", true);
    heartbeat_interval = j.value("heartbeat_interval", 1.0);
    symbol             = j.at("symbol");
  }

  void Validate() const
  {
    if (heartbeat_interval <= 0)
    {
      throw std::invalid_argument{"invalid " + type + " heartbeat interval: " +
                                  std::to_string(heartbeat_interval)};
    }
  }

  friend std::ostream& operator<<(std::ostream& os, const ClientInterface& conf)
  {
    os << conf.ToString();
    return os;
  }

  std::string ToString(uint32_t indent_chars = 0) const
  {
    std::ostringstream oss;
    PutHeader(oss, indent_chars, "client interface config");
    PutLine(oss, indent_chars, "enabled", enabled);
    Put(oss, WebsocketClient::ToString(indent_chars + 1));
    PutLine(oss, indent_chars, "heartbeat_interval", heartbeat_interval);
    PutLine(oss, indent_chars, "symbol", symbol);
    return oss.str();
  }

  const std::string type;
  bool enabled              = true;
  double heartbeat_interval = 1;
  std::string symbol;
};

} // namespace biger::common::config

#endif
