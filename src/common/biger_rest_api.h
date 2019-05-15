/***************************************************************************
**
** Copyright (c) 2018 UCEX
**
**    Author        : bd@biger.in
**    File          : biger_rest_api.h
**    Date          : 2018/09/30 13:20:24
**    Description   :
**
***************************************************************************/

#ifndef BIGER_REST_API_H
#define BIGER_REST_API_H

#include <exception>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/chrono/time.hpp"
#include "common/log.hpp"
#include "common/utils.h"

//#define BIGER_REST_API_HOST "https://biger.pro/web-api-gateway"
#define BIGER_REST_API_HOST "http://pub-api.qa.ccx123.com"
//#define BIGER_REST_API_HOST "http://127.0.0.1:10241"

namespace biger
{
class BigerRestApi
{
 public:
  BigerRestApi();
  BigerRestApi(std::string& access_tocken, std::string& secret_key);
  ~BigerRestApi();
  void Curl(std::string& url, std::string& result_json);
  void CurlWithHead(
      std::string& url, std::string& result_json,
      std::vector<std::string>& extra_http_header, std::string& post_data,
      std::string& action);
  size_t OnMessage(void* content, size_t size, size_t nmemb);

  static void Init();
  static void Stop();
  static size_t OnCall(void* content, size_t size, size_t nmemb, void* f);
  std::string GetQueryString(const std::map<std::string, std::string>& params);
  void SetCommonHttpHeader(
      std::vector<std::string>& header, const std::string hash,
      uint64_t expiry_ts);
  std::string GetSignData(
      const std::string& method, uint64_t expiry_ts,
      const std::string& query_string, const std::string& body);

  // Public API
  void GetAllCoins(nlohmann::json& json_result);
  void GetAllSymbols24hr(nlohmann::json& json_result);
  void GetAccount(nlohmann::json& json_result);
  void QueryOrder(const std::string& order_id, nlohmann::json& json_result);
  void PutLimitOrder(
      const std::string& symbol, const std::string& side,
      const std::string& price, const std::string& amount,
      nlohmann::json& json_result);
  void CancelOrder(const std::string& order_id, nlohmann::json& json_result);

 private:
  std::string access_tocken_ = "maxiao";
  std::string secret_key_ =
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

  CURL* curl_ = NULL;
  std::string result_;
};
} // namespace biger

#endif
