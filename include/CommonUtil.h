#ifndef Common_Util_H
#define Common_Util_H

//#include <string>
#include <algorithm>
#include <sstream>
#include <sys/stat.h>
#include <libgen.h>
#ifdef WINDOWS
#include <csignal>
#include <direct.h>
#else
#include <unistd.h>
#include <signal.h>
#endif

class CommonUtil
{
public:
  CommonUtil();
  ~CommonUtil();

private:
  static int __static_init;
  static int static_init();

public:
  static void cleanUpResources();
  static void populateResources();
  static std::string LOG_FILE;
  static std::string CHILD_PROCESS_LOG_FILE;
  static FILE *LOG_FILE_HANDLE;
  static FILE *CHILD_PROCESS_LOG_FILE_HANDLE;
  static bool ENABLE_DEBUGGING_LOG;
  static void log_message(std::string filename, char *message);
  static void log_message(std::string filename, const char *message);
  static void log_message(char *message);
  static void log_message(const char *message);
  static void log_child_console(const char *message);
  static void debug_message(char *message);
  static void debug_message(const char *message);
  static std::string getCurrentTime();

public:
  static std::string getCurrentWorkingDir(void);

public:
  static std::string getComputerName();

public:
  static std::string getUserName();

public:
  static std::string getComputerName_env();

public:
  static std::string getUserName_env();

public:
  static int getPid();

public:
  static int getPid(std::string processName);

public:
  static bool killProcessWithId(int pid, int &exitCode);

public:
  static bool killProcessWithId(int pid);

public:
  static bool gracefullyKillProcessWithId(int pid, int &exitCode, void *signalHandler);

public:
  static bool gracefullyKillProcessWithId(int pid, void *signalHandler);

public:
  static bool ifProcessWithPIDExist(int pid);

public:
  static bool killProcessWithName(std::string processName, int &exitCode);

public:
  static bool killProcessWithName(std::string processName);

public:
  static int getPPid();

public:
  static void ltrim(std::string &s);

public:
  static void rtrim(std::string &s);

public:
  static void trim(std::string &s);

public:
  static bool caseInsensitiveCompare(std::string str1, std::string str2);

public:
  static void _sleep(int seconds);

public:
  static std::string toString(std::wstring wstr);
  static std::wstring toWString(std::string str);

};

#endif