#ifndef Config_H
#define Config_H

#include "../include/CommonUtil.h"
#include <map>

class Config
{

public:
  Config();

public:
  void readConfig();

public:
  std::string getJar();

public:
  std::string getJavaExecutable();

public:
  std::string getAppParam();

public:
  std::string getJVMArguments();

public:
  std::string getServiceName();

public:
  std::string getServiceDisplayName();

public:
  std::string getServiceDiscription();

public:
  std::string fetchRunnableServiceCommand();

public:
  std::string getWrapperWorkingDir();

public:
  std::string getWrapperTempDir();

public:
  std::string getDaemonPIDDir();

public:
  std::string fetchLogFile();

public:
  std::string getPIDFileName();

public:
  std::string getLockFileName();

public:
  bool enableDebuggingLog();

public:
  bool forkWrapperAsDaemon();

public:
  int fetchApplicationStartupTimeout();

public:
  int fetchApplicationShutdownTimeout();

public:
  bool enableProcessPIDCheck();

public:
  bool enableProcessHealthCheck();

public:
  int fetchProcessPIDCheckInterval();

public:
  int fetchProcessHealthCheckInterval();

public:
  int fetchHealthCheckRetryAttemptCount();

public:
  int fetchHealthCheckReceiveTimeout();

public:
  int fetchAppPort();
  std::string fetchAppServer();
  std::string fetchAppHealthAPIRequestURI();
  std::string fetchAppHealthAPIRequestMethod();
  std::string fetchAppHealthAPIRequestBody();
  std::string fetchAppHealthAPIRequestContentType();
  std::string fetchAppHealthAPIResponseContentType();
  std::string fetchAppHealthAPIResponseHealthKey();
  std::string fetchAppHealthAPIResponseExpectedValue();

private:
  std::string findMapValueOrExit(std::string key);

private:
  bool ifConfigFileHasKey(std::string line, std::string key);

private:
  std::map<std::string, std::string> configMap;
};

#endif