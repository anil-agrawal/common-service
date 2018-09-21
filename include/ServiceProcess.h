#ifndef Service_Process_H
#define Service_Process_H

// #define WINDOWS /* uncomment this line to use it for windows.*/

#include "../include/HttpUtil.h"
#include "../include/Config.h"
//#include <thread>
#include <pthread.h>

#ifdef WINDOWS
#include <winsock2.h>
#include <windows.h>
#endif

struct ProcessHealthCheckData
{

  int appPort;
  std::string appServer;
  std::string requestBody;
  std::string requestContentType;
  std::string requestURI;
  std::string requestMethod;
  std::string responseContentTYpe;
  std::string responseKey;
  std::string responseValue;
};

class ServiceProcess
{

public:
  HttpUtil httpUtil;

public:
  Config config;

private:
  static int child_process_pid;
  static int child_process_std_out;
  static int child_process_std_in;

private:
  void terminateApplication(int applicationShutdownTimeout);

public:
  ServiceProcess();
  ~ServiceProcess();

public:
  void run();

private:
  void processMonitorThread();
  void healthMonitorThread();
  void logChildProcessStdOutThread();

private:
  static void *processMonitorThreadCallback(void *objInstance);
  static void *healthMonitorThreadCallback(void *objInstance);
  static void *logChildProcessStdOutThreadCallback(void *objInstance);

private:
  int launchProcessInSeparateDaemon(const char *command);
  int launchProcessInSeparateDaemonWithHandlers(const char *command, int *infp, int *outfp);

#ifdef WINDOWS
private:
  static BOOL WINAPI signal_handler_callback(DWORD dwType);
#else
private:
  static void signal_handler_callback(int sig);
#endif

public:
  void showSysInfo();

public:
  bool healthTest(ProcessHealthCheckData *healthCheckData);
};

#endif