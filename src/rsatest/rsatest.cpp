#include <iostream>

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/ssl.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"

int private_encrypt(
    unsigned char* data, int data_len, unsigned char* key,
    unsigned char* encrypted)
{
  RSA* rsa    = nullptr;
  BIO* keybio = nullptr;
  keybio      = BIO_new_mem_buf(key, -1);
  if (keybio == nullptr)
  {
    std::cout << "load error key";
  }
  // assert(keybio == NULL);
  rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
  if (rsa == nullptr)
  {
    std::cout << "read rsa key failed";
  }
  // assert(rsa == NULL);
  int result =
      RSA_private_encrypt(data_len, data, encrypted, rsa, RSA_PKCS1_PADDING);
  return result;
}
int public_decrypt(
    unsigned char* enc_data, int data_len, unsigned char* key,
    unsigned char* decrypted)
{
  RSA* rsa    = NULL;
  BIO* keybio = NULL;
  keybio      = BIO_new_mem_buf(key, -1);
  if (keybio == nullptr)
  {
    std::cout << "load error key";
  }
  // assert(keybio == NULL);
  rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);
  if (rsa == nullptr)
  {
    std::cout << "read rsa key failed";
  }
  // assert(rsa == NULL);
  int result =
      RSA_public_decrypt(data_len, enc_data, decrypted, rsa, RSA_PKCS1_PADDING);
  return result;
}

int main(int argc, char** argv)
{
  using namespace biger;
  std::string signed_data = "test";

  std::string secret_key =
      "-----BEGIN RSA PRIVATE KEY-----\n"
      "MIIEowIBAAKCAQEAy8Dbv8prpJ/0kKhlGeJYozo2t60EG8L0561g13R29LvMR5hy\n"
      "vGZlGJpmn65+A4xHXInJYiPuKzrKUnApeLZ+vw1HocOAZtWK0z3r26uA8kQYOKX9\n"
      "Qt/DbCdvsF9wF8gRK0ptx9M6R13NvBxvVQApfc9jB9nTzphOgM4JiEYvlV8FLhg9\n"
      "yZovMYd6Wwf3aoXK891VQxTr/kQYoq1Yp+68i6T4nNq7NWC+UNVjQHxNQMQMzU6l\n"
      "WCX8zyg3yH88OAQkUXIXKfQ+NkvYQ1cxaMoVPpY72+eVthKzpMeyHkBn7ciumk5q\n"
      "gLTEJAfWZpe4f4eFZj/Rc8Y8Jj2IS5kVPjUywQIDAQABAoIBADhg1u1Mv1hAAlX8\n"
      "omz1Gn2f4AAW2aos2cM5UDCNw1SYmj+9SRIkaxjRsE/C4o9sw1oxrg1/z6kajV0e\n"
      "N/t008FdlVKHXAIYWF93JMoVvIpMmT8jft6AN/y3NMpivgt2inmmEJZYNioFJKZG\n"
      "X+/vKYvsVISZm2fw8NfnKvAQK55yu+GRWBZGOeS9K+LbYvOwcrjKhHz66m4bedKd\n"
      "gVAix6NE5iwmjNXktSQlJMCjbtdNXg/xo1/G4kG2p/MO1HLcKfe1N5FgBiXj3Qjl\n"
      "vgvjJZkh1as2KTgaPOBqZaP03738VnYg23ISyvfT/teArVGtxrmFP7939EvJFKpF\n"
      "1wTxuDkCgYEA7t0DR37zt+dEJy+5vm7zSmN97VenwQJFWMiulkHGa0yU3lLasxxu\n"
      "m0oUtndIjenIvSx6t3Y+agK2F3EPbb0AZ5wZ1p1IXs4vktgeQwSSBdqcM8LZFDvZ\n"
      "uPboQnJoRdIkd62XnP5ekIEIBAfOp8v2wFpSfE7nNH2u4CpAXNSF9HsCgYEA2l8D\n"
      "JrDE5m9Kkn+J4l+AdGfeBL1igPF3DnuPoV67BpgiaAgI4h25UJzXiDKKoa706S0D\n"
      "4XB74zOLX11MaGPMIdhlG+SgeQfNoC5lE4ZWXNyESJH1SVgRGT9nBC2vtL6bxCVV\n"
      "WBkTeC5D6c/QXcai6yw6OYyNNdp0uznKURe1xvMCgYBVYYcEjWqMuAvyferFGV+5\n"
      "nWqr5gM+yJMFM2bEqupD/HHSLoeiMm2O8KIKvwSeRYzNohKTdZ7FwgZYxr8fGMoG\n"
      "PxQ1VK9DxCvZL4tRpVaU5Rmknud9hg9DQG6xIbgIDR+f79sb8QjYWmcFGc1SyWOA\n"
      "SkjlykZ2yt4xnqi3BfiD9QKBgGqLgRYXmXp1QoVIBRaWUi55nzHg1XbkWZqPXvz1\n"
      "I3uMLv1jLjJlHk3euKqTPmC05HoApKwSHeA0/gOBmg404xyAYJTDcCidTg6hlF96\n"
      "ZBja3xApZuxqM62F6dV4FQqzFX0WWhWp5n301N33r0qR6FumMKJzmVJ1TA8tmzEF\n"
      "yINRAoGBAJqioYs8rK6eXzA8ywYLjqTLu/yQSLBn/4ta36K8DyCoLNlNxSuox+A5\n"
      "w6z2vEfRVQDq4Hm4vBzjdi3QfYLNkTiTqLcvgWZ+eX44ogXtdTDO7c+GeMKWz4XX\n"
      "uJSUVL5+CVjKLjZEJ6Qc2WZLl94xSwL71E41H4YciVnSCQxVc4Jw\n"
      "-----END RSA PRIVATE KEY-----\n";

  unsigned char privateKey[] =
      "-----BEGIN RSA PRIVATE KEY-----\n"
      "MIIEowIBAAKCAQEAy8Dbv8prpJ/0kKhlGeJYozo2t60EG8L0561g13R29LvMR5hy\n"
      "vGZlGJpmn65+A4xHXInJYiPuKzrKUnApeLZ+vw1HocOAZtWK0z3r26uA8kQYOKX9\n"
      "Qt/DbCdvsF9wF8gRK0ptx9M6R13NvBxvVQApfc9jB9nTzphOgM4JiEYvlV8FLhg9\n"
      "yZovMYd6Wwf3aoXK891VQxTr/kQYoq1Yp+68i6T4nNq7NWC+UNVjQHxNQMQMzU6l\n"
      "WCX8zyg3yH88OAQkUXIXKfQ+NkvYQ1cxaMoVPpY72+eVthKzpMeyHkBn7ciumk5q\n"
      "gLTEJAfWZpe4f4eFZj/Rc8Y8Jj2IS5kVPjUywQIDAQABAoIBADhg1u1Mv1hAAlX8\n"
      "omz1Gn2f4AAW2aos2cM5UDCNw1SYmj+9SRIkaxjRsE/C4o9sw1oxrg1/z6kajV0e\n"
      "N/t008FdlVKHXAIYWF93JMoVvIpMmT8jft6AN/y3NMpivgt2inmmEJZYNioFJKZG\n"
      "X+/vKYvsVISZm2fw8NfnKvAQK55yu+GRWBZGOeS9K+LbYvOwcrjKhHz66m4bedKd\n"
      "gVAix6NE5iwmjNXktSQlJMCjbtdNXg/xo1/G4kG2p/MO1HLcKfe1N5FgBiXj3Qjl\n"
      "vgvjJZkh1as2KTgaPOBqZaP03738VnYg23ISyvfT/teArVGtxrmFP7939EvJFKpF\n"
      "1wTxuDkCgYEA7t0DR37zt+dEJy+5vm7zSmN97VenwQJFWMiulkHGa0yU3lLasxxu\n"
      "m0oUtndIjenIvSx6t3Y+agK2F3EPbb0AZ5wZ1p1IXs4vktgeQwSSBdqcM8LZFDvZ\n"
      "uPboQnJoRdIkd62XnP5ekIEIBAfOp8v2wFpSfE7nNH2u4CpAXNSF9HsCgYEA2l8D\n"
      "JrDE5m9Kkn+J4l+AdGfeBL1igPF3DnuPoV67BpgiaAgI4h25UJzXiDKKoa706S0D\n"
      "4XB74zOLX11MaGPMIdhlG+SgeQfNoC5lE4ZWXNyESJH1SVgRGT9nBC2vtL6bxCVV\n"
      "WBkTeC5D6c/QXcai6yw6OYyNNdp0uznKURe1xvMCgYBVYYcEjWqMuAvyferFGV+5\n"
      "nWqr5gM+yJMFM2bEqupD/HHSLoeiMm2O8KIKvwSeRYzNohKTdZ7FwgZYxr8fGMoG\n"
      "PxQ1VK9DxCvZL4tRpVaU5Rmknud9hg9DQG6xIbgIDR+f79sb8QjYWmcFGc1SyWOA\n"
      "SkjlykZ2yt4xnqi3BfiD9QKBgGqLgRYXmXp1QoVIBRaWUi55nzHg1XbkWZqPXvz1\n"
      "I3uMLv1jLjJlHk3euKqTPmC05HoApKwSHeA0/gOBmg404xyAYJTDcCidTg6hlF96\n"
      "ZBja3xApZuxqM62F6dV4FQqzFX0WWhWp5n301N33r0qR6FumMKJzmVJ1TA8tmzEF\n"
      "yINRAoGBAJqioYs8rK6eXzA8ywYLjqTLu/yQSLBn/4ta36K8DyCoLNlNxSuox+A5\n"
      "w6z2vEfRVQDq4Hm4vBzjdi3QfYLNkTiTqLcvgWZ+eX44ogXtdTDO7c+GeMKWz4XX\n"
      "uJSUVL5+CVjKLjZEJ6Qc2WZLl94xSwL71E41H4YciVnSCQxVc4Jw\n"
      "-----END RSA PRIVATE KEY-----\n";

  unsigned char privateKey123[] =
      "-----BEGIN RSA PRIVATE KEY-----\n"
      "MIICWgIBAAKBgFUIytlnjO9kbfSXh0D8Rkar79Nblt6sWi2SLJqMpyxlzqKrzhkW\n"
      "LpEgtaCmfgUyDlxwpL38waWXRA4BHVvzRUztvH4e3gObjwZxenXpl8Au5Sc85sm6\n"
      "mnyV2StjeYeWOKDyJ87/nBC8gNaMb65Z38kPmLuFESvCszmEklxRqL6xAgMBAAEC\n"
      "gYA6CN4osnuFhs1keWZd+88avI3ZelDleEuzfmfisswFiRYV/5uRk4oEkoZjNj4b\n"
      "3aXfgSFuaOrg0PQpeqlG8CkDJnhGEe5t4GNQQOGDI2fnQ7UXAjQSFtISJQu9I8Oz\n"
      "wxRHr+B81trIyzLja+AYGrDm3/1SSBAy5+292XyaJW80gQJBAJRNPpCUtsALqcby\n"
      "wUhZAU2GhdLv7tZJPTSxQLrt2vB/tw1XPC8hTOxlvg2lOBjerfyLxcYhOpT6E3lb\n"
      "coq69mUCQQCSyYQWbwfQ7egcq044U+JkHWm9av6LSC0RxZj5xLqS5zwyVSXvQEu5\n"
      "DbAPaiWydf5EzEtiVwoWI4bMSbYSJoxdAkBSrtpyC6f0XMxUkqX2q0ERsy3LlGAp\n"
      "8v1/8k9vqQuHSP2LH5b7g+p6ZqNWwkYLf6OriVZEB+S8iMzwvW6YMHMNAkBS0T+l\n"
      "KJ/QUWpUQpKvVSS2N6IhLOzQyLgk/seApG5f0/cyrrfodO5ESmS7TbhXKBt91YXy\n"
      "xgj61LCJMk13kChBAkBwXwAxM5c0qPMbLs2mKDQbqb6KYgcFQZOjsj8u3T6zQvnX\n"
      "4jXF5U9sgNwyC/2IYVJvMAh9hXlFtEeGS3w2XL2M\n"
      "-----END RSA PRIVATE KEY-----\n";

  unsigned char publicKey[] =
      "-----BEGIN PUBLIC KEY-----\n"
      "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAy8Dbv8prpJ/0kKhlGeJY\n"
      "ozo2t60EG8L0561g13R29LvMR5hyvGZlGJpmn65+A4xHXInJYiPuKzrKUnApeLZ+\n"
      "vw1HocOAZtWK0z3r26uA8kQYOKX9Qt/DbCdvsF9wF8gRK0ptx9M6R13NvBxvVQAp\n"
      "fc9jB9nTzphOgM4JiEYvlV8FLhg9yZovMYd6Wwf3aoXK891VQxTr/kQYoq1Yp+68\n"
      "i6T4nNq7NWC+UNVjQHxNQMQMzU6lWCX8zyg3yH88OAQkUXIXKfQ+NkvYQ1cxaMoV\n"
      "PpY72+eVthKzpMeyHkBn7ciumk5qgLTEJAfWZpe4f4eFZj/Rc8Y8Jj2IS5kVPjUy\n"
      "wQIDAQAB\n"
      "-----END PUBLIC KEY-----\n";

  std::string privateKey_str =
      "-----BEGIN RSA PRIVATE KEY-----\n"
      "MIICWgIBAAKBgFUIytlnjO9kbfSXh0D8Rkar79Nblt6sWi2SLJqMpyxlzqKrzhkW\n"
      "LpEgtaCmfgUyDlxwpL38waWXRA4BHVvzRUztvH4e3gObjwZxenXpl8Au5Sc85sm6\n"
      "mnyV2StjeYeWOKDyJ87/nBC8gNaMb65Z38kPmLuFESvCszmEklxRqL6xAgMBAAEC\n"
      "gYA6CN4osnuFhs1keWZd+88avI3ZelDleEuzfmfisswFiRYV/5uRk4oEkoZjNj4b\n"
      "3aXfgSFuaOrg0PQpeqlG8CkDJnhGEe5t4GNQQOGDI2fnQ7UXAjQSFtISJQu9I8Oz\n"
      "wxRHr+B81trIyzLja+AYGrDm3/1SSBAy5+292XyaJW80gQJBAJRNPpCUtsALqcby\n"
      "wUhZAU2GhdLv7tZJPTSxQLrt2vB/tw1XPC8hTOxlvg2lOBjerfyLxcYhOpT6E3lb\n"
      "coq69mUCQQCSyYQWbwfQ7egcq044U+JkHWm9av6LSC0RxZj5xLqS5zwyVSXvQEu5\n"
      "DbAPaiWydf5EzEtiVwoWI4bMSbYSJoxdAkBSrtpyC6f0XMxUkqX2q0ERsy3LlGAp\n"
      "8v1/8k9vqQuHSP2LH5b7g+p6ZqNWwkYLf6OriVZEB+S8iMzwvW6YMHMNAkBS0T+l\n"
      "KJ/QUWpUQpKvVSS2N6IhLOzQyLgk/seApG5f0/cyrrfodO5ESmS7TbhXKBt91YXy\n"
      "xgj61LCJMk13kChBAkBwXwAxM5c0qPMbLs2mKDQbqb6KYgcFQZOjsj8u3T6zQvnX\n"
      "4jXF5U9sgNwyC/2IYVJvMAh9hXlFtEeGS3w2XL2M\n"
      "-----END RSA PRIVATE KEY-----\n";

  char plainText[2048 / 8]      = "HellothisisRavi";
  unsigned char encrypted[4098] = {};
  unsigned char decrypted[4098] = {};

  int encrypted_length = private_encrypt(
      (unsigned char*)plainText, strlen(plainText), privateKey, encrypted);
  printf("Encrypted length =%d\n", encrypted_length);

  int decrypted_length =
      public_decrypt(encrypted, encrypted_length, publicKey, decrypted);
  printf("Decrypted Text =%s\n", decrypted);
  printf("Decrypted Length =%d\n", decrypted_length);

  std::cout << "sha256 data: "
            << common::Base64::Encode(common::Utils::Sha256(plainText))
            << std::endl;

  std::cout << "signed data: "
            << common::Utils::Sign(signed_data, privateKey_str) << std::endl;

  return 0;
}
