/****************************************************************************
**
**  Copyright (c) 2019 Du Yang
**
**    Author        : bd@biger.in
**    File          : utils.hpp
**    Date          : 2019-04-17 18:27:34
**    Description   :
**
****************************************************************************/

#ifndef BIGER_COMMON_NET_UTILS_HPP
#define BIGER_COMMON_NET_UTILS_HPP

#include <iostream>

#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/steady_timer.hpp>

namespace biger::common::net
{
template <class DurationType = std::chrono::milliseconds>
static void AsyncWait(
    boost::asio::io_context& ioc, boost::asio::yield_context yield,
    const DurationType& duration = std::chrono::milliseconds{200})
{
  boost::system::error_code ec;
  boost::asio::steady_timer t(
      ioc, std::chrono::duration_cast<std::chrono::milliseconds>(duration));
  t.async_wait(yield[ec]);
}

template <class DurationType = std::chrono::milliseconds>
static void AsyncWait(
    boost::asio::io_context::strand& strand, boost::asio::yield_context yield,
    const DurationType& duration = std::chrono::milliseconds{200})
{
  boost::system::error_code ec;
  boost::asio::steady_timer t(
      strand.get_io_service(),
      std::chrono::duration_cast<std::chrono::milliseconds>(duration));
  t.async_wait(yield[ec]);
}

template <class F>
static bool SetSigAction(boost::asio::signal_set& sigset, const F& f)
{
  sigset.add(SIGTERM);
  sigset.add(SIGINT);
  sigset.add(SIGPIPE);
  sigset.add(SIGUSR1);
  sigset.add(SIGALRM);
  sigset.add(SIGHUP);

  sigset.async_wait(
      [&sigset, f](const boost::system::error_code& ec, int signal) {
        BOOST_LOG(ucex_lg) << "received process signal: " << signal;
        switch (signal)
        {
        case SIGTERM:
        case SIGINT:
          BOOST_LOG(ucex_lg) << "gracefully shutdown, exiting....";
          std::cout << "gracefully shutdown, exiting...." << std::endl;
          f();
          break;
        case SIGPIPE:
        case SIGUSR1:
        case SIGALRM:
        case SIGHUP:
        default:
          SetSigAction(sigset, f);
          break;
        }
      });

  return true;
}
} // namespace biger::common::net

#endif
