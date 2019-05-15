/***************************************************************************
**
** Copyright (c) 2018 UCEX
**
**    Author        : bd@biger.in
**    File          : utils.cpp
**    Date          : 2018/09/30 13:20:24
**    Description   :
**
***************************************************************************/

#include "common/utils.h"

namespace biger::common
{
//--------------------------------------
std::string Utils::B2aHex(const std::string& byte_arr)
{
  const std::string HexCodes = "0123456789abcdef";
  std::string HexString;
  for (unsigned int i = 0; i < byte_arr.size(); ++i)
  {
    unsigned char BinValue = byte_arr[i];
    HexString += HexCodes[(BinValue >> 4) & 0x0F];
    HexString += HexCodes[BinValue & 0x0F];
  }
  return HexString;
}

//---------------------------
std::string Utils::HmacSha256(const std::string& key, const std::string& data)
{
  unsigned char* digest;
  digest = HMAC(
      EVP_sha256(), key.c_str(), key.size(), (unsigned char*)data.c_str(),
      data.size(), NULL, NULL);
  return B2aHex(std::string((char*)digest, SHA256_DIGEST_LENGTH));
}

//------------------------------

std::string Utils::Sha256(const std::string& data)
{
  unsigned char hash[SHA256_DIGEST_LENGTH] = {0};
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, data.c_str(), data.size());
  SHA256_Final(hash, &sha256);
  return std::string((const char*)hash, SHA256_DIGEST_LENGTH);
}

// 公钥解密
std::string Utils::RsaPubDecrypt(
    const std::string& cipherText, const std::string& pubKey)
{
  std::string strRet;
  RSA* rsa    = NULL;
  BIO* keybio = BIO_new_mem_buf((unsigned char*)pubKey.c_str(), -1);
  rsa         = PEM_read_bio_RSAPublicKey(keybio, &rsa, NULL, NULL);
  int len     = RSA_size(rsa);
  unsigned char* decryptedText = (unsigned char*)malloc(len + 1);
  // 解密函数
  int ret = RSA_public_decrypt(
      cipherText.length(), (const unsigned char*)cipherText.c_str(),
      decryptedText, rsa, RSA_PKCS1_PADDING);
  if (ret >= 0)
  {
    strRet = std::string((char*)decryptedText, ret);
  }
  // 释放内存
  free(decryptedText);
  BIO_free_all(keybio);
  RSA_free(rsa);
  return strRet;
}

// 私钥加密
std::string Utils::RsaPriEncrypt(
    const std::string& clearText, const std::string& priKey)
{
  std::string strRet;
  RSA* rsa    = NULL;
  BIO* keybio = BIO_new_mem_buf((unsigned char*)priKey.c_str(), -1);
  rsa         = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
  int len     = RSA_size(rsa);
  unsigned char* encryptedText = (unsigned char*)malloc(len + 1);
  // 加密函数
  int ret = RSA_private_encrypt(
      clearText.length(), (const unsigned char*)clearText.c_str(),
      encryptedText, rsa, RSA_PKCS1_PADDING);
  if (ret >= 0)
  {
    strRet = std::string((char*)encryptedText, ret);
  }
  // 释放内存
  free(encryptedText);
  BIO_free_all(keybio);
  RSA_free(rsa);
  return strRet;
}

std::string Utils::Sign(const std::string& clearText, const std::string& priKey)
{
  std::string utf8_str = boost::locale::conv::to_utf<char>(clearText, "utf-8");
  return Base64::Encode(RsaPriEncrypt(Sha256(utf8_str), priKey));
}

int Utils::Verify(
    const std::string& clearText, const std::string& cipherText,
    const std::string& pubKey)
{
  std::string decryptedText;
  decryptedText.reserve(cipherText.size() / 4 * 3 + 1);
  Base64::Decode(cipherText, decryptedText);
  return clearText == RsaPubDecrypt(decryptedText, pubKey) ? 0 : 1;
}

} // namespace biger::common
