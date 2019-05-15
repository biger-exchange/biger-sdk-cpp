/***************************************************************************
**
** Copyright (c) 2018 UCEX
**
**    Author        : bd@biger.in
**    File          : client_interface.hpp
**    Date          : 2018/09/30 13:20:24
**    Description   :
**
***************************************************************************/

#ifndef BIGER_COMMON_CLIENT_INTERFACE_HPP
#define BIGER_COMMON_CLIENT_INTERFACE_HPP

#include <memory>

#include "common/config/client_interface.hpp"
#include "common/log.hpp"
#include "common/message/biger_heartbeat.hpp"
#include "common/net/tcp/client.hpp"
#include "common/net/tcp/websocket/ssl_connection.hpp"
#include "common/timer.hpp"

namespace biger::common
{
template <
    class Application, class NetHandler,
    template <class, class> class NetClientPolicy>
class ClientInterface
  : public NetClientPolicy<
        ClientInterface<Application, NetHandler, NetClientPolicy>, NetHandler>
{
 public:
  using Self = ClientInterface<Application, NetHandler, NetClientPolicy>;
  using Ptr  = std::shared_ptr<Self>;
  using ApplicationPtr = std::add_pointer_t<Application>;
  using NetClient      = NetClientPolicy<Self, NetHandler>;
  using Heartbeat      = message::BigerHeartbeat;
  using Config         = config::ClientInterface;

  ClientInterface(boost::asio::io_context& ioc, const Config& conf)
    : NetClient{ioc, conf},
      ioc_{ioc},
      application_{static_cast<ApplicationPtr>(this)},
      conf_{conf},
      heartbeat_timer_{ioc, conf.heartbeat_interval},
      enabled_{conf.enabled},
      symbol_{conf.symbol}
  {
    if (!enabled_)
    {
      return;
    }

    static_assert(
        std::is_base_of<ClientInterface, Application>::value,
        "class ClientInterface should be derived by an application "
        "class");
    BOOST_LOG(ucex_lg) << "client interface configuration:\n" << conf;
    StartTimers();
  }

  ClientInterface(
      ApplicationPtr application, boost::asio::io_context& ioc,
      const Config& conf)
    : NetClient{ioc, conf},
      ioc_{ioc},
      application_{application},
      conf_{conf},
      heartbeat_timer_{ioc, conf.heartbeat_interval},
      enabled_{conf.enabled},
      symbol_{conf.symbol}
  {
    if (!enabled_)
    {
      return;
    }

    BOOST_LOG(ucex_lg) << "client interface configuration:\n" << conf;
    StartTimers();
  }

  inline void StartTimers()
  {
    heartbeat_timer_.Start([this]() { this->Write(Heartbeat().ToString()); });
  }

  inline void Parse(std::string_view remote_url, const std::string& message)
  {
    try
    {
      const auto data = nlohmann::json::parse(message);
      if (data.count("result"))
      {
        const auto& result = data.at("result");
        if (result.is_string() && result == "pong")
        {
          // do nothing
          // BOOST_LOG_SEV(ucex_lg, debug)
          //    << "received heartbeat message: " << message
          //    << ", remote url: " << remote_url;
          return;
        }
        else if (result.count("status"))
        {
          if (result.at("status") == "success")
          {
            BOOST_LOG_SEV(ucex_lg, debug)
                << "subscribe success, message:" << message;
          }
          return;
        }
      }

      BOOST_LOG(ucex_lg) << "received message: " << message
                         << ", remote url: " << remote_url;
      application_->OnMessage(message);
    }
    catch (const std::exception& e)
    {
      BOOST_LOG_SEV(ucex_lg, error)
          << "invalid message: " << message << ", remote url: " << remote_url
          << ", reason: " << e.what();
    }
  }

  inline void OnClose()
  {
  }

  inline bool Enabled() const
  {
    return enabled_;
  }

  inline bool IsAvailable() const
  {
    return !enabled_ || (is_available_ && NetClient::IsAvailable());
  }

  inline void OnConnected()
  {
    is_available_ = true;
    ioc_.post([this]() { application_->Subscribe(symbol_); });
  }

  std::string Symbol()
  {
    return symbol_;
  }

  void Stop()
  {
    BOOST_LOG(ucex_lg) << "stop client interface";
    heartbeat_timer_.Stop();
    NetClient::Stop();
  }

 private:
  boost::asio::io_context& ioc_;
  ApplicationPtr application_;
  Config conf_;
  DurationTimer<std::chrono::duration<double>> heartbeat_timer_;
  bool enabled_      = true;
  bool is_available_ = false;
  std::string symbol_;
};

template <class Application>
using WebsocketClientInterface = ClientInterface<
    Application,
    net::tcp::WebsocketClientHelper<net::tcp::websocket::SSLConnection>,
    net::tcp::Client>;

} // namespace biger::common

#endif
