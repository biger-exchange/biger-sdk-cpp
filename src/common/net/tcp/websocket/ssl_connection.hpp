/****************************************************************************
**
**  Copyright (c) 2018 Du Yang
**
**    Author        : bd@biger.in
**    File          : ssl_connection.hpp
**    Date          : 2018-11-22 16:06:35
**    Description   :
**
****************************************************************************/

#ifndef BIGER_COMMON_NET_TCP_WEBSOCKET_SSL_CONNECTION_HPP
#define BIGER_COMMON_NET_TCP_WEBSOCKET_SSL_CONNECTION_HPP

#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/websocket/ssl.hpp>

#include "common/net/tcp/websocket/connection.hpp"

namespace biger::common::net::tcp::websocket
{
class SSLConnection
  : public Connection<boost::beast::websocket::stream<
        boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>>
{
 public:
  using Ptr    = std::shared_ptr<SSLConnection>;
  using Buffer = Connection::Buffer;

  template <class Context, class... Args>
  SSLConnection(
      boost::asio::ip::tcp::socket&& socket, const std::string& url,
      Context& context, const Args&... args)
    : Connection{std::move(socket), url, context}
  {
  }

  template <class Context, class... Args>
  SSLConnection(
      boost::asio::ip::tcp::socket&& socket, Context& context,
      const Args&... args)
    : Connection{std::move(socket), context}
  {
  }

  template <class... Args>
  inline void Connect(Args&&... args)
  {
    boost::asio::async_connect(
        Connection::Socket().next_layer().next_layer(),
        std::forward<Args>(args)...);
  }

  template <class HandShakeType, class... Args>
  inline void SSLHandShake(HandShakeType type, Args&&... args)
  {
    Connection::Socket().next_layer().async_handshake(
        type, std::forward<Args>(args)...);
  }

  template <class HandShakeType, class... Args>
  inline void SSLHandShake(
      HandShakeType type, boost::asio::yield_context yield,
      boost::system::error_code& ec)
  {
    Connection::Socket().next_layer().async_handshake(type, yield[ec]);
  }
};
} // namespace biger::common::net::tcp::websocket

#endif
