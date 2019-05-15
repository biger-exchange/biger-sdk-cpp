/****************************************************************************
**
**  Copyright (c) 2018 Du Yang
**
**    Author        : bd@biger.in
**    File          : url.hpp
**    Date          : 2018-12-06 16:29:54
**    Description   :
**
****************************************************************************/

#ifndef BIGER_COMMON_NET_URL_HPP
#define BIGER_COMMON_NET_URL_HPP

#include <regex>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace biger::common::net
{
struct Url
{
  Url(const std::string& url) : url{url}
  {
    Parse();
    ParseTarget();
    ParseQuery();
  }

  static inline auto GetPort(
      const std::string& protocol, const std::string& url = std::string{})
  {
    if ("https" == protocol || "wss" == protocol)
    {
      return 443;
    }
    else if ("http" == protocol || "ws" == protocol)
    {
      return 80;
    }
    else if ("tcps" == protocol || "tcp" == protocol || "ssl" == protocol)
    {
      return 0;
    }
    else
    {
      throw std::invalid_argument{"port should be set, url: " + url};
    }
  }

  void Parse()
  {
    std::regex url_pattern1{"([a-z]+)://([^/:]+):([0-9]+)(/.*)?"};
    std::regex url_pattern2{"([a-z]+)://([^/:]+)(/.*)?"};
    std::smatch sm;

    if (std::regex_match(url, sm, url_pattern1))
    {
      protocol = sm[1];
      host     = sm[2];
      port     = boost::lexical_cast<unsigned short>(sm[3]);
      target   = "/";

      if (sm.size() > 4 && !sm[4].str().empty())
      {
        target = sm[4];
      }
    }
    else if (std::regex_match(url, sm, url_pattern2))
    {
      protocol = sm[1];
      host     = sm[2];
      target   = "/";
      port     = GetPort(protocol, url);

      if (sm.size() > 3 && !sm[3].str().empty())
      {
        target = sm[3];
      }
    }
    else
    {
      throw std::invalid_argument{"invalid url string: " + url};
    }
  }

  void ParseTarget()
  {
    std::regex target_pattern1{"(/[^\\?#]*)"};
    std::regex target_pattern2{"(/[^\\?#]*)\\?([^#]*)"};
    std::regex target_pattern3{"(/[^\\?#]*)\\?([^#]*)#(.*)"};
    std::regex target_pattern4{"(/[^\\?#]*)#(.*)"};
    std::smatch sm;

    if (std::regex_match(target, sm, target_pattern1))
    {
      path = sm[1];
    }
    else if (std::regex_match(target, sm, target_pattern2))
    {
      path  = sm[1];
      query = sm[2];
    }
    else if (std::regex_match(target, sm, target_pattern3))
    {
      path     = sm[1];
      query    = sm[2];
      fragment = sm[3];
    }
    else if (std::regex_match(target, sm, target_pattern4))
    {
      path     = sm[1];
      fragment = sm[2];
    }
    else
    {
      throw std::invalid_argument{"invalid target string: " + target};
    }
  }

  void ParseQuery()
  {
    if (query.empty())
    {
      return;
    }

    std::regex query_pattern{"([^=]+=[^&]*)(&[^=]+=[^&]*)*"};
    std::smatch sm;

    if (std::regex_match(query, sm, query_pattern))
    {
      std::vector<std::string> entries;
      boost::algorithm::split(entries, query, boost::is_any_of("&"));
      for (const auto& entry : entries)
      {
        std::vector<std::string> pair;
        boost::algorithm::split(pair, entry, boost::is_any_of("="));
        if (pair.size() == 2)
        {
          params.emplace(pair[0], pair[1]);
        }
        else if (pair.size() == 1)
        {
          params.emplace(pair[0], std::string{});
        }
      }
    }
    else
    {
      throw std::invalid_argument{"invalid query string: " + query};
    }
  }

  static std::string Unescape(const std::string& in)
  {
    std::string out;
    out.reserve(in.size());
    for (std::size_t i = 0; i < in.size(); ++i)
    {
      switch (in[i])
      {
      case '%':
        if (i + 3 <= in.size())
        {
          unsigned int value = 0;
          for (std::size_t j = i + 1; j < i + 3; ++j)
          {
            switch (in[j])
            {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
              value += in[j] - '0';
              break;
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
              value += in[j] - 'a' + 10;
              break;
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
              value += in[j] - 'A' + 10;
              break;
            default:
              throw std::invalid_argument{"invalid url: " + in};
            }
            if (j == i + 1)
              value <<= 4;
          }
          out += static_cast<char>(value);
          i += 2;
        }
        else
          throw std::invalid_argument{"invalid url: " + in};
        break;
      case '-':
      case '_':
      case '.':
      case '!':
      case '~':
      case '*':
      case '\'':
      case '(':
      case ')':
      case ':':
      case '@':
      case '&':
      case '=':
      case '+':
      case '$':
      case ',':
      case '/':
      case ';':
        out += in[i];
        break;
      default:
        if (!std::isalnum(in[i]))
          throw std::invalid_argument{"invalid url: " + in};
        out += in[i];
        break;
      }
    }
    return out;
  }

  auto ToString() const
  {
    return "protocol: " + protocol + ", host: " + host +
           ", port: " + std::to_string(port) + ", target: " + target;
  }

  std::string url;
  std::string protocol;
  std::string host;
  unsigned short port;
  std::string target;
  std::string path;
  std::string query;
  std::string fragment;
  std::map<std::string, std::string> params;
};

} // namespace biger::common::net

#endif
