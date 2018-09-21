#include "../include/HttpUtil.h"
#include "../include/JSONUtil.h"
#include <iostream>

// Command : g++ -g HttpUtil.cpp TCPClient.cpp CommonUtil.cpp -std=c++11
// Command : g++ -DWINDOWS=\"1\" -g HttpUtil.cpp TCPClient.cpp CommonUtil.cpp -std=c++11 -lws2_32

std::string HttpUtil::fetchHTTPGetResponse(std::string server, int port, std::string path, std::string contentType)
{
    if (!tcp.setup(server, port))
    {
        tcp.exit();
        return "";
    }

    stringstream ss;

    ss << "GET " + path + " HTTP/1.1\r\n"
       << "Host: \r\n"
       << "Accept: " + contentType + "\r\n"
       << "\r\n\r\n";

    string request = ss.str();
    string response = "";
    if (tcp.Send(request))
    {
        string response = tcp.receiveAll();
    }
    tcp.exit();
    return response;
}

std::string HttpUtil::fetchHTTPPostResponse(std::string server, int port, std::string path, std::string contentType, std::string requestBodyJSONStr)
{
    if (!tcp.setup(server, port))
    {
        tcp.exit();
        return "";
    }

    stringstream ss, tmp;

    tmp << strlen(requestBodyJSONStr.c_str());

    ss << "POST " + path + " HTTP/1.1\r\n"
       //<< "Content-Type: application/x-www-form-urlencoded\r\n"
       << "Content-Type: application/json\r\n"
       << "Host: localhost\r\n"
       << "Accept: " + contentType + "\r\n"
       << "Content-Length: " << tmp.str() << "\r\n"
       << "\r\n"
       << requestBodyJSONStr;

    string request = ss.str();

    // cout << request << endl;

    string response = "";
    if (tcp.Send(request))
    {
        string response = tcp.receiveAll();
    }
    tcp.exit();
    return response;
}

void HttpUtil::parseCookie(HttpResponse *response, std::string cookieString)
{
    int prevIndx = 0, indx = 0, length = strlen(cookieString.c_str());
    while (indx >= 0 && prevIndx < length)
    {
        indx = cookieString.find(";", prevIndx);
        std::string line = cookieString.substr(prevIndx, (indx - prevIndx));
        std::string key, value;
        int matchFound = 0, pos;

        pos = line.find("=");
        if (pos > 0)
        {
            key = line.substr(0, pos);
            value = line.substr(pos + 1, (indx - prevIndx - pos - 1));
            CommonUtil::trim(key);
            CommonUtil::trim(value);
        }
        else
        {
            prevIndx = indx + 1;
            continue;
        }

        if (matchFound == 0 && (pos = key.find("SESSION")) >= 0)
        {
            response->sessionIdKey = key;
            response->sessionId = value;
            matchFound = 1;
        }
        if (matchFound == 0 && CommonUtil::caseInsensitiveCompare(key, "rememberMe"))
        {
            matchFound = 1;
        }
        if (matchFound == 0 && CommonUtil::caseInsensitiveCompare(key, "Max-Age"))
        {
            matchFound = 1;
        }
        if (matchFound == 0 && CommonUtil::caseInsensitiveCompare(key, "Path"))
        {
            matchFound = 1;
        }
        if (matchFound == 0 && CommonUtil::caseInsensitiveCompare(key, "Server"))
        {
            matchFound = 1;
        }
        prevIndx = indx + 1;
    }
}

HttpResponse HttpUtil::parseResponse(std::string responseString)
{
    HttpResponse httpResponse;
    int prevIndx = 0, indx = 0, length = strlen(responseString.c_str());
    int responseBodyPos = responseString.find("\r\n\r\n");
    while (indx >= 0 && prevIndx < length)
    {
        indx = responseString.find("\r\n", prevIndx);
        std::string line = responseString.substr(prevIndx, (indx - prevIndx));
        std::string key, value;
        int matchFound = 0, pos;

        pos = line.find(":");
        if (pos > 0)
        {
            key = line.substr(0, pos);
            value = line.substr(pos + 1, (indx - prevIndx - pos - 1));
            CommonUtil::trim(key);
            CommonUtil::trim(value);
        }
        else
        {
            if (matchFound == 0 && (pos = line.find("HTTP/1.1 ")) == 0)
            {
                value = line.substr(9, (indx - prevIndx - 9));
                pos = value.find(" ");
                value = value.substr(0, pos);
                sscanf(value.c_str(), "%d", &httpResponse.responseCode);
                matchFound = 1;
            }
            prevIndx = indx + 2;
            continue;
        }

        if (matchFound == 0 && CommonUtil::caseInsensitiveCompare(key, "Set-Cookie"))
        {
            parseCookie(&httpResponse, value);
            matchFound = 1;
        }
        if (matchFound == 0 && CommonUtil::caseInsensitiveCompare(key, "Content-Type"))
        {
            httpResponse.contentType = value;
            matchFound = 1;
        }
        if (matchFound == 0 && CommonUtil::caseInsensitiveCompare(key, "Content-Length"))
        {
            sscanf(value.c_str(), "%d", &httpResponse.contentLength);
            matchFound = 1;
        }
        if (matchFound == 0 && CommonUtil::caseInsensitiveCompare(key, "Server"))
        {
            httpResponse.server = value;
            matchFound = 1;
        }

        if (indx >= responseBodyPos)
        {
            break;
        }
        else
        {
            prevIndx = indx + 2;
        }
    }
    httpResponse.responseString = responseString.substr(responseBodyPos + 4, length - responseBodyPos - 4);
    return httpResponse;
}

int mainHttpUtil(int argc, char *argv[])
//int main(int argc, char *argv[])
{
    HttpUtil httpUtil;
    //string rec = httpUtil.fetchHTTPGetResponse("192.168.4.71", 2029, "/Login", "text/html");
    string rec = httpUtil.fetchHTTPPostResponse("192.168.4.71", 2029, "/rest/auth/login", "application/json", "{\"user\":{\"username\":\"admin\",\"password\":\"admin\"}}");
    //rec = httpUtil.fetchHTTPPostResponse("192.168.4.71", 2029, "/rest/auth/login", "application/json", "{\"user\":{\"username\":\"admin\",\"password\":\"admin\"}}");
    HttpResponse res = httpUtil.parseResponse(rec);
    if (rec != "")
    {
        cout << "Server Response:" << res.server << endl;

        std::wstring wsTmp(res.responseString.begin(), res.responseString.end());
        std::cout << wsTmp.c_str() << endl;
        JSONValue *json = JSON::Parse(wsTmp.c_str());
        std::cout << json << endl;

        std::string status = JSONUtil::getChildAsString(json, (wchar_t *)(CommonUtil::toWString("message").c_str()));
        std::cout << status << endl;
        if (CommonUtil::caseInsensitiveCompare(status, "Success"))
        {
            return true;
        }
    }
    return 0;
}