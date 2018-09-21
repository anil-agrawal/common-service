
#include "../../include/winservice/ServiceBaseImpl.h"

CServiceBaseImpl::CServiceBaseImpl(LPSTR pszServiceName,
                                   BOOL fCanStop,
                                   BOOL fCanShutdown,
                                   BOOL fCanPauseContinue)
    : CServiceBase(pszServiceName, fCanStop, fCanShutdown, fCanPauseContinue), m_bDefaultDontShowUIStatus(FALSE)
{
    CommonUtil::log_message("MyTestService Constructor");
}

CServiceBaseImpl::~CServiceBaseImpl(void)
{
    CommonUtil::log_message("MyTestService Destructor");
}

ServiceProcess *CServiceBaseImpl::serviceProcess = NULL;

DWORD WINAPI CServiceBaseImpl::ServiceWorkerThread(void *lpParam)
{
    int counter = 0;
    char msg[100] = {0};
    //  Periodically check if the service has been requested to stop
    while (0) //while (WaitForSingleObject(m_hStoppedEvent, 0) != WAIT_OBJECT_0)
    {
        sprintf(msg, "Inside Service. Running Counter : %d \n", counter++);
        CommonUtil::log_message(msg);
        Sleep(3000);
    }
    sprintf(msg, "Inside Service. Running Counter : %d \n", counter++);
    CommonUtil::log_message(msg);
    serviceProcess->run();
    return ERROR_SUCCESS;
}

void CServiceBaseImpl::OnStart(DWORD /*dwArgc*/, LPSTR * /*lpszArgv*/)
{

    char msg[255] = {0};
    WriteEventLogEntry("MyTestService in OnStart", EVENTLOG_INFORMATION_TYPE);

    CommonUtil::log_message("Starting My Test Service");
    CommonUtil::log_message(" Service started");
    HANDLE hThread = CreateThread(NULL, 0, ServiceWorkerThread, NULL, 0, NULL);
    CommonUtil::log_message(" Service started");
}

void CServiceBaseImpl::OnStop()
{
    WriteEventLogEntry("MyTestService in OnStop", EVENTLOG_INFORMATION_TYPE);
    CommonUtil::log_message("Stopping My Test Service");
}

void CServiceBaseImpl::OnShutdown()
{
    WriteEventLogEntry("MyTestService in OnShutdown", EVENTLOG_INFORMATION_TYPE);
    CommonUtil::log_message("OnShutdown My Test Service");
    this->OnStop();
}