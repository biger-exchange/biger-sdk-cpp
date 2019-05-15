/****************************************************************************
**
**  Copyright (c) 2018 Du Yang
**
**    Author        : bd@biger.in
**    File          : client.hpp
**    Date          : 2019-02-28 15:04:00
**    Description   :
**
****************************************************************************/

#ifndef BIGER_COMMON_NET_SSL_CONTEXT_CLIENT_HPP
#define BIGER_COMMON_NET_SSL_CONTEXT_CLIENT_HPP

#include "common/net/ssl/context/base.hpp"

namespace biger::common::net::ssl::context
{
class Client : public Base
{
 public:
  template <class Config>
  Client(
      const Config& config, Method method = Context::sslv23_client,
      Options options = Context::default_workarounds | Context::no_sslv2 |
                        Context::no_sslv3,
      VerifyMode mode = boost::asio::ssl::verify_none)
    : Base{config, method, options, mode}
  {
    try
    {
      Init();
    }
    catch (const std::exception& e)
    {
      throw std::runtime_error{
          std::string{"failed to init client ssl context: "} + e.what()};
    }
  }

  template <class Config, class Type>
  Client(const Config& config, const Type& type)
    : Base{config, type.method, type.options, type.mode}
  {
  }

 protected:
  void Init()
  {
    if (Base::CertificateAuthority().empty())
    {
      Base::Get().add_certificate_authority(
          boost::asio::const_buffer(kRootCA, sizeof(kRootCA)));
    }
  }

 private:
  static constexpr auto kRootCA =
      /*  This is the DigiCert root certificate.

          CN = DigiCert High Assurance EV Root CA
          OU = www.digicert.com
          O = DigiCert Inc
          C = US

          Valid to: Sunday, ?November ?9, ?2031 5:00:00 PM

          Thumbprint(sha1):
          5f b7 ee 06 33 e2 59 db ad 0c 4c 9a e6 d3 8f 1a 61 c7 dc 25
      */
      "-----BEGIN CERTIFICATE-----\n"
      "MIIDxTCCAq2gAwIBAgIQAqxcJmoLQJuPC3nyrkYldzANBgkqhkiG9w0BAQUFADBs\n"
      "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
      "d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j\n"
      "ZSBFViBSb290IENBMB4XDTA2MTExMDAwMDAwMFoXDTMxMTExMDAwMDAwMFowbDEL\n"
      "MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3\n"
      "LmRpZ2ljZXJ0LmNvbTErMCkGA1UEAxMiRGlnaUNlcnQgSGlnaCBBc3N1cmFuY2Ug\n"
      "RVYgUm9vdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMbM5XPm\n"
      "+9S75S0tMqbf5YE/yc0lSbZxKsPVlDRnogocsF9ppkCxxLeyj9CYpKlBWTrT3JTW\n"
      "PNt0OKRKzE0lgvdKpVMSOO7zSW1xkX5jtqumX8OkhPhPYlG++MXs2ziS4wblCJEM\n"
      "xChBVfvLWokVfnHoNb9Ncgk9vjo4UFt3MRuNs8ckRZqnrG0AFFoEt7oT61EKmEFB\n"
      "Ik5lYYeBQVCmeVyJ3hlKV9Uu5l0cUyx+mM0aBhakaHPQNAQTXKFx01p8VdteZOE3\n"
      "hzBWBOURtCmAEvF5OYiiAhF8J2a3iLd48soKqDirCmTCv2ZdlYTBoSUeh10aUAsg\n"
      "EsxBu24LUTi4S8sCAwEAAaNjMGEwDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQF\n"
      "MAMBAf8wHQYDVR0OBBYEFLE+w2kD+L9HAdSYJhoIAu9jZCvDMB8GA1UdIwQYMBaA\n"
      "FLE+w2kD+L9HAdSYJhoIAu9jZCvDMA0GCSqGSIb3DQEBBQUAA4IBAQAcGgaX3Nec\n"
      "nzyIZgYIVyHbIUf4KmeqvxgydkAQV8GK83rZEWWONfqe/EW1ntlMMUu4kehDLI6z\n"
      "eM7b41N5cdblIZQB2lWHmiRk9opmzN6cN82oNLFpmyPInngiK3BD41VHMWEZ71jF\n"
      "hS9OMPagMRYjyOfiZRYzy78aG6A9+MpeizGLYAiJLQwGXFK3xPkKmNEVX58Svnw2\n"
      "Yzi9RKR/5CYrCsSXaQ3pjOLAEFe4yHYSkVXySGnYvCoCWw9E1CAx2/S6cCZdkGCe\n"
      "vEsXCS+0yx5DaMkHJ8HSXPfqIbloEpw8nL+e/IBcm2PN7EeqJSdnoDfzAIJ9VNep\n"
      "+OkuE6N36B9K\n"
      "-----END CERTIFICATE-----\n"
      /*  This is the GeoTrust root certificate.

          CN = GeoTrust Global CA
          O = GeoTrust Inc.
          C = US
          Valid to: Friday, May 20, 2022 9:00:00 PM

          Thumbprint(sha1):
          de 28 f4 a4 ff e5 b9 2f a3 c5 03 d1 a3 49 a7 f9 96 2a 82 12
      */
      "-----BEGIN CERTIFICATE-----\n"
      "MIIDVDCCAjygAwIBAgIDAjRWMA0GCSqGSIb3DQEBBQUAMEIxCzAJBgNVBAYTAlVT\n"
      "MRYwFAYDVQQKEw1HZW9UcnVzdCBJbmMuMRswGQYDVQQDExJHZW9UcnVzdCBHbG9i\n"
      "YWwgQ0EwHhcNMDIwNTIxMDQwMDAwWhcNMjIwNTIxMDQwMDAwWjBCMQswCQYDVQQG\n"
      "EwJVUzEWMBQGA1UEChMNR2VvVHJ1c3QgSW5jLjEbMBkGA1UEAxMSR2VvVHJ1c3Qg\n"
      "R2xvYmFsIENBMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA2swYYzD9\n"
      "9BcjGlZ+W988bDjkcbd4kdS8odhM+KhDtgPpTSEHCIjaWC9mOSm9BXiLnTjoBbdq\n"
      "fnGk5sRgprDvgOSJKA+eJdbtg/OtppHHmMlCGDUUna2YRpIuT8rxh0PBFpVXLVDv\n"
      "iS2Aelet8u5fa9IAjbkU+BQVNdnARqN7csiRv8lVK83Qlz6cJmTM386DGXHKTubU\n"
      "1XupGc1V3sjs0l44U+VcT4wt/lAjNvxm5suOpDkZALeVAjmRCw7+OC7RHQWa9k0+\n"
      "bw8HHa8sHo9gOeL6NlMTOdReJivbPagUvTLrGAMoUgRx5aszPeE4uwc2hGKceeoW\n"
      "MPRfwCvocWvk+QIDAQABo1MwUTAPBgNVHRMBAf8EBTADAQH/MB0GA1UdDgQWBBTA\n"
      "ephojYn7qwVkDBF9qn1luMrMTjAfBgNVHSMEGDAWgBTAephojYn7qwVkDBF9qn1l\n"
      "uMrMTjANBgkqhkiG9w0BAQUFAAOCAQEANeMpauUvXVSOKVCUn5kaFOSPeCpilKIn\n"
      "Z57QzxpeR+nBsqTP3UEaBU6bS+5Kb1VSsyShNwrrZHYqLizz/Tt1kL/6cdjHPTfS\n"
      "tQWVYrmm3ok9Nns4d0iXrKYgjy6myQzCsplFAMfOEVEiIuCl6rYVSAlk6l5PdPcF\n"
      "PseKUgzbFbS9bZvlxrFUaKnjaZC2mqUPuLk/IH2uSrW4nOQdtqvmlKXBx4Ot2/Un\n"
      "hw4EbNX/3aBd7YdStysVAq45pmp06drE57xNNB6pXE0zX5IJL4hmXXeXxx12E6nV\n"
      "5fEWCRE11azbJHFwLJhWC9kXtNHjUStedejV0NxPNO3CBWaAocvmMw==\n"
      "-----END CERTIFICATE-----\n";
};

struct ClientVerifyNone : public Client
{
  template <class... Args>
  ClientVerifyNone(Args&&... args)
    : Client(
          std::forward<Args>(args)...,
          Context::default_workarounds | Context::no_sslv2 | Context::no_sslv3,
          boost::asio::ssl::verify_none)
  {
    static_assert(
        sizeof...(args) == 2,
        "No. of arguments of ClientVerifyNone should be 2");
  }
};

struct ClientVerifyPeer : public Client
{
  template <class... Args>
  ClientVerifyPeer(Args&&... args)
    : Client(
          std::forward<Args>(args)...,
          Context::default_workarounds | Context::no_sslv2 | Context::no_sslv3,
          boost::asio::ssl::verify_peer)
  {
    static_assert(
        sizeof...(args) == 2,
        "No. of arguments of ClientVerifyPeer should be 2");
  }
};

} // namespace biger::common::net::ssl::context

#endif
