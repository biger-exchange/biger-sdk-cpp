/****************************************************************************
**
**  Copyright (c) 2018 Du Yang
**
**    Author        : bd@biger.in
**    File          : expiry_policy.hpp
**    Date          : 2018-12-22 12:14:49
**    Description   :
**
****************************************************************************/

#ifndef BIGER_COMMON_CHRONO_EXPIRY_POLICY_HPP
#define BIGER_COMMON_CHRONO_EXPIRY_POLICY_HPP

namespace biger::common::chrono
{
template <class IntervalType, class TimePoint>
class ExpireAtPoint
{
 public:
  template <class TP, class T>
  ExpireAtPoint(TP&& expiry_time, T&& interval)
    : expiry_time_{std::forward<TP>(expiry_time)},
      interval_{std::forward<T>(interval)}
  {
  }

  template <class TP>
  inline void ExpireAt(TP&& expiry_time)
  {
    expiry_time_ = std::forward<TP>(expiry_time);
  }

  template <class T>
  inline void Interval(T&& interval)
  {
    interval_ = std::forward<T>(interval);
  }

 protected:
  template <class T>
  inline void SetExpireTime(T& timer)
  {
    while (true)
    {
      try
      {
        timer.expires_at(expiry_time_);
        expiry_time_ += interval_;
        break;
      }
      catch (const std::exception& e)
      {
      }
    }
  }

 private:
  TimePoint expiry_time_;
  IntervalType interval_;
};

template <class IntervalType, class TimePoint>
class ExpireAfter
{
 public:
  template <class TP, class T>
  ExpireAfter(const TP&, T&& interval) : interval_{std::forward<T>(interval)}
  {
  }

  template <class T>
  inline void Interval(T&& interval)
  {
    interval_ = std::forward<T>(interval);
  }

 protected:
  template <class T>
  inline void SetExpireTime(T& timer)
  {
    while (true)
    {
      try
      {
        timer.expires_after(
            std::chrono::duration_cast<std::chrono::microseconds>(interval_));
        break;
      }
      catch (const std::exception& e)
      {
      }
    }
  }

 private:
  IntervalType interval_;
};

} // namespace biger::common::chrono

#endif
