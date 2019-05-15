/****************************************************************************
**
**  Copyright (c) 2018 Du Yang
**
**    Author        : bd@biger.in
**    File          : message.hpp
**    Date          : 2018-11-15 12:09:55
**    Description   :
**
****************************************************************************/

#ifndef BIGER_COMMONE_NET_TCP_WEBSOCKET_MESSAGE_HPP
#define BIGER_COMMONE_NET_TCP_WEBSOCKET_MESSAGE_HPP

#include <string>

#include "common/net/tcp/message.hpp"

namespace biger::common::net::tcp::websocket
{
template <class T1, class T2, class T3, class T4>
struct MessageImpl
{
  using Request     = T1;
  using RequestPtr  = T2;
  using Response    = T3;
  using ResponsePtr = T4;

  template <class... Args>
  static inline void Fix(Args&&...)
  {
  }

  template <class... Args>
  static inline void SetLimits(Args&&...)
  {
  }

  template <class T>
  static inline auto Validate(const T& message)
  {
    if (!message.empty())
    {
      return true;
    }
    return false;
  }

  template <class T>
  static inline decltype(auto) Read(T&& message)
  {
    return std::forward<T>(message);
  }
};

using Message = MessageImpl<
    net::tcp::Message, typename net::tcp::Message::Ptr, std::string,
    std::shared_ptr<std::string>>;

} // namespace biger::common::net::tcp::websocket

#endif
