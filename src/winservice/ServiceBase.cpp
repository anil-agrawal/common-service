#include "../../include/winservice/ServiceBase.h"

CServiceBase::CServiceBase(LPSTR pszServiceName,
                           BOOL fCanStop,
                           BOOL fCanShutdown,
                           BOOL fCanPauseContinue)
{
    // Service name must be a valid string and cannot be NULL.
    m_name = (LPSTR)((pszServiceName == NULL) ? "" : pszServiceName);

    m_statusHandle = 0;

    // The service runs in its own process.
    m_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;

    // The service is starting.
    m_status.dwCurrentState = SERVICE_START_PENDING;

    // The accepted commands of the service.
    DWORD dwControlsAccepted = 0;
    if (fCanStop)
        dwControlsAccepted |= SERVICE_ACCEPT_STOP;
    if (fCanShutdown)
        dwControlsAccepted |= SERVICE_ACCEPT_SHUTDOWN;
    if (fCanPauseContinue)
        dwControlsAccepted |= SERVICE_ACCEPT_PAUSE_CONTINUE;
    m_status.dwControlsAccepted = dwControlsAccepted;

    m_status.dwWin32ExitCode = NO_ERROR;
    m_status.dwServiceSpecificExitCode = 0;
    m_status.dwCheckPoint = 0;
    m_status.dwWaitHint = 0;
}

CServiceBase::~CServiceBase(void)
{
}

// Initialize the singleton service instance.s
CServiceBase *CServiceBase::s_service = NULL;

BOOL CServiceBase::Run(CServiceBase &service)
{
    CommonUtil::log_message("inside CServiceBase::Run()");
    s_service = &service;

    SERVICE_TABLE_ENTRY serviceTable[] =
        {
            {service.m_name, ServiceMain},
            {NULL, NULL}};
    return StartServiceCtrlDispatcher(serviceTable);
}

void WINAPI CServiceBase::ServiceMain(DWORD dwArgc, LPSTR *pszArgv)
{
    assert(s_service != NULL);

    // Register the handler function for the service
    s_service->m_statusHandle = RegisterServiceCtrlHandler(
        s_service->m_name, ServiceCtrlHandler);
    if (s_service->m_statusHandle == 0)
    {
        throw GetLastError();
    }

    // Start the service.
    s_service->Start(dwArgc, pszArgv);
}

void WINAPI CServiceBase::ServiceCtrlHandler(DWORD dwCtrl)
{
    switch (dwCtrl)
    {
    case SERVICE_CONTROL_STOP:
        s_service->Stop();
        break;
    case SERVICE_CONTROL_PAUSE:
        s_service->Pause();
        break;
    case SERVICE_CONTROL_CONTINUE:
        s_service->Continue();
        break;
    case SERVICE_CONTROL_SHUTDOWN:
        s_service->Shutdown();
        break;
    case SERVICE_CONTROL_INTERROGATE:
        break;
    default:
        break;
    }
}

void CServiceBase::Start(DWORD dwArgc, LPSTR *pszArgv)
{
    try
    {
        SetServiceStatus(SERVICE_START_PENDING);
        OnStart(dwArgc, pszArgv);
        SetServiceStatus(SERVICE_RUNNING);
    }
    catch (DWORD dwError)
    {
        WriteErrorLogEntry("Service Start", dwError);
        SetServiceStatus(SERVICE_STOPPED, dwError);
    }
    catch (...)
    {
        WriteEventLogEntry("Service failed to start.", EVENTLOG_ERROR_TYPE);
        SetServiceStatus(SERVICE_STOPPED);
    }
}

void CServiceBase::OnStart(DWORD /*dwArgc*/, LPSTR * /*pszArgv*/)
{
}

void CServiceBase::Stop()
{
    DWORD dwOriginalState = m_status.dwCurrentState;
    try
    {
        SetServiceStatus(SERVICE_STOP_PENDING);
        OnStop();
        SetServiceStatus(SERVICE_STOPPED);
    }
    catch (DWORD dwError)
    {
        WriteErrorLogEntry("Service Stop", dwError);
        SetServiceStatus(dwOriginalState);
    }
    catch (...)
    {
        WriteEventLogEntry("Service failed to stop.", EVENTLOG_ERROR_TYPE);
        SetServiceStatus(dwOriginalState);
    }
}

void CServiceBase::OnStop()
{
}

void CServiceBase::Pause()
{
    try
    {
        SetServiceStatus(SERVICE_PAUSE_PENDING);
        OnPause();
        SetServiceStatus(SERVICE_PAUSED);
    }
    catch (DWORD dwError)
    {
        WriteErrorLogEntry("Service Pause", dwError);
        SetServiceStatus(SERVICE_RUNNING);
    }
    catch (...)
    {
        WriteEventLogEntry("Service failed to pause.", EVENTLOG_ERROR_TYPE);
        SetServiceStatus(SERVICE_RUNNING);
    }
}

void CServiceBase::OnPause()
{
}

void CServiceBase::Continue()
{
    try
    {
        SetServiceStatus(SERVICE_CONTINUE_PENDING);
        OnContinue();
        SetServiceStatus(SERVICE_RUNNING);
    }
    catch (DWORD dwError)
    {
        WriteErrorLogEntry("Service Continue", dwError);
        SetServiceStatus(SERVICE_PAUSED);
    }
    catch (...)
    {
        WriteEventLogEntry("Service failed to resume.", EVENTLOG_ERROR_TYPE);
        SetServiceStatus(SERVICE_PAUSED);
    }
}

void CServiceBase::OnContinue()
{
}

void CServiceBase::Shutdown()
{
    try
    {
        OnShutdown();
        SetServiceStatus(SERVICE_STOPPED);
    }
    catch (DWORD dwError)
    {
        WriteErrorLogEntry("Service Shutdown", dwError);
    }
    catch (...)
    {
        WriteEventLogEntry("Service failed to shut down.", EVENTLOG_ERROR_TYPE);
    }
}

void CServiceBase::OnShutdown()
{
}

void CServiceBase::SetServiceStatus(DWORD dwCurrentState,
                                    DWORD dwWin32ExitCode,
                                    DWORD dwWaitHint)
{
    static DWORD dwCheckPoint = 1;
    m_status.dwCurrentState = dwCurrentState;
    m_status.dwWin32ExitCode = dwWin32ExitCode;
    m_status.dwWaitHint = dwWaitHint;

    m_status.dwCheckPoint =
        ((dwCurrentState == SERVICE_RUNNING) ||
         (dwCurrentState == SERVICE_STOPPED))
            ? 0
            : dwCheckPoint++;

    ::SetServiceStatus(m_statusHandle, &m_status);
}

void CServiceBase::WriteEventLogEntry(const char *pszMessage, WORD wType)
{
    HANDLE hEventSource = NULL;
    LPCSTR lpszStrings[2] = {NULL, NULL};

    hEventSource = RegisterEventSource(NULL, m_name);
    if (hEventSource)
    {
        lpszStrings[0] = m_name;
        lpszStrings[1] = pszMessage;

        ReportEvent(hEventSource, // Event log handle
                    wType,        // Event type
                    0,            // Event category
                    0,            // Event identifier
                    NULL,         // No security identifier
                    2,            // Size of lpszStrings array
                    0,            // No binary data
                    lpszStrings,  // Array of strings
                    NULL          // No binary data
        );

        DeregisterEventSource(hEventSource);
    }
}

void CServiceBase::WriteErrorLogEntry(const char *pszFunction, DWORD dwError)
{
    char szMessage[260];
    snprintf(szMessage, strlen(szMessage), "%s failed w/err 0x%08lx", pszFunction, dwError);
    CommonUtil::log_message(szMessage);
    WriteEventLogEntry(szMessage, EVENTLOG_ERROR_TYPE);
}