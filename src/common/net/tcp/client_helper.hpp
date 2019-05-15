/****************************************************************************
**
**  Copyright (c) 2018 Du Yang
**
**    Author        : bd@biger.in
**    File          : client_helper.hpp
**    Date          : 2018-11-15 11:35:01
**    Description   :
**
****************************************************************************/

#ifndef BIGER_COMMONE_NET_TCP_CLIENT_HELPER_HPP
#define BIGER_COMMONE_NET_TCP_CLIENT_HELPER_HPP

#include "common/config/websocket_client.hpp"
#include "common/net/tcp/websocket/message.hpp"

namespace biger::common::net::tcp
{
template <class C, class M, class T>
struct ClientHelper
{
  using Config     = C;
  using Message    = M;
  using Connection = T;
};

template <class T>
using WebsocketClientHelper =
    ClientHelper<config::WebsocketClient, websocket::Message, T>;
} // namespace biger::common::net::tcp

#endif
