
#include "../../include/winservice/ServiceInstaller.h"

void CServiceInstaller::InstallService(LPSTR pszServiceName,
                                       LPSTR pszDisplayName,
                                       LPSTR pszServiceDesc,
                                       DWORD dwStartType,
                                       LPSTR pszDependencies,
                                       LPSTR /*pszAccount*/,
                                       LPSTR pszPassword)
{
    char szPath[MAX_PATH]; //= "D:\\learn\\bin\\a.exe";
    SC_HANDLE schSCManager = NULL;
    SC_HANDLE schService = NULL;
    SERVICE_DESCRIPTION description = {pszServiceDesc};

    char msg[255] = {0};

    int nSize = GetModuleFileName(NULL, szPath, MAX_PATH);
    if (nSize == 0)
    {
        printf("GetModuleFileName failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    sprintf(msg, "Executable path : %s \n", szPath);
    CommonUtil::log_message(msg);
    // Open the local default service control manager database
    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE);
    if (schSCManager == NULL)
    {
        sprintf(msg, "OpenSCManager failed w/err 0x%08lx\n", GetLastError());
        CommonUtil::log_message(msg);
        goto Cleanup;
    }
    else
    {
        CommonUtil::log_message("OpenSCManager successfull\n");
    }
    // Install the service into SCM by calling CreateService
    schService = CreateService(
        schSCManager,              // SCManager database
        pszServiceName,            // Name of service
        pszDisplayName,            // Name to display
        SERVICE_ALL_ACCESS,        // Desired access
        SERVICE_WIN32_OWN_PROCESS, // Service type
        dwStartType,               // Service start type
        SERVICE_ERROR_NORMAL,      // Error control type
        szPath,                    // Service's binary
        NULL,                      // No load ordering group
        NULL,                      // No tag identifier
        pszDependencies,           // Dependencies
        NULL /*pszAccount*/,       // Service running account : Using NULL to run as LocalSystem
        pszPassword                // Password of the account
    );
    if (schService == NULL)
    {
        sprintf(msg, "CreateService failed w/err 0x%08lx\n", GetLastError());
        CommonUtil::log_message(msg);
        goto Cleanup;
    }
    else
    {
        CommonUtil::log_message("CreateService successfull\n");
    }
    sprintf(msg, "%s is installed.\n", pszServiceName);
    CommonUtil::log_message(msg);

    //! set service description
    ChangeServiceConfig2(schService, SERVICE_CONFIG_DESCRIPTION, &description);

Cleanup:
    // Centralized cleanup for all allocated resources.
    if (schSCManager)
    {
        CloseServiceHandle(schSCManager);
        schSCManager = NULL;
    }
    if (schService)
    {
        CloseServiceHandle(schService);
        schService = NULL;
    }
}

void CServiceInstaller::UninstallService(LPSTR pszServiceName)
{
    SC_HANDLE schSCManager = NULL;
    SC_HANDLE schService = NULL;
    SERVICE_STATUS ssSvcStatus = {};
    char msg[255] = {0};

    // Open the local default service control manager database
    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (schSCManager == NULL)
    {
        sprintf(msg, "OpenSCManager failed w/err 0x%08lx\n", GetLastError());
        CommonUtil::log_message(msg);
        goto Cleanup;
    }

    // Open the service with delete, stop, and query status permissions
    schService = OpenService(schSCManager, pszServiceName, SERVICE_STOP | SERVICE_QUERY_STATUS | DELETE);
    if (schService == NULL)
    {
        sprintf(msg, "OpenService failed w/err 0x%08lx\n", GetLastError());
        CommonUtil::log_message(msg);
        goto Cleanup;
    }

    // Try to stop the service
    if (ControlService(schService, SERVICE_CONTROL_STOP, &ssSvcStatus))
    {
        sprintf(msg, "Stopping %s.", pszServiceName);
        CommonUtil::log_message(msg);
        Sleep(1000);

        while (QueryServiceStatus(schService, &ssSvcStatus))
        {
            if (ssSvcStatus.dwCurrentState == SERVICE_STOP_PENDING)
            {
                CommonUtil::log_message(".");
                Sleep(1000);
            }
            else
                break;
        }

        if (ssSvcStatus.dwCurrentState == SERVICE_STOPPED)
        {
            sprintf(msg, "\n%s is stopped.\n", pszServiceName);
            CommonUtil::log_message(msg);
        }
        else
        {
            sprintf(msg, "\n%s is failed.\n", pszServiceName);
            CommonUtil::log_message(msg);
        }
    }

    // Now remove the service by calling DeleteService.
    if (!DeleteService(schService))
    {
        sprintf(msg, "DeleteService failed w/err 0x%08lx\n", GetLastError());
        CommonUtil::log_message(msg);
        goto Cleanup;
    }
    sprintf(msg, "%s is removed.\n", pszServiceName);
    CommonUtil::log_message(msg);

Cleanup:
    // Centralized cleanup for all allocated resources.
    if (schSCManager)
    {
        CloseServiceHandle(schSCManager);
        schSCManager = NULL;
    }
    if (schService)
    {
        CloseServiceHandle(schService);
        schService = NULL;
    }
}

void CServiceInstaller::RunService(LPSTR pszServiceName)
{
    SC_HANDLE schSCManager = NULL;
    SC_HANDLE schService = NULL;
    SERVICE_STATUS_PROCESS ssStatus;
    DWORD dwOldCheckPoint;
    DWORD dwStartTickCount;
    DWORD dwWaitTime;
    DWORD dwBytesNeeded;
    char msg[255] = {0};

    // Open the local default service control manager database
    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (schSCManager == NULL)
    {
        sprintf(msg, "OpenSCManager failed w/err 0x%08lx\n", GetLastError());
        CommonUtil::log_message(msg);
        goto Cleanup;
    }

    // Open the service with delete, stop, and query status permissions
    schService = OpenService(schSCManager, pszServiceName, SERVICE_START | SERVICE_QUERY_STATUS);
    if (schService == NULL)
    {
        sprintf(msg, "OpenService failed w/err 0x%08lx\n", GetLastError());
        CommonUtil::log_message(msg);
        goto Cleanup;
    }

    if (!StartService(schService, // handle to service
                      0,          // number of arguments
                      NULL))
    {
        sprintf(msg, "StartService failed w/err 0x%08lx\n", GetLastError());
        CommonUtil::log_message(msg);
        goto Cleanup;
    }

    // Check the status until the service is no longer start pending.

    if (!QueryServiceStatusEx(
            schService,                     // handle to service
            SC_STATUS_PROCESS_INFO,         // info level
            (LPBYTE)(&ssStatus),            // address of structure
            sizeof(SERVICE_STATUS_PROCESS), // size of structure
            &dwBytesNeeded))                // if buffer too small
    {
        sprintf(msg, "QueryServiceStatusEx failed w/err 0x%08lx\n", GetLastError());
        CommonUtil::log_message(msg);
        goto Cleanup;
    }

    // Save the tick count and initial checkpoint.

    dwStartTickCount = GetTickCount();
    dwOldCheckPoint = ssStatus.dwCheckPoint;

    while (ssStatus.dwCurrentState == SERVICE_START_PENDING)
    {
        // Do not wait longer than the wait hint. A good interval is
        // one tenth the wait hint, but no less than 1 second and no
        // more than 10 seconds.

        dwWaitTime = ssStatus.dwWaitHint / 10;

        if (dwWaitTime < 1000)
            dwWaitTime = 1000;
        else if (dwWaitTime > 10000)
            dwWaitTime = 10000;

        ::Sleep(dwWaitTime);

        // Check the status again.

        if (!QueryServiceStatusEx(
                schService,                     // handle to service
                SC_STATUS_PROCESS_INFO,         // info level
                (LPBYTE)(&ssStatus),            // address of structure
                sizeof(SERVICE_STATUS_PROCESS), // size of structure
                &dwBytesNeeded))                // if buffer too small
        {
            sprintf(msg, "QueryServiceStatusEx failed w/err 0x%08lx\n", GetLastError());
            CommonUtil::log_message(msg);
            break;
        }

        if (ssStatus.dwCheckPoint > dwOldCheckPoint)
        {
            // The service is making progress.
            dwStartTickCount = GetTickCount();
            dwOldCheckPoint = ssStatus.dwCheckPoint;
        }
        else
        {
            if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint)
            {
                // No progress made within the wait hint
                break;
            }
        }
    }

    if (ssStatus.dwCurrentState == SERVICE_RUNNING)
    {
        CommonUtil::log_message("StartService SUCCESS.\n");
    }
    else
    {
        CommonUtil::log_message("\nService not started. \n");
        sprintf(msg, "  Current State: %d\n", ssStatus.dwCurrentState);
        CommonUtil::log_message(msg);
        sprintf(msg, "  Exit Code: %d\n", ssStatus.dwWin32ExitCode);
        CommonUtil::log_message(msg);
        sprintf(msg, "  Service Specific Exit Code: %d\n", ssStatus.dwServiceSpecificExitCode);
        CommonUtil::log_message(msg);
        sprintf(msg, "  Check Point: %d\n", ssStatus.dwCheckPoint);
        CommonUtil::log_message(msg);
        sprintf(msg, "  Wait Hint: %d\n", ssStatus.dwWaitHint);
        CommonUtil::log_message(msg);
    }

Cleanup:
    // Centralized cleanup for all allocated resources.
    if (schSCManager)
    {
        CloseServiceHandle(schSCManager);
        schSCManager = NULL;
    }
    if (schService)
    {
        CloseServiceHandle(schService);
        schService = NULL;
    }
}

void CServiceInstaller::StopService(LPSTR pszServiceName)
{
    SC_HANDLE schSCManager = NULL;
    SC_HANDLE schService = NULL;
    SERVICE_STATUS ssSvcStatus = {};
    char msg[255];

    // Open the local default service control manager database
    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (schSCManager == NULL)
    {
        sprintf(msg, "OpenSCManager failed w/err 0x%08lx\n", GetLastError());
        CommonUtil::log_message(msg);
        goto Cleanup;
    }

    // Open the service with delete, stop, and query status permissions
    schService = OpenService(schSCManager, pszServiceName, SERVICE_STOP | SERVICE_QUERY_STATUS);
    if (schService == NULL)
    {
        sprintf(msg, "OpenService failed w/err 0x%08lx\n", GetLastError());
        CommonUtil::log_message(msg);
        goto Cleanup;
    }

    // Try to stop the service
    if (ControlService(schService, SERVICE_CONTROL_STOP, &ssSvcStatus))
    {
        sprintf(msg, "Stopping %s.", pszServiceName);
        CommonUtil::log_message(msg);
        Sleep(1000);

        while (QueryServiceStatus(schService, &ssSvcStatus))
        {
            if (ssSvcStatus.dwCurrentState == SERVICE_STOP_PENDING)
            {
                CommonUtil::log_message(".");
                Sleep(1000);
            }
            else
                break;
        }

        if (ssSvcStatus.dwCurrentState == SERVICE_STOPPED)
        {
            sprintf(msg, "\n%s is stopped.\n", pszServiceName);
            CommonUtil::log_message(msg);
        }
        else
        {
            sprintf(msg, "\n%s failed to stop.\n", pszServiceName);
            CommonUtil::log_message(msg);
        }
    }

Cleanup:
    // Centralized cleanup for all allocated resources.
    if (schSCManager)
    {
        CloseServiceHandle(schSCManager);
        schSCManager = NULL;
    }
    if (schService)
    {
        CloseServiceHandle(schService);
        schService = NULL;
    }
}