/****************************************************************************
**
**  Copyright (c) 2018 Du Yang
**
**    Author        : bd@biger.in
**    File          : time.hpp
**    Date          : 2018-12-24 18:44:56
**    Description   :
**
****************************************************************************/

#ifndef BIGER_COMMON_CHRONO_TIME_HPP
#define BIGER_COMMON_CHRONO_TIME_HPP

#include <chrono>

#include <boost/date_time/posix_time/posix_time.hpp>

namespace biger::common::chrono
{
class Time
{
 public:
  template <class... T>
  static inline auto Now()
  {
    return NowImpl(Type<T...>{});
  }

 private:
  template <class... T>
  struct Type
  {
  };

  template <class... T>
  static inline auto NowImpl(const Type<std::chrono::duration<T...>>&)
  {
    return std::chrono::duration_cast<std::chrono::duration<T...>>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
  }

  template <class... T>
  static inline auto NowImpl(const Type<T...>&)
  {
    return NowImpl(Type<std::chrono::duration<T...>>{});
  }
};
} // namespace biger::common::chrono

#endif
