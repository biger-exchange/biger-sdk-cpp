/****************************************************************************
**
**  Copyright (c) 2018 UCEX
**
**    Author        : bd@biger.in
**    File          : session_queue.hpp
**    Date          : 2018/07/17 17:25:00
**    Description   : Session Queue
**
****************************************************************************/

#ifndef BIGER_COMMON_NET_SESSION_QUEUE_H
#define BIGER_COMMON_NET_SESSION_QUEUE_H

#include <queue>

#include "common/container/data_queue.hpp"
#include "common/log.hpp"

namespace biger::common::net
{
template <class Session, class Message, class MessagePtr>
class SessionQueue : public container::DataQueue<MessagePtr>
{
 public:
  using MessageQueue = container::DataQueue<MessagePtr>;

  SessionQueue(uint32_t limit)
    : MessageQueue{limit}, session_(static_cast<Session*>(this))
  {
    static_assert(
        std::is_base_of<SessionQueue, Session>::value,
        "SessionHelper should be inherited by Session");
  }

  SessionQueue(const SessionQueue&) = delete;
  SessionQueue& operator=(const SessionQueue&) = delete;

 protected:
  template <class T>
  inline void Enqueue(T&& message)
  {
    EnqueueImpl(message, Type<std::decay_t<T>>{});
  }

  inline bool CheckSize()
  {
    // clear queue if run out of limit
    if (MessageQueue::LimitExceeded())
    {
      BOOST_LOG_SEV(ucex_lg, warning)
          << "clear session write buffer, client: " << session_->RemoteUrl()
          << ", reason: data queue run over limit " << MessageQueue::Limit();
      MessageQueue::Clear();
      // Enqueue("Service Busy");
      return true;
    }
    return false;
  }

  template <class T>
  struct Type
  {
  };

 protected:
  virtual ~SessionQueue()
  {
  }

 private:
  template <class M, class T>
  inline void EnqueueImpl(M&& message, const Type<T>&)
  {
    MessageQueue::Enqueue(std::make_shared<Message>(std::forward<M>(message)));
  }

  template <class M>
  inline void EnqueueImpl(M&& message, const Type<MessagePtr>&)
  {
    MessageQueue::Enqueue(std::forward<M>(message));
  }

 private:
  Session* session_;
};
} // namespace biger::common::net

#endif
