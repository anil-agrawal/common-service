#ifndef HTTP_UTIL_H
#define HTTP_UTIL_H

#include "../include/TCPClient.h"

class HttpResponse
{
public:
  int responseCode;
  std::string contentType;
  std::string sessionId;
  std::string sessionIdKey;
  std::string responseString;
  std::string server;
  int contentLength;
};

class HttpUtil
{
private:
  TCPClient tcp;

public:
  std::string fetchHTTPGetResponse(std::string server, int port, std::string path, std::string contentType);
  std::string fetchHTTPPostResponse(std::string server, int port, std::string path, std::string contentType, std::string requestBodyJSONStr);
  HttpResponse parseResponse(std::string responseString);
  void parseCookie(HttpResponse *response, std::string cookieString);
};

#endif