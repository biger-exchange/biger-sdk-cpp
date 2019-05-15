/***************************************************************************
**
** Copyright (c) 2018 UCEX
**
**    Author        : bd@biger.in
**    File          : utils.h
**    Date          : 2018/09/30 13:20:24
**    Description   :
**
***************************************************************************/

#ifndef BIGER_COMMON_UTILS_H
#define BIGER_COMMON_UTILS_H

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <assert.h>
#include <boost/locale.hpp>
#include <openssl/err.h>
#include <openssl/hmac.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>

#include "common/base64.hpp"

namespace biger::common
{
struct Utils
{
  static std::string B2aHex(const std::string& byte_arr);
  static std::string HmacSha256(
      const std::string& key, const std::string& data);
  static std::string Sha256(const std::string& data);
  // static RSA* CreateRSA(unsigned char * key,int pub);
  static std::string RsaPubDecrypt(
      const std::string& cipherText, const std::string& pubKey);
  static std::string RsaPriEncrypt(
      const std::string& clearText, const std::string& priKey);
  static std::string Sign(
      const std::string& clearText, const std::string& priKey);
  static int Verify(
      const std::string& clearText, const std::string& cipherText,
      const std::string& pubKey);
};
} // namespace biger::common

#endif
