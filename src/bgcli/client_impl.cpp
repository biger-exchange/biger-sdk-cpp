/***************************************************************************
**
** Copyright (c) 2019 UCEX
**
**    Author        : bd@biger.in
**    File          : client_impl.cpp
**    Date          : 2019/05/05 20:07:25
**    Description   :
**
***************************************************************************/
#include "client_impl.hpp"

namespace biger
{
void ClientImpl::Subscribe(const std::string& symbol)
{
  // depth.subscribe
  Write(
      R"({"method": "depth.subscribe", "params": [")" + symbol +
      R"(", 10, "0"], "id": 1000001})");
  // price.subscribe
  Write(
      R"({"method": "price.subscribe", "params": [")" + symbol +
      R"("], "id": 1000002})");
  // kline.subscribe
  Write(
      R"({"method": "kline.subscribe", "params": [")" + symbol +
      R"(", 900], "id": 1000003})");
}

void ClientImpl::OnMessage(const std::string& msg)
{
  if (stopped_)
  {
    return;
  }

  BOOST_LOG_SEV(ucex_lg, debug) << "processing message ..."
                                << ", msg:" << msg;
}

} // namespace biger
