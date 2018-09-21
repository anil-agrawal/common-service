#include "../include/JSONUtil.h"
#include <iostream>
#include <string>

//Ref : https://github.com/MJPA/SimpleJSON

// Command : g++ -g JSONUtil.cpp JSONValue.cpp JSON.cpp -std=c++11
// Command : g++ -DWINDOWS=\"1\" -g JSONUtil.cpp JSONValue.cpp JSON.cpp -std=c++11

std::string JSONUtil::getChildAsString(JSONValue *json, wchar_t *key)
{
    std::wstring value = json->Child(key)->AsString();
    std::string valueStr(value.begin(), value.end());
    return valueStr;
}

int mainJSONUtil()
//int main()
{
    JSONValue *json = JSON::Parse("{\"name\":\"anil\"}");
    std::string str = "name";
    std::wstring wstr;
    std::copy(str.begin(), str.end(), wstr.begin());
    std::cout << JSONUtil::getChildAsString(json, (wchar_t *)wstr.c_str()) << std::endl;
    return 0;
}