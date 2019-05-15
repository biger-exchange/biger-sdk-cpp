/****************************************************************************
**
**  Copyright (c) 2018 UCEX
**
**    Author        : bd@biger.in
**    File          : data_queue.hpp
**    Date          : 2018/07/19 14:00:00
**    Description   : Data Queue
**
****************************************************************************/

#ifndef BIGER_COMMON_CONTAINER_DATA_QUEUE_H
#define BIGER_COMMON_CONTAINER_DATA_QUEUE_H

#include <queue>

#include "common/detail/lock_policy.hpp"

namespace biger::common::container
{
template <
    class Data, template <class> class StorePolicy = std::queue,
    class LockPolicy = detail::NoLock>
class DataQueue
{
 public:
  using Mutex      = typename LockPolicy::MutexType;
  using LockGuard  = typename LockPolicy::Guard;
  using SharedLock = typename LockPolicy::SharedGuard;

  DataQueue(uint32_t limit) : limit_{limit}
  {
  }

  DataQueue& operator=(const DataQueue&) = delete;
  DataQueue& operator=(DataQueue&&) = delete;
  DataQueue(const DataQueue&)       = delete;
  DataQueue(DataQueue&&)            = delete;

  inline void Limit(int32_t limit)
  {
    if (limit >= 0)
    {
      limit_ = limit;
    }
  }

  inline auto Limit() const
  {
    return limit_;
  }

  auto Dequeue()
  {
    return DequeueImpl(Type<Data>{});
  }

  inline auto Count() const
  {
    SharedLock lock(mutex_);
    return queue_.size();
  }

  inline bool LimitExceeded() const
  {
    SharedLock lock(mutex_);
    if (0 != limit_ && queue_.size() > limit_)
    {
      return true;
    }
    return false;
  }

  inline void Clear()
  {
    LockGuard lock(mutex_);
    std::queue<Data> empty_msg_queue{};
    queue_.swap(empty_msg_queue);
  }

  template <class T>
  inline void Enqueue(T&& message)
  {
    LockGuard lock(mutex_);
    queue_.emplace(std::forward<T>(message));
  }

 protected:
  virtual ~DataQueue()
  {
  }

  template <class T>
  struct Type
  {
  };

  template <class T>
  inline Data DequeueImpl(const Type<T>&)
  {
    if (!queue_.empty())
    {
      LockGuard lock(mutex_);
      if (!queue_.empty())
      {
        auto msg = queue_.front();
        queue_.pop();
        return msg;
      }
    }
    return Data{};
  }

  template <class T>
  inline Data DequeueImpl(const Type<std::unique_ptr<T>>&)
  {
    if (!queue_.empty())
    {
      LockGuard lock(mutex_);
      if (!queue_.empty())
      {
        auto msg = std::move(queue_.front());
        queue_.pop();
        return msg;
      }
    }
    return Data{};
  }

 private:
  mutable Mutex mutex_;
  StorePolicy<Data> queue_;
  uint32_t limit_;
};
} // namespace biger::common::container

#endif
