/***************************************************************************
**
** Copyright (c) 2018 UCEX
**
**    Author        : bd@biger.in
**    File          : rest_api.cpp
**    Date          : 2018/09/29 11:54:15
**    Description   :
**
***************************************************************************/
#include <thread>

#include <boost/preprocessor/stringize.hpp>
#include <nlohmann/json.hpp>

#include "client_impl.hpp"
#include "common/biger_rest_api.h"
#include "common/config/config.hpp"
#include "common/config/log.hpp"
#include "common/log.hpp"
#include "common/net/utils.hpp"

static constexpr auto version = BOOST_PP_STRINGIZE(REVISION);

int main(int argc, char** argv)
{
  boost::asio::io_context ioc;

  try
  {
    using namespace biger;
    if (argc < 2)
    {
      std::cerr << "Error command"
                << ", e.g ./rest_api cfg/rest_api.cfg" << std::endl;
      return 1;
    }
    common::config::Config conf{argv[1]};
    // init log
    common::config::Log logconf{conf.data};
    auto& logger = common::Log::Get(logconf, argv[0]);

    ClientImpl rest_api{ioc, conf.data};

    // handle process signals
    boost::asio::signal_set sigset{ioc};
    common::net::SetSigAction(sigset, [&]() {
      rest_api.Stop();
      logger.Flush();
      ioc.stop();
      BigerRestApi::Stop();
    });

    // init curl api
    BigerRestApi::Init();

    std::thread t1{boost::bind(&boost::asio::io_context::run, &ioc)};
    // wait and start service
    rest_api.Start();

    t1.join();
  }
  catch (std::exception& e)
  {
    std::cerr << "Error: failed to start rest_api service, reason: " << e.what()
              << std::endl;
    return 1;
  }

  return 0;
}
/* vim: set ts=4 sw=4 sts=4 tw=100 */
