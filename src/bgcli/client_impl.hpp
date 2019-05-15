/***************************************************************************
**
** Copyright (c) 2019 UCEX
**
**    Author        : bd@biger.in
**    File          : client_impl.hpp
**    Date          : 2019/05/05 20:07:25
**    Description   :
**
***************************************************************************/
#ifndef BIGER_CLIENT_IMPL_HPP
#define BIGER_CLIENT_IMPL_HPP

#include <string>

#include <boost/beast/http.hpp>
#include <nlohmann/json.hpp>

#include "common/client_interface.hpp"

namespace biger
{
template <class ClientImpl>
using WebsocketClientInterface = common::WebsocketClientInterface<ClientImpl>;

class ClientImpl : public WebsocketClientInterface<ClientImpl>
{
 public:
  using Ptr   = std::add_pointer_t<ClientImpl>;
  using WSMgr = WebsocketClientInterface<ClientImpl>;

  ClientImpl(boost::asio::io_context& ioc, const nlohmann::json& j)
    : WSMgr{ioc, j}, ioc_{ioc}
  {
  }

  void Start()
  {
    try
    {
      WaitAvailable();
    }
    catch (const std::exception& e)
    {
      BOOST_LOG_SEV(ucex_lg, error)
          << "failed to start bgcli service, reason: " << e.what();
      Stop();
      ioc_.stop();
      throw;
    }
  }

  void WaitAvailable() const
  {
    // wait till match engine is ready
    while (!stopped_ && !WSMgr::IsAvailable())
    {
      BOOST_LOG(ucex_lg)
          << "waiting for websocket client interface to be ready";
      std::this_thread::sleep_for(std::chrono::milliseconds{100});
    }
  }

  // void Write(const std::string& msg) { WSMgr::Write(msg); }
  // std::string Symbol() { return WSMgr::Symbol(); }

  void Stop()
  {
    stopped_ = true;
    WSMgr::Stop();
  }

  void Subscribe(const std::string& symbol);
  void OnMessage(const std::string& msg);

 private:
  boost::asio::io_context& ioc_;
  bool stopped_ = false;
};

} // namespace biger
#endif
