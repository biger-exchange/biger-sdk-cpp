/****************************************************************************
**
**  Copyright (c) 2018 Du Yang
**
**    Author        : bd@biger.in
**    File          : session_state.hpp
**    Date          : 2018-11-24 20:23:22
**    Description   :
**
****************************************************************************/

#ifndef BIGER_COMMON_NET_TCP_SESSION_STATE_HPP
#define BIGER_COMMON_NET_TCP_SESSION_STATE_HPP

namespace biger::common::net::tcp
{
class SessionState
{
 public:
  enum class Status : int32_t
  {
    Initialized,
    Connecting,
    Connected,
    Disconnecting,
    Disconnected,
    Closed
  };

  SessionState() : state_{Status::Initialized}
  {
  }

  inline auto State() const
  {
    return state_;
  }

  inline void State(Status s)
  {
    if (state_ == Status::Closed)
    {
      return;
    }
    state_ = s;
  }

  inline auto Connected() const
  {
    return Status::Connected == state_;
  }

  inline auto Connecting() const
  {
    return Status::Connecting == state_;
  }

  inline auto Disconnected() const
  {
    return Status::Disconnected == state_;
  }

  inline auto Disconnecting() const
  {
    return Status::Disconnecting == state_;
  }

  inline auto Connectable() const
  {
    return Status::Disconnected == state_ || Status::Initialized == state_;
  }

  inline auto Closed() const
  {
    return Status::Closed == state_;
  }

 private:
  Status state_;
};
} // namespace biger::common::net::tcp

#endif
