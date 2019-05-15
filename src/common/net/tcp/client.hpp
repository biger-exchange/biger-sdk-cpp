/***************************************************************************
**
** Copyright (c) 2018 UCEX
**
**    Author        : bd@biger.in
**    File          : client.hpp
**    Date          : 2018/11/13 20:09:57
**    Description   : websocket/http client
**
***************************************************************************/

#ifndef BIGER_COMMON_NET_TCP_CLIENT_H
#define BIGER_COMMON_NET_TCP_CLIENT_H

#include <atomic>
#include <memory>
#include <queue>

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>

#include "common/config/websocket_client.hpp"
#include "common/detail/no_shared_from_this.hpp"
#include "common/log.hpp"
#include "common/net/address_book.hpp"
#include "common/net/clean_on_reconnect.hpp"
#include "common/net/session_queue.hpp"
#include "common/net/ssl/context/client.hpp"
#include "common/net/tcp/client_helper.hpp"
#include "common/net/tcp/session_state.hpp"
#include "common/net/utils.hpp"

namespace biger::common::net::tcp
{
template <
    class Parser, class Helper,
    class SSLContext                    = ssl::context::ClientVerifyNone,
    class BufferCleanPolicy             = net::CleanOnReconnect,
    template <class> class SharedPolicy = detail::NoSharedFromThis>
class Client
  : public SSLContext,
    public SessionState,
    public AddressBook,
    public SharedPolicy<
        Client<Parser, Helper, SSLContext, BufferCleanPolicy, SharedPolicy>>,
    public SessionQueue<
        Client<Parser, Helper, SSLContext, BufferCleanPolicy, SharedPolicy>,
        typename Helper::Message::Request, typename Helper::Message::RequestPtr>
{
 public:
  using Self =
      Client<Parser, Helper, SSLContext, BufferCleanPolicy, SharedPolicy>;
  using SharedFromThis = SharedPolicy<Self>;
  using Config         = typename Helper::Config;
  using Connection     = typename Helper::Connection;
  using ConnectionPtr  = typename Connection::Ptr;
  using Message        = typename Helper::Message;
  using Request        = typename Message::Request;
  using RequestPtr     = typename Message::RequestPtr;
  using Response       = typename Message::Response;
  using MessageQueue   = SessionQueue<Self, Request, RequestPtr>;
  using Status         = typename SessionState::Status;
  using context        = boost::asio::ssl::context;

  Client(
      boost::asio::io_context& ioc, const Config& conf,
      context::method method = context::sslv23_client, bool start = true)
    : SSLContext{conf, method},
      AddressBook{conf.address_book},
      MessageQueue{static_cast<uint32_t>(conf.queue_limit)},
      strand_{ioc},
      config_{conf},
      connection_{std::make_shared<Connection>(
          boost::asio::ip::tcp::socket{ioc}, AddressBook::Url(),
          SSLContext::Get(), AddressBook::UseSSL())},
      parser_{static_cast<Parser*>(this)},
      writing_{false},
      reconnect_{false},
      response_size_limit_{conf.response_size_limit},
      session_timeout_{static_cast<uint32_t>(conf.timeout)},
      reconnect_interval_{conf.reconnect_interval}
  {
    static_assert(
        std::is_base_of<Client, Parser>::value,
        "class Client should be derived by a message parser class");
    if (start)
    {
      Start();
    }
  }

  Client(const Client&) = delete;
  Client(Client&&)      = delete;
  Client& operator=(const Client&) = delete;
  Client& operator=(Client&&) = delete;

  void Start()
  {
    auto self(SharedFromThis::shared_from_this());
    boost::asio::spawn(strand_, [this, self](boost::asio::yield_context yield) {
      HandleRead(yield);
    });
    boost::asio::spawn(strand_, [this, self](boost::asio::yield_context yield) {
      Monitor(yield);
    });
  }

  inline auto UseSSL() const
  {
    return AddressBook::UseSSL();
  }

  inline const auto& RemoteHost() const
  {
    return AddressBook::Host();
  }

  inline auto RemotePort() const
  {
    return AddressBook::Port();
  }

  inline const auto& RemotePath() const
  {
    return AddressBook::Path();
  }

  inline const auto& RemoteUrl() const
  {
    return AddressBook::Url();
  }

  inline void UseNextAddress()
  {
    AddressBook::UseNext();
  }

  inline void SessionTimeout(int32_t timeout)
  {
    if (timeout >= 0)
    {
      session_timeout_ = timeout;
    }
  }

  inline auto SessionTimeout() const
  {
    return session_timeout_;
  }

  inline bool IsAvailable() const
  {
    return SessionState::Connected();
  }

  inline void UrlPath(const std::string& path)
  {
    strand_.post([this, path]() { AddressBook::Path(path); });
  }

  inline void Reconnect()
  {
    reconnect_ = true;
  }

  inline void Stop()
  {
    BOOST_LOG(ucex_lg) << "stop websocket client connection to " << RemoteUrl();
    SessionState::State(Status::Closed);
  }

  inline void Clear()
  {
    // clear read data buffer
    if (BufferCleanPolicy::clean())
    {
      // clear write buffer
      MessageQueue::Clear();
    }
  }

  template <class T>
  void Write(T&& message)
  {
    if (!SessionState::Connected())
    {
      return;
    }

    // Write data
    strand_.post([this, self = SharedFromThis::shared_from_this(),
                  message = std::forward<T>(message)]() {
      // Insert data to a queue
      MessageQueue::Enqueue(std::move(message));

      if (writing_)
      {
        return;
      }

      boost::asio::spawn(
          strand_, [this, self](boost::asio::yield_context yield) {
            HandleWrite(yield);
          });
    });
  }

 protected:
  void Close(boost::asio::yield_context yield)
  {
    BOOST_LOG(ucex_lg) << "close server connection, remote endpoint: "
                       << RemoteUrl();
    if (SessionState::Disconnecting())
    {
      return;
    }

    SessionState::State(Status::Disconnecting);
    Clear();

    // call parser on-close callback
    parser_->OnClose();

    auto connection = connection_;
    boost::system::error_code ec;
    connection->Close(yield, ec);

    // reset socket & context
    connection_ = std::make_shared<Connection>(
        boost::asio::ip::tcp::socket{strand_.get_io_service()},
        AddressBook::Url(), SSLContext::Get(), AddressBook::UseSSL());
    SessionState::State(Status::Disconnected);
    BOOST_LOG_SEV(ucex_lg, debug)
        << "connection closed, remote endpoint: " << RemoteUrl();
  }

  void HandleWrite(boost::asio::yield_context yield)
  {
    if (writing_)
    {
      return;
    }
    writing_ = true;

    boost::system::error_code ec;

    while (!SessionState::Closed())
    {
      auto connection = connection_;
      if (!connection->IsOpen() || !SessionState::Connected())
      {
        break;
      }

      auto message = MessageQueue::Dequeue();
      if (!message)
      {
        break;
      }

      // fix host/port/path information for http message
      Message::Fix(message, RemoteHost(), RemotePort(), RemotePath());
      connection->Write(message, yield[ec]);
      if (ec)
      {
        Fail(ec, "write");
        Close(yield);
        break;
      }
    }
    writing_ = false;
  }

  void Connect(boost::asio::yield_context yield)
  {
    using boost::asio::ip::tcp;
    using boost::asio::ssl::stream_base;

    boost::system::error_code ec;
    while (!SessionState::Closed())
    {
      if (!SessionState::Connectable())
      {
        return;
      }
      SessionState::State(Status::Connecting);

      auto connection = connection_;
      // clear data before trying new connection
      Clear();

      // assign next remote end point if available
      UseNextAddress();

      BOOST_LOG(ucex_lg) << "start to connect " << RemoteUrl();

      tcp::resolver resolver(strand_.get_io_service());

      tcp::resolver::query query(
          tcp::v4(), RemoteHost(), std::to_string(RemotePort()));

      tcp::resolver::iterator endpoint_iterator =
          resolver.async_resolve(query, yield[ec]);
      if (ec)
      {
        Fail(ec, "resolve");
        Close(yield);
        AsyncWait(strand_, yield, reconnect_interval_);
        continue;
      }

      connection->Connect(endpoint_iterator, yield[ec]);
      if (ec)
      {
        Fail(ec, "connect");
        Close(yield);
        AsyncWait(strand_, yield, reconnect_interval_);
        continue;
      }

      connection->SSLHandShake(stream_base::client, yield, ec);
      if (ec)
      {
        Fail(ec, "ssl handshake");
        Close(yield);
        AsyncWait(strand_, yield, reconnect_interval_);
        continue;
      }

      connection->HandShake(strand_, RemoteHost(), RemotePath(), yield, ec);
      if (ec)
      {
        Fail(ec, "handshake");
        Close(yield);
        AsyncWait(strand_, yield, reconnect_interval_);
        continue;
      }

      BOOST_LOG(ucex_lg) << "connected to " << RemoteUrl();
      // callback on connection established
      SessionState::State(Status::Connected);
      parser_->OnConnected();
      return;
    }
  }

  void HandleRead(boost::asio::yield_context yield)
  {
    boost::system::error_code ec;
    // Init connection
    Connect(yield);

    // Loop to read data from server
    while (!SessionState::Closed())
    {
      auto connection = connection_;
      if (!connection->IsOpen() || !SessionState::Connected())
      {
        AsyncWait(strand_, yield);
        if (SessionState::Disconnected())
        {
          Connect(yield);
        }
        continue;
      }

      Response response{};
      connection->Read(response, yield, ec);
      if (ec)
      {
        Fail(ec, "read data from");
        Close(yield);
        continue;
      }

      if (!connection->IsMessageDone())
      {
        BOOST_LOG_SEV(ucex_lg, warning)
            << "received oversized data from " << RemoteUrl();
        Close(yield);
        continue;
      }

      if (Message::Validate(response))
      {
        connection->SetLastUpdate();
        strand_.get_io_service().post([this, response = std::move(response)]() {
          parser_->Parse(RemoteUrl(), std::move(response));
        });
      }
    }
  }

  void Monitor(boost::asio::yield_context yield)
  {
    if (0 == session_timeout_)
    {
      return;
    }

    while (!SessionState::Closed())
    {
      AsyncWait(strand_, yield, std::chrono::seconds{1});

      // Close session if no data update before session timed out
      auto connection = connection_;
      auto now        = std::chrono::system_clock::now();
      auto time_diff  = std::chrono::duration_cast<std::chrono::seconds>(
                           now - connection->LastUpdate())
                           .count();
      // useful for websocket clients only
      //  - send ping request for websocket clients
      if (SessionState::Connected())
      {
        connection->Monitor(yield);
      }

      if (reconnect_ || SessionState::Closed() ||
          (connection->IsOpen() && time_diff > session_timeout_))
      {
        if (time_diff > session_timeout_)
        {
          BOOST_LOG_SEV(ucex_lg, error)
              << "connection timeout after " << session_timeout_
              << "s, remote endpoint: " << RemoteUrl();
        }
        // reset reconnect status
        reconnect_ = false;
        Close(yield);
      }
    }
  }

  // Report a failure
  inline void Fail(const boost::system::error_code& ec, std::string_view what)
  {
    BOOST_LOG_SEV(ucex_lg, error) << "failed to " << what << " " << RemoteUrl()
                                  << ", error msg=" << ec.message();
  }

 protected:
  virtual ~Client()
  {
  }

 private:
  boost::asio::io_context::strand strand_;
  Config config_;
  ConnectionPtr connection_;
  Parser* parser_;
  bool writing_;
  bool reconnect_;
  int32_t response_size_limit_;
  uint32_t session_timeout_;
  std::chrono::duration<double> reconnect_interval_;
};
} // namespace biger::common::net::tcp

#endif // BIGER_COMMON_NET_TCP_CLIENT_H
