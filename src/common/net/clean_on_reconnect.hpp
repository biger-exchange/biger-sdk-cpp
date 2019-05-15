/****************************************************************************
**
**  Copyright (c) 2018 UCEX
**
**    Author        : bd@biger.in
**    File          : clean_on_reconnect.hpp
**    Date          : 2018/06/14 15:05:00
**    Description   : Clean on reconnect
**
****************************************************************************/

#ifndef BIGER_COMMON_NET_CLEAN_ON_RECONNECT_HPP
#define BIGER_COMMON_NET_CLEAN_ON_RECONNECT_HPP

namespace biger::common::net
{
struct CleanOnReconnect
{
  static constexpr inline bool clean()
  {
    return true;
  }
};

struct NoCleanOnReconnect
{
  static constexpr inline bool clean()
  {
    return false;
  }
};

} // namespace biger::common::net

#endif
