
#include "../../include/winservice/ServiceInstaller.h"
#include "../../include/winservice/ServiceBaseImpl.h"

// g++ WinService.cpp ServiceBase.cpp ServiceBaseImpl.cpp ServiceInstaller.cpp -DUNICODE -D_UNICODE -static-libgcc -static-libstdc++
// g++ ../src/winservice/WinService.cpp ../src/winservice/ServiceBase.cpp ../src/winservice/ServiceBaseImpl.cpp ../src/winservice/ServiceInstaller.cpp -DUNICODE -D_UNICODE -static-libgcc -static-libstdc++
// Internal name of the service

// #define SERVICE_START_TYPE SERVICE_AUTO_START
// #define SERVICE_DEPENDENCIES ""
// #define SERVICE_ACCOUNT "NT AUTHORITY\\LocalService"
// #define SERVICE_PASSWORD NULL

#define _stricmp strcmp

#ifndef strdup
char *strdup(const char *s)
{
    size_t slen = strlen(s);
    char *result = (char *)malloc(slen + 1);
    if (result == NULL)
    {
        return NULL;
    }

    memcpy(result, s, slen + 1);
    return result;
}
#endif

void RunProgram(ServiceProcess *serviceProcess, LPSTR serviceName)
{
    CServiceBaseImpl service(serviceName);
    service.serviceProcess = serviceProcess;
    //service.onStart();
    serviceProcess->run();
}

int main(int argc, char *argv[])
{
    int SERVICE_START_TYPE = SERVICE_AUTO_START;
    LPSTR SERVICE_DEPENDENCIES = strdup("");
    LPSTR SERVICE_ACCOUNT = strdup("NT AUTHORITY\\LocalService");
    LPSTR SERVICE_PASSWORD = NULL;

    ServiceProcess serviceProcess;
    LPSTR serviceName = strdup(serviceProcess.config.getServiceName().c_str());
    LPSTR serviceDescription = strdup(serviceProcess.config.getServiceDiscription().c_str());
    LPSTR serviceDisplayName = strdup(serviceProcess.config.getServiceDisplayName().c_str());

    CServiceInstaller serviceInstaller;

    if ((argc > 1) && ((*argv[1] == '-' || (*argv[1] == '/'))))
    {
        if (_stricmp("install", argv[1] + 1) == 0)
        {
            CommonUtil::log_message("Installing Service\n");
            serviceInstaller.InstallService(
                serviceName,          // Name of service
                serviceDisplayName,   // Name to display
                serviceDescription,   // Service description
                SERVICE_START_TYPE,   // Service start type
                SERVICE_DEPENDENCIES, // Dependencies
                SERVICE_ACCOUNT,      // Service running account
                SERVICE_PASSWORD      // Password of the account
            );
            CommonUtil::log_message("Installed Service\n");
        }
        else if (_stricmp("remove", argv[1] + 1) == 0)
        {
            CommonUtil::log_message("Removing Service\n");
            serviceInstaller.UninstallService(serviceName);
        }
        else if (_stricmp("start", argv[1] + 1) == 0)
        {
            CommonUtil::log_message("Starting Service\n");
            serviceInstaller.RunService(serviceName);
        }
        else if (_stricmp("stop", argv[1] + 1) == 0)
        {
            CommonUtil::log_message("Stopping Service\n");
            serviceInstaller.StopService(serviceName);
        }
        else if (_stricmp("run", argv[1] + 1) == 0)
        {
            CommonUtil::log_message("Running Service\n");
            RunProgram(&serviceProcess, serviceName);
        }
    }
    else
    {
        CommonUtil::log_message("Parameters:\n");
        CommonUtil::log_message(" -install  to install the service.\n");
        CommonUtil::log_message(" -remove   to remove the service.\n");
        CommonUtil::log_message(" -start    to start the service.\n");
        CommonUtil::log_message(" -stop     to stop the service.\n");
        CommonUtil::log_message(" -run      to run as a program.\n");

        CServiceBaseImpl service(serviceName);
        service.serviceProcess = &serviceProcess;
        if (!CServiceBase::Run(service))
        {
            char msg[100];
            sprintf(msg, "Service failed to run w/err 0x%08lx\n", GetLastError());
            CommonUtil::log_message(msg);
        }
    }
    return 0;
}