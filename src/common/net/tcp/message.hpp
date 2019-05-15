/****************************************************************************
**
**  Copyright (c) 2018 UCEX
**
**    Author        : bd@biger.in
**    File          : tcp_message.hpp
**    Date          : 2018/07/06 14:18:00
**    Description   : TCP Message
**
****************************************************************************/

#ifndef BIGER_COMMON_NET_TCP_MESSAGE_HPP
#define BIGER_COMMON_NET_TCP_MESSAGE_HPP

#include <memory>

namespace biger::common::net::tcp
{
class Message
{
 public:
  using Ptr          = std::shared_ptr<Message>;
  Message()          = default;
  Message(Message&&) = default;

  template <class String>
  Message(String&& data) : id_{0}
  {
    // gcc bug 89793
    data_ = std::forward<String>(data);
  }

  template <class String>
  Message(uint32_t id, String&& data) : id_{id}
  {
    // gcc bug 89793
    data_ = std::forward<String>(data);
  }

  inline auto ID() const
  {
    return id_;
  }

  inline bool Empty() const
  {
    return data_.size() == 0;
  }

  inline const auto& Data() const&
  {
    return data_;
  }

  inline auto&& Data() const&&
  {
    return std::move(data_);
  }

  inline auto ToString() const
  {
    return "id=" + std::to_string(id_) + ", data=" + data_;
  }

  friend std::ostream& operator<<(std::ostream& os, const Message& m)
  {
    os << m.ToString();
    return os;
  }

  friend std::ostream& operator<<(std::ostream& os, const Ptr& m)
  {
    if (m)
    {
      os << m->ToString();
    }
    return os;
  }

 private:
  uint32_t id_ = 0;
  std::string data_;
};

template <
    class T1, class T2, class T3 = typename T1::Ptr,
    class T4 = typename T2::Ptr>
struct MessageImpl
{
  using Request     = T1;
  using Response    = T2;
  using RequestPtr  = T3;
  using ResponsePtr = T4;

  template <class T>
  static inline bool Validate(const T& message)
  {
    return message.Size() != 0;
  }

  template <class T>
  static inline auto Read(const T& parser)
  {
    return std::move(parser.Get());
  }

  template <class... Args>
  static inline void SetLimits(Args&&...)
  {
  }

  template <class... Args>
  static inline void Fix(Args&&...)
  {
  }
};

} // namespace biger::common::net::tcp

#endif
