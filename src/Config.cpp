#include "../include/CommonUtil.h"
#include "../include/Config.h"
#include <fstream>
#include <iostream>
#include <stdio.h>

// #define TEST_ENV /* uncomment this line to use it for TEST_ENV.*/

using namespace std;

Config::Config()
{
    Config::readConfig();
}

void Config::readConfig()
{
    std::string line;
    std::string filePath = CommonUtil::getCurrentWorkingDir();
#ifdef WINDOWS
    filePath += "\\service\\conf\\win-wrapper.conf";
#else
    filePath += "/service/conf/nix-wrapper.conf";
#endif

    CommonUtil::log_message(filePath.c_str());
    std::ifstream infile(filePath.c_str());
    configMap.insert(pair<std::string, std::string>("java.additional", ""));
    configMap.insert(pair<std::string, std::string>("app.parameter", ""));
    while (std::getline(infile, line))
    {
        CommonUtil::trim(line);
        //std::cout << line << endl;
        int tmp = line.find("=");
        std::string value;
        if (tmp == -1 || line.size() <= tmp)
        {
            continue;
        }
        value = line.substr(tmp + 1);
        CommonUtil::trim(value);
        if (line.find("wrapper.java.additional") == 0)
        {
            //std::cout << "wrapper.java.additional : " << value << endl;
            value = configMap.find("java.additional")->second + " " + value;
            configMap.erase("java.additional");
            configMap.insert(pair<std::string, std::string>("java.additional", value));
        }
        else if (line.find("wrapper.app.parameter") == 0)
        {
            //std::cout << "wrapper.app.parameter : " << value << endl;
            value = configMap.find("app.parameter")->second + " " + value;
            configMap.erase("app.parameter");
            configMap.insert(pair<std::string, std::string>("app.parameter", value));
        }
        else if (ifConfigFileHasKey(line, "wrapper.java.command"))
        {
            //std::cout << "wrapper.java.command : " << value << endl;
            configMap.insert(pair<std::string, std::string>("java.command", value));
        }
        else if (ifConfigFileHasKey(line, "wrapper.java.app.jar"))
        {
            //std::cout << "wrapper.java.app.jar : " << value << endl;
            configMap.insert(pair<std::string, std::string>("java.app.jar", value));
        }
        else if (ifConfigFileHasKey(line, "wrapper.ntservice.name"))
        {
            //std::cout << "wrapper.ntservice.name : " << value << endl;
            configMap.insert(pair<std::string, std::string>("ntservice.name", value));
        }
        else if (ifConfigFileHasKey(line, "wrapper.ntservice.displayname"))
        {
            //std::cout << "wrapper.ntservice.displayname : " << value << endl;
            configMap.insert(pair<std::string, std::string>("ntservice.displayname", value));
        }
        else if (ifConfigFileHasKey(line, "wrapper.ntservice.description"))
        {
            //std::cout << "wrapper.ntservice.description : " << value << endl;
            configMap.insert(pair<std::string, std::string>("ntservice.description", value));
        }
        else if (ifConfigFileHasKey(line, "wrapper.daemon.pid.dir"))
        {
            //std::cout << "wrapper.ntservice.description : " << value << endl;
            configMap.insert(pair<std::string, std::string>("daemon.pid.dir", value));
        }
        else if (ifConfigFileHasKey(line, "wrapper.logfile"))
        {
            //std::cout << "logfile : " << value << endl;
            configMap.insert(pair<std::string, std::string>("logfile", value));
        }
        else if (ifConfigFileHasKey(line, "wrapper.working.dir"))
        {
            //std::cout << "working.dir : " << value << endl;
            configMap.insert(pair<std::string, std::string>("working.dir", value));
        }
        else if (ifConfigFileHasKey(line, "wrapper.tmp.path"))
        {
            //std::cout << "tmp.path : " << value << endl;
            configMap.insert(pair<std::string, std::string>("tmp.path", value));
        }
        else if (ifConfigFileHasKey(line, "wrapper.startup.timeout"))
        {
            //std::cout << "startup.timeout : " << value << endl;
            configMap.insert(pair<std::string, std::string>("startup.timeout", value));
        }
        else if (ifConfigFileHasKey(line, "wrapper.shutdown.timeout"))
        {
            //std::cout << "shutdown.timeout : " << value << endl;
            configMap.insert(pair<std::string, std::string>("shutdown.timeout", value));
        }
        else if (ifConfigFileHasKey(line, "wrapper.ping.pidCheckInterval"))
        {
            //std::cout << "ping.pidCheckInterval : " << value << endl;
            configMap.insert(pair<std::string, std::string>("ping.pidCheckInterval", value));
        }
        else if (ifConfigFileHasKey(line, "wrapper.ping.healthCheckInterval"))
        {
            //std::cout << "ping.healthCheckInterval : " << value << endl;
            configMap.insert(pair<std::string, std::string>("ping.healthCheckInterval", value));
        }
        else if (ifConfigFileHasKey(line, "wrapper.ping.healthCheckReceiveTimeout"))
        {
            //std::cout << "ping.healthCheckReceiveTimeout : " << value << endl;
            configMap.insert(pair<std::string, std::string>("ping.healthCheckReceiveTimeout", value));
        }
        else if (ifConfigFileHasKey(line, "wrapper.ping.healthCheckRetryAttemptCount"))
        {
            //std::cout << "ping.healthCheckRetryAttemptCount : " << value << endl;
            configMap.insert(pair<std::string, std::string>("ping.healthCheckRetryAttemptCount", value));
        }
        else if (ifConfigFileHasKey(line, "wrapper.app.server"))
        {
            //std::cout << "app.server : " << value << endl;
            configMap.insert(pair<std::string, std::string>("app.server", value));
        }
        else if (ifConfigFileHasKey(line, "wrapper.app.port"))
        {
            //std::cout << "app.port : " << value << endl;
            configMap.insert(pair<std::string, std::string>("app.port", value));
        }
        else if (ifConfigFileHasKey(line, "wrapper.app.healthAPI.requestMethod"))
        {
            //std::cout << "app.healthAPI.requestMethod : " << value << endl;
            configMap.insert(pair<std::string, std::string>("app.healthAPI.requestMethod", value));
        }
        else if (ifConfigFileHasKey(line, "wrapper.ping.enablePIDCheck"))
        {
            //std::cout << "ping.enablePIDCheck : " << value << endl;
            configMap.insert(pair<std::string, std::string>("ping.enablePIDCheck", value));
        }
        else if (ifConfigFileHasKey(line, "wrapper.ping.enableHealthCheck"))
        {
            //std::cout << "ping.enableHealthCheck : " << value << endl;
            configMap.insert(pair<std::string, std::string>("ping.enableHealthCheck", value));
        }
        else if (ifConfigFileHasKey(line, "wrapper.app.healthAPI.requestURI"))
        {
            //std::cout << "app.healthAPI.requestURI : " << value << endl;
            configMap.insert(pair<std::string, std::string>("app.healthAPI.requestURI", value));
        }
        else if (ifConfigFileHasKey(line, "wrapper.app.healthAPI.requestBody"))
        {
            //std::cout << "app.healthAPI.requestBody : " << value << endl;
            configMap.insert(pair<std::string, std::string>("app.healthAPI.requestBody", value));
        }
        else if (ifConfigFileHasKey(line, "wrapper.app.healthAPI.requestContentType"))
        {
            //std::cout << "app.healthAPI.requestContentType : " << value << endl;
            configMap.insert(pair<std::string, std::string>("app.healthAPI.requestContentType", value));
        }
        else if (ifConfigFileHasKey(line, "wrapper.app.healthAPI.responseContentType"))
        {
            //std::cout << "app.healthAPI.responseContentType : " << value << endl;
            configMap.insert(pair<std::string, std::string>("app.healthAPI.responseContentType", value));
        }
        else if (ifConfigFileHasKey(line, "wrapper.app.healthAPI.responseHealthKey"))
        {
            //std::cout << "app.healthAPI.responseHealthKey : " << value << endl;
            configMap.insert(pair<std::string, std::string>("app.healthAPI.responseHealthKey", value));
        }
        else if (ifConfigFileHasKey(line, "wrapper.app.healthAPI.responseExpectedValue"))
        {
            //std::cout << "app.healthAPI.responseExpectedValue : " << value << endl;
            configMap.insert(pair<std::string, std::string>("app.healthAPI.responseExpectedValue", value));
        }
        else if (ifConfigFileHasKey(line, "wrapper.enableDebuggingLog"))
        {
            //std::cout << "enableDebuggingLog : " << value << endl;
            configMap.insert(pair<std::string, std::string>("enableDebuggingLog", value));
        }
        else if (ifConfigFileHasKey(line, "wrapper.daemon.forkAsDaemon"))
        {
            //std::cout << "daemon.forkAsDaemon : " << value << endl;
            configMap.insert(pair<std::string, std::string>("daemon.forkAsDaemon", value));
        }
        else
        {
            // std::cout << "other : " << value << endl;
        }
    }
    //std::cout << "java.additional ==> " << configMap.find("java.additional")->second << endl;
    //std::cout << "app.parameter ==> " << configMap.find("app.parameter")->second << endl;
    //std::cout << "java.command ==> " << configMap.find("java.command")->second << endl;
    //std::cout << "java.app.jar ==> " << configMap.find("java.app.jar")->second << endl;
}

bool Config::ifConfigFileHasKey(std::string line, std::string key)
{
    if (line.find(key + " ") == 0 || line.find(key + "=") == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

std::string Config::findMapValueOrExit(std::string key)
{
    if (configMap.count(key) < 1)
    {
        std::string msg = "No value provided for mandatory field : wrapper." + key + " in configuration file.";
        CommonUtil::log_message(msg.c_str());
        exit(1);
    }
    return configMap.find(key)->second;
}

std::string Config::getDaemonPIDDir()
{
    if (configMap.count("daemon.pid.dir") > 0)
    {
        return configMap.find("daemon.pid.dir")->second;
    }
    else
    {
        return "/var/run";
    }
}

std::string Config::getJar()
{
    return findMapValueOrExit("java.app.jar");
}

std::string Config::getJavaExecutable()
{
    return findMapValueOrExit("java.command");
}

std::string Config::getAppParam()
{
    return findMapValueOrExit("app.parameter");
}

std::string Config::getJVMArguments()
{
    return findMapValueOrExit("java.additional");
}

std::string Config::getServiceName()
{
    return findMapValueOrExit("ntservice.name");
}

std::string Config::getServiceDisplayName()
{
    return findMapValueOrExit("ntservice.displayname");
}

std::string Config::getServiceDiscription()
{
    return findMapValueOrExit("ntservice.description");
}

std::string Config::getPIDFileName()
{
    std::string serviceName = getServiceName();
    std::replace(serviceName.begin(), serviceName.end(), ' ', '_');
    return serviceName + ".pid";
}

std::string Config::getLockFileName()
{
    std::string serviceName = getServiceName();
    std::replace(serviceName.begin(), serviceName.end(), ' ', '_');
    return serviceName + ".lock";
}

std::string Config::fetchRunnableServiceCommand()
{
    std::string command;
#ifdef TEST_ENV
    command = CommonUtil::getCurrentWorkingDir() + "/test.exe";
#else
    command = getJavaExecutable() + " " + getJVMArguments() + " -jar " + getJar() + " " + getAppParam();
#endif
    CommonUtil::trim(command);
    return command;
}

std::string Config::fetchLogFile()
{
    return findMapValueOrExit("logfile");
}

std::string Config::getWrapperWorkingDir()
{
    return findMapValueOrExit("working.dir");
}

std::string Config::getWrapperTempDir()
{
    return findMapValueOrExit("tmp.path");
}

int Config::fetchProcessPIDCheckInterval()
{
    std::string pidCheckIntervalStr = findMapValueOrExit("ping.pidCheckInterval");
    stringstream pidCheckIntervalStream(pidCheckIntervalStr);
    int interval = 3; //In Seconds
    pidCheckIntervalStream >> interval;
    return interval;
}

int Config::fetchProcessHealthCheckInterval()
{
    std::string healthCheckIntervalStr = findMapValueOrExit("ping.healthCheckInterval");
    stringstream healthCheckIntervalStream(healthCheckIntervalStr);
    int interval = 3; //In Seconds
    healthCheckIntervalStream >> interval;
    return interval;
}

int Config::fetchHealthCheckRetryAttemptCount()
{
    std::string str = findMapValueOrExit("ping.healthCheckRetryAttemptCount");
    stringstream strStream(str);
    int num = 3;
    strStream >> num;
    return num;
}

int Config::fetchApplicationStartupTimeout()
{
    std::string str = findMapValueOrExit("startup.timeout");
    stringstream strStream(str);
    int num = 3; //In Seconds
    strStream >> num;
    return num;
}

int Config::fetchApplicationShutdownTimeout()
{
    std::string str = findMapValueOrExit("shutdown.timeout");
    stringstream strStream(str);
    int num = 3; //In Seconds
    strStream >> num;
    return num;
}

bool Config::enableDebuggingLog()
{
    std::string enableDebuggingLogStr = findMapValueOrExit("enableDebuggingLog");
    bool enableDebuggingLog = CommonUtil::caseInsensitiveCompare(enableDebuggingLogStr, "true");
    return enableDebuggingLog;
}

bool Config::forkWrapperAsDaemon()
{
    std::string forkAsDaemonStr = findMapValueOrExit("daemon.forkAsDaemon");
    bool forkAsDaemon = CommonUtil::caseInsensitiveCompare(forkAsDaemonStr, "true");
    return forkAsDaemon;
}

int Config::fetchHealthCheckReceiveTimeout()
{
    std::string str = findMapValueOrExit("ping.healthCheckReceiveTimeout");
    stringstream strStream(str);
    int num = 3; //In Seconds
    strStream >> num;
    return num;
}

int Config::fetchAppPort()
{
    std::string str = findMapValueOrExit("app.port");
    stringstream strStream(str);
    int num = 3;
    strStream >> num;
    return num;
}

std::string Config::fetchAppServer()
{
    return findMapValueOrExit("app.server");
}

std::string Config::fetchAppHealthAPIRequestURI()
{
    return findMapValueOrExit("app.healthAPI.requestURI");
}

std::string Config::fetchAppHealthAPIRequestBody()
{
    return findMapValueOrExit("app.healthAPI.requestBody");
}

std::string Config::fetchAppHealthAPIRequestContentType()
{
    return findMapValueOrExit("app.healthAPI.requestContentType");
}

std::string Config::fetchAppHealthAPIResponseContentType()
{
    return findMapValueOrExit("app.healthAPI.responseContentType");
}

std::string Config::fetchAppHealthAPIResponseHealthKey()
{
    return findMapValueOrExit("app.healthAPI.responseHealthKey");
}

std::string Config::fetchAppHealthAPIResponseExpectedValue()
{
    return findMapValueOrExit("app.healthAPI.responseExpectedValue");
}

bool Config::enableProcessPIDCheck()
{
    std::string boolStr = findMapValueOrExit("ping.enablePIDCheck");
    bool boolValue = CommonUtil::caseInsensitiveCompare(boolStr, "true");
    return boolValue;
}

bool Config::enableProcessHealthCheck()
{
    std::string boolStr = findMapValueOrExit("ping.enableHealthCheck");
    bool boolValue = CommonUtil::caseInsensitiveCompare(boolStr, "true");
    return boolValue;
}

std::string Config::fetchAppHealthAPIRequestMethod()
{
    return findMapValueOrExit("app.healthAPI.requestMethod");
}