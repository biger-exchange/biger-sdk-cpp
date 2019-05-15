/***************************************************************************
**
** Copyright (c) 2018 UCEX
**
**    Author        : bd@biger.in
**    File          : biger_rest_api.cpp
**    Date          : 2018/09/30 13:20:24
**    Description   :
**
***************************************************************************/

#include "common/biger_rest_api.h"

namespace biger
{
//---------------------------------
BigerRestApi::BigerRestApi(std::string& access_tocken, std::string& secret_key)
  : access_tocken_{access_tocken}, secret_key_{secret_key}
{
  BigerRestApi::curl_ = curl_easy_init();
}

BigerRestApi::BigerRestApi()
{
  BigerRestApi::curl_ = curl_easy_init();
}

void BigerRestApi::Init()
{
  curl_global_init(CURL_GLOBAL_DEFAULT);
}

void BigerRestApi::Stop()
{
  curl_global_cleanup();
}

BigerRestApi::~BigerRestApi()
{
  curl_easy_cleanup(BigerRestApi::curl_);
}

std::string BigerRestApi::GetQueryString(
    const std::map<std::string, std::string>& params)
{
  std::string query_string;
  for (auto v : params)
  {
    query_string += v.first + "=" + v.second + "&";
  }
  boost::trim_right_if(query_string, boost::is_any_of("&"));
  return query_string;
}

void BigerRestApi::SetCommonHttpHeader(
    std::vector<std::string>& header, const std::string hash,
    uint64_t expiry_ts)
{
  header.push_back(std::string("Content-Type: application/json"));
  header.push_back(std::string("Accept: application/json"));
  header.push_back(std::string("User-Agent: biger/cpp"));
  header.push_back(std::string("BIGER-ACCESS-TOKEN: " + access_tocken_));
  header.push_back(
      std::string("BIGER-REQUEST-EXPIRY: " + std::to_string(expiry_ts)));
  header.push_back(std::string("BIGER-REQUEST-HASH: " + hash));
}

std::string BigerRestApi::GetSignData(
    const std::string& method, uint64_t expiry_ts,
    const std::string& query_string, const std::string& body)
{
  std::string data = query_string + method + std::to_string(expiry_ts) + body;
  BOOST_LOG_SEV(ucex_lg, debug) << "data: " << data;
  return data;
}

size_t BigerRestApi::OnCall(void* content, size_t size, size_t nmemb, void* f)
{
  // Call non-static member function.
  return static_cast<BigerRestApi*>(f)->OnMessage(content, size, nmemb);
}

//-----------------
// Curl's callback
size_t BigerRestApi::OnMessage(void* content, size_t size, size_t nmemb)
{
  // BOOST_LOG_SEV(ucex_lg, debug) << "<BigerRestApi::OnMessage> ";
  result_.append((char*)content, size * nmemb);
  // BOOST_LOG_SEV(ucex_lg, debug) << "<BigerRestApi::OnMessage> done";
  return size * nmemb;
}

//--------------------------------------------------
void BigerRestApi::Curl(std::string& url, std::string& result_json)
{
  std::vector<std::string> v;
  std::string action    = "GET";
  std::string post_data = "";
  CurlWithHead(url, result_json, v, post_data, action);
}

//--------------------
// Do the curl
void BigerRestApi::CurlWithHead(
    std::string& url, std::string& str_result,
    std::vector<std::string>& extra_http_header, std::string& post_data,
    std::string& action)
{
  BOOST_LOG_SEV(ucex_lg, debug) << "<BigerRestApi::CurlWithHead>";

  CURLcode res;
  result_.clear();
  if (BigerRestApi::curl_)
  {
    curl_easy_setopt(BigerRestApi::curl_, CURLOPT_URL, url.c_str());
    curl_easy_setopt(
        BigerRestApi::curl_, CURLOPT_WRITEFUNCTION, BigerRestApi::OnCall);
    curl_easy_setopt(BigerRestApi::curl_, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(BigerRestApi::curl_, CURLOPT_SSL_VERIFYPEER, false);
    curl_easy_setopt(BigerRestApi::curl_, CURLOPT_ENCODING, "gzip");

    if (extra_http_header.size() > 0)
    {
      struct curl_slist* chunk = NULL;
      for (unsigned int i = 0; i < extra_http_header.size(); i++)
      {
        chunk = curl_slist_append(chunk, extra_http_header[i].c_str());
      }
      curl_easy_setopt(BigerRestApi::curl_, CURLOPT_HTTPHEADER, chunk);
    }

    if (post_data.size() > 0 || action == "POST" || action == "PUT" ||
        action == "DELETE")
    {
      if (action == "PUT" || action == "DELETE")
      {
        curl_easy_setopt(
            BigerRestApi::curl_, CURLOPT_CUSTOMREQUEST, action.c_str());
      }
      curl_easy_setopt(
          BigerRestApi::curl_, CURLOPT_POSTFIELDS, post_data.c_str());
    }

    res = curl_easy_perform(BigerRestApi::curl_);

    /* Check for errors */
    if (res != CURLE_OK)
    {
      BOOST_LOG_SEV(ucex_lg, error)
          << "<BigerRestApi::CurlWithHead> curl_easy_perform() failed: "
          << curl_easy_strerror(res);
    }
  }
  str_result = result_;
  BOOST_LOG_SEV(ucex_lg, debug) << "<BigerRestApi::CurlWithHead> done";
}

//------------------
// GET /exchange/coins/query/all
//------------
void BigerRestApi::GetAllCoins(nlohmann::json& json_result)
{
  std::string url(BIGER_REST_API_HOST);
  url += "/exchange/coins/query/all";

  BOOST_LOG_SEV(ucex_lg, debug) << "<BigerRestApi::GetAllCoins>";

  std::string str_result;
  Curl(url, str_result);

  if (str_result.size() > 0)
  {
    try
    {
      json_result = nlohmann::json::parse(str_result);
    }
    catch (std::exception& e)
    {
      BOOST_LOG_SEV(ucex_lg, error)
          << "<BigerRestApi::GetAllCoins> Error ! reason: " << e.what();
    }
    BOOST_LOG_SEV(ucex_lg, debug) << "<BigerRestApi::GetAllCoins> Done.";
  }
  else
  {
    BOOST_LOG_SEV(ucex_lg, error)
        << "<BigerRestApi::GetAllCoins> Failed to get anything.";
  }
}

//------------------
// GET /exchange/markets/query/all
//------------
void BigerRestApi::GetAllSymbols24hr(nlohmann::json& json_result)
{
  std::string url(BIGER_REST_API_HOST);
  url += "/exchange/markets/query/all";

  BOOST_LOG_SEV(ucex_lg, debug) << "<BigerRestApi::GetAllSymbols24hr>"
                                << ", url: " << url;

  std::string str_result;
  Curl(url, str_result);

  if (str_result.size() > 0)
  {
    try
    {
      json_result = nlohmann::json::parse(str_result);
    }
    catch (std::exception& e)
    {
      BOOST_LOG_SEV(ucex_lg, error)
          << "<BigerRestApi::GetAllSymbols24hr> Error ! reason: " << e.what();
    }
    BOOST_LOG_SEV(ucex_lg, debug) << "<BigerRestApi::GetAllSymbols24hr> Done.";
  }
  else
  {
    BOOST_LOG_SEV(ucex_lg, error)
        << "<BigerRestApi::GetAllSymbols24hr> Failed to get anything.";
  }
}

//------------------
// GET /exchange/markets/query/all
//------------
void BigerRestApi::GetAccount(nlohmann::json& json_result)
{
  BOOST_LOG_SEV(ucex_lg, debug) << "<BigerRestApi::get_account>";

  if (access_tocken_.size() == 0 || secret_key_.size() == 0)
  {
    BOOST_LOG_SEV(ucex_lg, error) << "<BigerRestApi::get_account> API tocken "
                                     "and Secret Key has not been set.";
    return;
  }

  std::string url(BIGER_REST_API_HOST);
  url += "/exchange/accounts/list/accounts";
  std::string action = "GET";

  std::map<std::string, std::string> params;

  std::string querystring = GetQueryString(params);
  std::string post_data   = "";

  uint64_t expiry_ts =
      (common::chrono::Time::Now<std::chrono::seconds>() + 5) * 1000 * 1000;
  std::string signed_data =
      GetSignData(action, expiry_ts, querystring, post_data);
  std::vector<std::string> extra_http_header;
  SetCommonHttpHeader(
      extra_http_header, common::Utils::Sign(signed_data, secret_key_),
      expiry_ts);

  if (querystring.size())
  {
    url.append("/?" + querystring);
  }
  BOOST_LOG_SEV(ucex_lg, debug)
      << "<BinaCPP::get_account> url = |" << url.c_str() << "|";
  std::string str_result;
  CurlWithHead(url, str_result, extra_http_header, post_data, action);

  if (str_result.size() > 0)
  {
    try
    {
      json_result = nlohmann::json::parse(str_result);
    }
    catch (std::exception& e)
    {
      BOOST_LOG_SEV(ucex_lg, error)
          << "<BigerRestApi::get_account> Error ! reason: " << e.what();
    }
    BOOST_LOG_SEV(ucex_lg, debug) << "<BigerRestApi::get_account> Done.";
  }
  else
  {
    BOOST_LOG_SEV(ucex_lg, error)
        << "<BigerRestApi::get_account> Failed to get anything.";
  }

  BOOST_LOG_SEV(ucex_lg, debug) << "<BigerRestApi::get_account> Done.";
}

//------------------
// 查询指定单
// 路径： /exchange/orders/get/orderId/{orderId}
//------------
void BigerRestApi::QueryOrder(
    const std::string& order_id, nlohmann::json& json_result)
{
  BOOST_LOG_SEV(ucex_lg, debug) << "<BigerRestApi::QueryOrder>";

  if (access_tocken_.size() == 0 || secret_key_.size() == 0)
  {
    BOOST_LOG_SEV(ucex_lg, error) << "<BigerRestApi::QueryOrder> API tocken "
                                     "and Secret Key has not been set.";
    return;
  }

  std::string url(BIGER_REST_API_HOST);
  url += "/exchange/orders/get/orderId/" + order_id;
  std::string action = "GET";

  std::map<std::string, std::string> params;
  std::string querystring = GetQueryString(params);
  std::string post_data   = "";

  uint64_t expiry_ts =
      (common::chrono::Time::Now<std::chrono::seconds>() + 5) * 1000 * 1000;
  std::string signed_data =
      GetSignData(action, expiry_ts, querystring, post_data);
  std::vector<std::string> extra_http_header;
  SetCommonHttpHeader(
      extra_http_header, common::Utils::Sign(signed_data, secret_key_),
      expiry_ts);

  if (querystring.size())
  {
    url.append("/?" + querystring);
  }
  BOOST_LOG_SEV(ucex_lg, debug)
      << "<BinaCPP::QueryOrder> url = |" << url.c_str() << "|";
  std::string str_result;
  CurlWithHead(url, str_result, extra_http_header, post_data, action);

  if (str_result.size() > 0)
  {
    try
    {
      json_result = nlohmann::json::parse(str_result);
    }
    catch (std::exception& e)
    {
      BOOST_LOG_SEV(ucex_lg, error)
          << "<BigerRestApi::QueryOrder> Error ! reason: " << e.what();
    }
    BOOST_LOG_SEV(ucex_lg, debug) << "<BigerRestApi::QueryOrder> Done.";
  }
  else
  {
    BOOST_LOG_SEV(ucex_lg, error)
        << "<BigerRestApi::QueryOrder> Failed to get anything.";
  }

  BOOST_LOG_SEV(ucex_lg, debug) << "<BigerRestApi::QueryOrder> Done.";
}

//------------------
// 限价单
// 路径：  /exchange/orders/create 方法: POST 请求体
//------------
void BigerRestApi::PutLimitOrder(
    const std::string& symbol, const std::string& side,
    const std::string& price, const std::string& amount,
    nlohmann::json& json_result)
{
  BOOST_LOG_SEV(ucex_lg, debug) << "<BigerRestApi::PutLimitOrder>";

  if (access_tocken_.size() == 0 || secret_key_.size() == 0)
  {
    BOOST_LOG_SEV(ucex_lg, error) << "<BigerRestApi::PutLimitOrder> API tocken "
                                     "and Secret Key has not been set.";
    return;
  }

  std::string url(BIGER_REST_API_HOST);
  url += "/exchange/orders/create";
  std::string action = "POST";

  std::map<std::string, std::string> params;
  std::string querystring = GetQueryString(params);
  std::string post_data   = nlohmann::json{
      {"symbol", symbol},
      {"side", side},
      {"price", price},
      {"orderQty", amount},
      {"orderType", "LIMIT"}}.dump();

  uint64_t expiry_ts =
      (common::chrono::Time::Now<std::chrono::seconds>() + 5) * 1000 * 1000;
  std::string signed_data =
      GetSignData(action, expiry_ts, querystring, post_data);
  std::vector<std::string> extra_http_header;
  SetCommonHttpHeader(
      extra_http_header, common::Utils::Sign(signed_data, secret_key_),
      expiry_ts);

  if (querystring.size())
  {
    url.append("/?" + querystring);
  }
  BOOST_LOG_SEV(ucex_lg, debug)
      << "<BinaCPP::PutLimitOrder> url = |" << url.c_str() << "|";
  std::string str_result;
  CurlWithHead(url, str_result, extra_http_header, post_data, action);

  if (str_result.size() > 0)
  {
    try
    {
      json_result = nlohmann::json::parse(str_result);
    }
    catch (std::exception& e)
    {
      BOOST_LOG_SEV(ucex_lg, error)
          << "<BigerRestApi::PutLimitOrder> Error ! reason: " << e.what();
    }
    BOOST_LOG_SEV(ucex_lg, debug) << "<BigerRestApi::PutLimitOrder> Done.";
  }
  else
  {
    BOOST_LOG_SEV(ucex_lg, error)
        << "<BigerRestApi::PutLimitOrder> Failed to get anything.";
  }

  BOOST_LOG_SEV(ucex_lg, debug) << "<BigerRestApi::PutLimitOrder> Done.";
}

//------------------
// 撤单
// 路径： /exchange/orders/cancel/{orderId} 方法: PUT
//------------
void BigerRestApi::CancelOrder(
    const std::string& order_id, nlohmann::json& json_result)
{
  BOOST_LOG_SEV(ucex_lg, debug) << "<BigerRestApi::CancelOrder>";

  if (access_tocken_.size() == 0 || secret_key_.size() == 0)
  {
    BOOST_LOG_SEV(ucex_lg, error) << "<BigerRestApi::CancelOrder> API tocken "
                                     "and Secret Key has not been set.";
    return;
  }

  std::string url(BIGER_REST_API_HOST);
  url += "/exchange/orders/cancel/" + order_id;
  std::string action = "PUT";

  std::map<std::string, std::string> params;
  std::string querystring = GetQueryString(params);
  std::string post_data   = "";

  uint64_t expiry_ts =
      (common::chrono::Time::Now<std::chrono::seconds>() + 5) * 1000 * 1000;
  std::string signed_data =
      GetSignData(action, expiry_ts, querystring, post_data);
  std::vector<std::string> extra_http_header;
  SetCommonHttpHeader(
      extra_http_header, common::Utils::Sign(signed_data, secret_key_),
      expiry_ts);

  if (querystring.size())
  {
    url.append("/?" + querystring);
  }
  BOOST_LOG_SEV(ucex_lg, debug)
      << "<BinaCPP::CancelOrder> url = |" << url.c_str() << "|";
  std::string str_result;
  CurlWithHead(url, str_result, extra_http_header, post_data, action);

  if (str_result.size() > 0)
  {
    try
    {
      json_result = nlohmann::json::parse(str_result);
    }
    catch (std::exception& e)
    {
      BOOST_LOG_SEV(ucex_lg, error)
          << "<BigerRestApi::CancelOrder> Error ! reason: " << e.what();
    }
    BOOST_LOG_SEV(ucex_lg, debug) << "<BigerRestApi::CancelOrder> Done.";
  }
  else
  {
    BOOST_LOG_SEV(ucex_lg, error)
        << "<BigerRestApi::CancelOrder> Failed to get anything.";
  }

  BOOST_LOG_SEV(ucex_lg, debug) << "<BigerRestApi::CancelOrder> Done.";
}

} // namespace biger
