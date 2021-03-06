/***************************************************************************
**
** Copyright (c) 2018 UCEX
**
**    Author        : bd@biger.in
**    File          : host_book.hpp
**    Date          : 2018/06/26 19:03:00
**    Description   :
**
***************************************************************************/

#ifndef BIGER_COMMON_CONFIG_HOST_ADDRESS_BOOK_HPP
#define BIGER_COMMON_CONFIG_HOST_ADDRESS_BOOK_HPP

#include <exception>
#include <vector>

#include "common/config/host_address.hpp"
#include "common/config/utils.hpp"

namespace biger::common::config
{
struct HostAddressBook
{
  using HostAddress    = config::HostAddress;
  using HostAddressPtr = typename config::HostAddress::Ptr;

  HostAddressBook()
  {
  }

  HostAddressBook(const HostAddressBook&) = default;
  HostAddressBook(HostAddressBook&&)      = default;

  HostAddressBook(const HostAddress& addr)
  {
    address_book.push_back(addr);
  }

  HostAddressBook(const typename HostAddress::Ptr& addr)
  {
    address_book.push_back(*addr);
  }

  HostAddressBook(
      const nlohmann::json& j, const std::string& type = std::string())
  {
    Parse(j, type);
  }

  void Parse(const nlohmann::json& j, const std::string& type)
  {
    if (!j.is_array())
    {
      throw std::invalid_argument{"host address book should be an array"};
    }

    for (const auto& addr : j)
    {
      try
      {
        address_book.emplace_back(addr.get<std::string>());
      }
      catch (const std::exception& e)
      {
        throw std::invalid_argument{
            type + " invalid address: " + addr.get<std::string>() +
            ", reason: " + e.what()};
      }
    }
  }

  std::string ToString(uint32_t indent_chars = 0) const
  {
    std::ostringstream oss;
    PutHeader(oss, indent_chars, "address book");
    for (const auto& address : address_book)
    {
      Put(oss, address.ToString(indent_chars + 1));
    }
    return oss.str();
  }

  std::vector<HostAddress> address_book;
};

} // namespace biger::common::config

#endif // BIGER_COMMON_CONFIG_HOST_ADDRESS_HPP
