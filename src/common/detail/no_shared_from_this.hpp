/****************************************************************************
**
**  Copyright (c) 2018 UCEX
**
**    Author        : bd@biger.in
**    File          : no_shared_from_this.hpp
**    Date          : 2018/06/06 18:15:40
**    Description   : No shared from this
**
****************************************************************************/

#ifndef BIGER_COMMON_DETAIL_NO_SHARED_FROM_THIS_HPP
#define BIGER_COMMON_DETAIL_NO_SHARED_FROM_THIS_HPP

namespace biger::common::detail
{
template <class T>
struct NoSharedFromThis
{
  NoSharedFromThis() : ptr_{static_cast<T*>(this)}
  {
  }

  inline auto shared_from_this() const
  {
    return ptr_;
  }

  T* ptr_;
};
} // namespace biger::common::detail

#endif
