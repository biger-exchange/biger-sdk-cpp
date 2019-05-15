/***************************************************************************
**
** Copyright (c) 2018 UCEX
**
**    Author        : bd@biger.in
**    File          : ssl_context.hpp
**    Date          : 2019-02-28 18:11:12
**    Description   :
**
***************************************************************************/

#ifndef BIGER_COMMON_CONFIG_SSL_CONTEXT_HPP
#define BIGER_COMMON_CONFIG_SSL_CONTEXT_HPP

#include <exception>
#include <thread>
#include <vector>

#include <nlohmann/json.hpp>

#include "common/config/utils.hpp"

namespace biger::common::config
{
struct SSLContext
{
  int32_t ssl_type = 0;
  std::string password;
  bool rfc2818_verification = false;
  std::string hostname;
  std::string private_key;
  std::string rsa_private_key;
  std::string certificate;
  std::string certificate_chain;
  std::string tmp_dh;
  std::string certificate_authority;
  bool use_default_verify_path = false;
  std::vector<std::string> verify_paths;

  SSLContext(const SSLContext&) = default;
  SSLContext(SSLContext&&)      = default;

  SSLContext(const nlohmann::json& j)
  {
    Parse(j);
    Validate();
  }

  void Parse(const nlohmann::json& j)
  {
    try
    {
      ssl_type              = j.value("ssl_type", 0);
      password              = j.value("password", std::string{});
      rfc2818_verification  = j.value("rfc2818_verification", false);
      hostname              = j.value("hostname", std::string{});
      private_key           = GetKey(j, "private_key");
      rsa_private_key       = GetKey(j, "rsa_private_key");
      certificate           = GetKey(j, "certificate");
      certificate_chain     = GetKey(j, "certificate_chain");
      tmp_dh                = GetKey(j, "tmp_dh");
      certificate_authority = GetKey(j, "certificate_authority");

      use_default_verify_path = j.value("use_default_verify_path", false);
      verify_paths            = j.value("verify_paths", nlohmann::json::array())
                         .get<std::vector<std::string>>();
    }
    catch (const std::exception& e)
    {
      const std::string error_info =
          std::string{"failed to parse ssl context config, reason: "} +
          e.what();
      throw std::invalid_argument(error_info);
    }
  }

  static inline std::string GetKey(
      const nlohmann::json& j, const std::string& key)
  {
    if (!j.count(key))
    {
      return std::string{};
    }
    else if (j.at(key).is_array())
    {
      size_t size = 0u;
      for (const auto& item : j.at(key))
      {
        if (!item.is_null())
        {
          size += item.size();
        }
      }

      std::string data;
      data.reserve(size + 64);
      for (const auto& item : j.at(key))
      {
        if (!item.is_null())
        {
          data += item.get<std::string>() + "\n";
        }
      }
      return data;
    }
    else
    {
      return j.value(key, std::string{});
    }
  }

  void Validate() const
  {
    if (ssl_type < 0)
    {
      throw std::invalid_argument{"invalid ssl type: " +
                                  std::to_string(ssl_type)};
    }

    if (rfc2818_verification && hostname.empty())
    {
      throw std::invalid_argument{
          "hostname should set for rfc2818 verification"};
    }

    if (rfc2818_verification && 0 == ssl_type)
    {
      throw std::invalid_argument{
          "ssl type should be 1 for rfc2818 verification"};
    }
  }

  friend std::ostream& operator<<(std::ostream& os, const SSLContext& conf)
  {
    os << conf.ToString();
    return os;
  }

  std::string ToString(uint32_t indent_chars = 0) const
  {
    std::ostringstream oss;

    PutHeader(oss, indent_chars, "ssl context");
    PutLine(oss, indent_chars, "ssl type", ssl_type);
    PutLine(oss, indent_chars, "rfc2818_verification", rfc2818_verification);
    PutLine(oss, indent_chars, "hostname", hostname);
    PutLine(
        oss, indent_chars, "use_default_verify_path", use_default_verify_path);
    return oss.str();
  }
};

} // namespace biger::common::config

#endif
