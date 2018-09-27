#include <string>
#include <vector>

//bool getCurrentCPUUsage(int *pTotalCPUUsage, int *pCurrentProcessUsage);
extern "C" int getCPUSpeedMHz();
extern "C" int getOSName2(std::string &output, int outSize);
extern "C" int getOSName1(std::wstring &output, int outSize);
extern "C" int getOSName(char *output, int outSize);