/***************************************************************************
**
** Copyright (c) 2018 UCEX
**
**    Author        : bd@biger.in
**    File          : biger_heartbeat.hpp
**    Date          : 2018/10/10 17:32:17
**    Description   :
**
***************************************************************************/

#ifndef BIGER_COMMON_MESSAGE_BIGER_HEARTBEAT_HPP
#define BIGER_COMMON_MESSAGE_BIGER_HEARTBEAT_HPP

#include <nlohmann/json.hpp>

#include "common/chrono/time.hpp"

namespace biger::common::message
{
struct BigerHeartbeat
{
  BigerHeartbeat()
  {
  }

  //{"method": "server.ping", "params": [], "id": 1516681178}
  inline auto ToJson() const
  {
    return nlohmann::json{{"method", "server.ping"},
                          {"params", nlohmann::json::array()},
                          {"id", chrono::Time::Now<std::chrono::seconds>()}};
  }

  inline auto ToString() const
  {
    return ToJson().dump();
  }

  friend std::ostream& operator<<(
      std::ostream& os, const BigerHeartbeat& heartbeat)
  {
    os << heartbeat.ToString();
    return os;
  }
};
} // namespace biger::common::message

#endif
