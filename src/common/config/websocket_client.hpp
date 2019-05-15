/***************************************************************************
**
** Copyright (c) 2018 UCEX
**
**    Author        : bd@biger.in
**    File          : websocket_client.hpp
**    Date          : 2018/09/30 11:21:00
**    Description   : websocket client configuration
**
***************************************************************************/

#ifndef BIGER_COMMON_CONFIG_WEBSOCKET_CLIENT_HPP
#define BIGER_COMMON_CONFIG_WEBSOCKET_CLIENT_HPP

#include <exception>

#include <nlohmann/json.hpp>

#include "common/config/host_address_book.hpp"
#include "common/config/ssl_context.hpp"
#include "common/config/utils.hpp"

namespace biger::common::config
{
struct WebsocketClient : public HostAddressBook, public SSLContext
{
  WebsocketClient(
      const nlohmann::json& j, const std::string& type = std::string{})
    : HostAddressBook{j.at("addr"), type}, SSLContext{j}
  {
    Parse(j, type);
    Validate();
  }

  void Parse(const nlohmann::json& j, const std::string& type)
  {
    try
    {
      ssl                 = j.value("ssl", false);
      timeout             = j.value("timeout", 30);
      reconnect_interval  = j.value("reconnect_interval", 1.0);
      queue_limit         = j.value("queue_limit", 0);
      response_size_limit = j.value("response_size_limit", 0);
    }
    catch (const std::exception& e)
    {
      throw std::invalid_argument{type +
                                  " websocket config load error: " + e.what()};
    }
  }

  void Validate()
  {
    if (timeout <= 0)
    {
      throw std::invalid_argument{"invalid timeout: " +
                                  std::to_string(timeout)};
    }

    if (reconnect_interval <= 0)
    {
      throw std::invalid_argument{"invalid reconnect interval: " +
                                  std::to_string(reconnect_interval)};
    }

    if (response_size_limit < 0)
    {
      throw std::invalid_argument{"invalid response size limit: " +
                                  std::to_string(response_size_limit)};
    }

    if (queue_limit < 0 || queue_limit > 1000)
    {
      throw std::invalid_argument{
          "invalid queue limit: " + std::to_string(queue_limit) +
          ", max queue size allowed: 1000"};
    }
  }

  std::string ToString(uint32_t indent_chars = 0) const
  {
    std::ostringstream oss;

    PutHeader(oss, indent_chars, "websocket client config");
    PutLine(oss, indent_chars, "timeout", timeout);
    PutLine(oss, indent_chars, "reconnect_interval", reconnect_interval);
    PutLine(oss, indent_chars, "queue_limit", queue_limit);
    PutLine(oss, indent_chars, "response_size_limit", response_size_limit);
    Put(oss, HostAddressBook::ToString(indent_chars + 1));
    return oss.str();
  }

  bool ssl                    = false;
  int32_t timeout             = 30;
  double reconnect_interval   = 1;
  int32_t queue_limit         = 0;
  int32_t response_size_limit = 0;
};

} // namespace biger::common::config

#endif
