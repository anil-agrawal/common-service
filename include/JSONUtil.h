#ifndef JSON_UTIL_H
#define JSON_UTIL_H

#include "../include/JSON.h"

class JSONUtil
{
  public:
    static std::string getChildAsString(JSONValue *json, wchar_t* key);
};

#endif