/****************************************************************************
**
**  Copyright (c) 2018 UCEX
**
**    Author        : bd@biger.in
**    File          : lock_policy.hpp
**    Date          : 2018/06/22 10:32:00
**    Description   : Lock Policy
**
****************************************************************************/

#ifndef BIGER_COMMON_DETAIL_LOCK_POLICY_HPP
#define BIGER_COMMON_DETAIL_LOCK_POLICY_HPP

#include <mutex>
#include <shared_mutex>

namespace biger::common::detail
{
struct NoLock
{
  template <class T>
  struct NoGuard
  {
    NoGuard(const T&)
    {
    }
  };
  struct NoMutex
  {
  };
  using MutexType   = NoMutex;
  using Guard       = NoGuard<MutexType>;
  using SharedGuard = NoGuard<MutexType>;
};

struct EnforceLock
{
  using MutexType   = std::mutex;
  using Guard       = std::lock_guard<MutexType>;
  using SharedGuard = std::lock_guard<MutexType>;
};

struct EnforceSharedLock
{
  using MutexType   = std::shared_mutex;
  using Guard       = std::lock_guard<MutexType>;
  using SharedGuard = std::shared_lock<MutexType>;
};

} // namespace biger::common::detail

#endif
