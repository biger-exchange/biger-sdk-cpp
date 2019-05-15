/****************************************************************************
**
**  Copyright (c) 2018 Du Yang
**
**    Author        : bd@biger.in
**    File          : base.hpp
**    Date          : 2019-02-28 15:04:00
**    Description   :
**
****************************************************************************/

#ifndef BIGER_COMMON_NET_SSL_CONTEXT_BASE_HPP
#define BIGER_COMMON_NET_SSL_CONTEXT_BASE_HPP

#include <exception>
#include <string>

#include <boost/asio/ssl.hpp>

namespace biger::common::net::ssl::context
{
class Base
{
 public:
  using VerifyMode = boost::asio::ssl::verify_mode;
  using Context    = boost::asio::ssl::context;
  using Options    = Context::options;
  using Method     = Context::method;

  template <class Config>
  Base(
      const Config& config, Method method,
      Options options = Context::default_workarounds | Context::no_sslv2 |
                        Context::no_sslv3,
      VerifyMode mode = boost::asio::ssl::verify_none)
    : rfc2818_verification_{config.rfc2818_verification},
      hostname_{config.hostname},
      use_default_verify_path_{config.use_default_verify_path},
      verify_paths_{config.verify_paths},
      password_{config.password},
      private_key_{config.private_key},
      rsa_private_key_{config.rsa_private_key},
      certificate_{config.certificate},
      certificate_chain_{config.certificate_chain},
      tmp_dh_{config.tmp_dh},
      certificate_authority_{config.certificate_authority},
      options_{options},
      verify_mode_{mode},
      context_{method}
  {
    try
    {
      Init();
    }
    catch (const std::exception& e)
    {
      throw std::runtime_error{
          std::string{"failed to init base ssl context: "} + e.what()};
    }
  }

  template <class Config, class Type>
  Base(const Config& config, const Type& type)
    : Base{config, type.method, type.options, type.mode}
  {
  }

  inline auto& Get()
  {
    return context_;
  }

 protected:
  inline const auto& CertificateAuthority() const
  {
    return certificate_authority_;
  }

  void Init()
  {
    context_.set_options(options_);
    context_.set_verify_mode(verify_mode_);

    if (rfc2818_verification_)
    {
      context_.set_default_verify_paths();
      context_.set_verify_callback(
          boost::asio::ssl::rfc2818_verification(hostname_));
    }

    if (use_default_verify_path_)
    {
      context_.set_default_verify_paths();
    }

    for (const auto& path : verify_paths_)
    {
      context_.add_verify_path(path);
    }

    if (!password_.empty())
    {
      context_.set_password_callback([this](auto max_length, auto purpose) {
        return password_.substr(0, max_length);
      });
    }

    if (!private_key_.empty())
    {
      context_.use_private_key(
          boost::asio::const_buffer(private_key_.data(), private_key_.size()),
          Context::pem);
    }

    if (!rsa_private_key_.empty())
    {
      context_.use_rsa_private_key(
          boost::asio::const_buffer(
              rsa_private_key_.data(), rsa_private_key_.size()),
          Context::pem);
    }

    if (!certificate_.empty())
    {
      context_.use_certificate(
          boost::asio::const_buffer(certificate_.data(), certificate_.size()),
          Context::pem);
    }

    if (!certificate_chain_.empty())
    {
      context_.use_certificate_chain(boost::asio::const_buffer(
          certificate_chain_.data(), certificate_chain_.size()));
    }

    if (!tmp_dh_.empty())
    {
      context_.set_options(Context::single_dh_use);
      context_.use_tmp_dh(
          boost::asio::const_buffer(tmp_dh_.data(), tmp_dh_.size()));
    }

    if (!certificate_authority_.empty())
    {
      context_.add_certificate_authority(boost::asio::const_buffer(
          certificate_authority_.data(), certificate_authority_.size()));
    }
  }

 private:
  const bool rfc2818_verification_;
  const std::string hostname_;
  const bool use_default_verify_path_;
  const std::vector<std::string> verify_paths_;
  const std::string password_;
  const std::string private_key_;
  const std::string rsa_private_key_;
  const std::string certificate_;
  const std::string certificate_chain_;
  const std::string tmp_dh_;
  const std::string certificate_authority_;
  const Options options_;
  const VerifyMode verify_mode_;
  Context context_;
};

} // namespace biger::common::net::ssl::context

#endif
