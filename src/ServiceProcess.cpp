#include "../include/ServiceProcess.h"
#include "../include/Config.h"
#include "../include/JSONUtil.h"
#include <fcntl.h>

// #define WINDOWS /* uncomment this line to use it for windows.*/

using namespace std;

ServiceProcess::ServiceProcess()
{
    CommonUtil::LOG_FILE = config.fetchLogFile() + ".service";
    CommonUtil::ENABLE_DEBUGGING_LOG = config.enableDebuggingLog();
    CommonUtil::populateResources();
    TCPClient::receiveTimeout = config.fetchHealthCheckReceiveTimeout();
    CommonUtil::log_message("Inside ServiceProcess Constructor");
}

ServiceProcess::~ServiceProcess()
{
    CommonUtil::cleanUpResources();
    CommonUtil::log_message("Inside ServiceProcess Desstructor");
}

int ServiceProcess::child_process_pid = 0;
int ServiceProcess::child_process_std_out = 0;
int ServiceProcess::child_process_std_in = 0;

void ServiceProcess::showSysInfo()
{
    try
    {
        char msg[255] = {0};

        CommonUtil::log_message("\n\n\n");

        sprintf(msg, "Service Name : %s ", config.getServiceDisplayName().c_str());
        CommonUtil::log_message(msg);

        sprintf(msg, "Service Discription : %s ", config.getServiceDiscription().c_str());
        CommonUtil::log_message(msg);

        sprintf(msg, "Service PID : %d ", CommonUtil::getPid());
        CommonUtil::log_message(msg);

        sprintf(msg, "Service Parent PID : %d ", CommonUtil::getPPid());
        CommonUtil::log_message(msg);

        sprintf(msg, "Service PID/Lock File Directory : %s ", config.getDaemonPIDDir().c_str());
        CommonUtil::log_message(msg);

        sprintf(msg, "Service PID File Name : %s ", config.getPIDFileName().c_str());
        CommonUtil::log_message(msg);

        sprintf(msg, "Service Lock File Name : %s ", config.getLockFileName().c_str());
        CommonUtil::log_message(msg);

        sprintf(msg, "Working Directory : %s ", CommonUtil::getCurrentWorkingDir().c_str());
        CommonUtil::log_message(msg);

        sprintf(msg, "Service Log File : %s ", config.fetchLogFile().c_str());
        CommonUtil::log_message(msg);

        sprintf(msg, "Java Path : %s ", config.getJavaExecutable().c_str());
        CommonUtil::log_message(msg);

        // sprintf(msg, "JVM Args : %s ", config.getJVMArguments().c_str());
        // CommonUtil::log_message(msg);

        sprintf(msg, "Application Path : %s ", config.getJar().c_str());
        CommonUtil::log_message(msg);

        // sprintf(msg, "Application Args : %s ", config.getAppParam().c_str());
        // CommonUtil::log_message(msg);

        sprintf(msg, "Computer name : %d ", CommonUtil::getComputerName().c_str());
        CommonUtil::log_message(msg);

        sprintf(msg, "User name : %d ", CommonUtil::getUserName().c_str());
        CommonUtil::log_message(msg);

        CommonUtil::log_message("\n\n\n");
    }
    catch (...)
    {
        CommonUtil::log_message("ERROR :: Some exception occurred in ServiceProcess::showSysInfo()");
    }
}

void ServiceProcess::run()
{
    char msg[255] = {0};
    sprintf(msg, "Starting service : %s from process PID : %d \n", config.getServiceDisplayName().c_str(), CommonUtil::getPid());
    CommonUtil::log_message(msg);

#ifdef WINDOWS
    SetConsoleCtrlHandler(signal_handler_callback, TRUE);
#else
    signal(SIGCHLD, SIG_IGN); /* ignore child */
    signal(SIGTSTP, SIG_IGN); /* ignore tty signals */
    signal(SIGTTOU, signal_handler_callback);
    signal(SIGTTIN, signal_handler_callback);
    signal(SIGHUP, signal_handler_callback);  /* catch hangup signal */
    signal(SIGTERM, signal_handler_callback); /* catch kill signal */
#endif

    showSysInfo();

    //Using thread
    //std::thread processMonitorThreadId(&ServiceProcess::processMonitorThread, this);
    //std::thread healthMonitorThreadId(&ServiceProcess::healthMonitorThread, this);
    //processMonitorThreadId.join();

    //Using pthread
    pthread_t processMonitorThreadId, healthMonitorThreadId;
    bool pidTestThreadInitiated = false;
    bool healthTestThreadInitiated = false;
    if (config.enableProcessPIDCheck())
    {
        pthread_create(&processMonitorThreadId, NULL, ServiceProcess::processMonitorThreadCallback, (void *)&*this);
        pidTestThreadInitiated = true;
    }
    if (config.enableProcessHealthCheck())
    {
        pthread_create(&healthMonitorThreadId, NULL, ServiceProcess::healthMonitorThreadCallback, (void *)&*this);
        healthTestThreadInitiated = true;
    }
    if (pidTestThreadInitiated)
    {
        pthread_join(processMonitorThreadId, NULL);
    }
    else if (healthTestThreadInitiated)
    {
        pthread_join(healthMonitorThreadId, NULL);
    }
}

#ifdef WINDOWS
int ServiceProcess::launchProcessInSeparateDaemon(const char *command)
{

    char msg[255];
    CommonUtil::log_message(("\n\n" + (std::string)command + "\n\n").c_str());

    // additional information
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    // set the size of the structures
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // start the program up
    int status = CreateProcess(NULL,           // the path
                               (LPSTR)command, // Command line
                               NULL,           // Process handle not inheritable
                               NULL,           // Thread handle not inheritable
                               FALSE,          // Set handle inheritance to FALSE
                               0,              // No creation flags
                               NULL,           // Use parent's environment block
                               NULL,           // Use parent's starting directory
                               &si,            // Pointer to STARTUPINFO structure
                               &pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
    );
    sprintf(msg, "ServiceProcess::launchProcessInSeparateDaemon() Status : %d \n", status);
    CommonUtil::log_message(msg);
    if (status)
    {
        // Close process and thread handles.
        status = pi.dwProcessId;
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return status;
    }
    else
    {
        return pi.dwProcessId;
    }
}
#else
int ServiceProcess::launchProcessInSeparateDaemon(const char *command)
{
    int pid;
    int i;

    CommonUtil::log_message(command);

    pid = fork();
    signal(SIGCHLD, SIG_IGN); /* ignore child */

    if (pid < 0)
    {
        //Unable to fork
        return pid;
    }
    else if (pid == 0)
    {
        //Inside forked thread
        for (i = getdtablesize(); i >= 0; --i)
        {
            close(i); /* close all descriptors */
        }

        setsid(); /* obtain a new process group */

        execl("/bin/sh", "sh", "-c", command, NULL);
        _exit(1);
    }
    else
    {
        //Inside parent thread
    }
    return pid;
}
#endif

#ifdef WINDOWS
int ServiceProcess::launchProcessInSeparateDaemonWithHandlers(const char *command, int *infp, int *outfp)
{

    char msg[255];
    CommonUtil::log_message(("\n\n" + (std::string)command + "\n\n").c_str());

    // Differnt Handlers
    HANDLE p_stdin[2];
    HANDLE p_stdout[2];

    // https://docs.microsoft.com/en-us/windows/desktop/ProcThread/creating-a-child-process-with-redirected-input-and-output

    // Set the bInheritHandle flag so pipe handles are inherited.
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    // Create a pipe for the child process's STDOUT.
    if (!CreatePipe(&p_stdout[0], &p_stdout[1], &saAttr, 0))
        return -1;

    // Ensure the read handle to the pipe for STDOUT is not inherited.
    if (!SetHandleInformation(p_stdout[0], HANDLE_FLAG_INHERIT, 0))
    {
        CloseHandle(p_stdout[0]);
        CloseHandle(p_stdout[1]);
        return -1;
    }

    // Create a pipe for the child process's STDIN.
    if (!CreatePipe(&p_stdin[0], &p_stdin[1], &saAttr, 0))
    {
        CloseHandle(p_stdout[0]);
        CloseHandle(p_stdout[1]);
        return -1;
    }

    // Ensure the write handle to the pipe for STDIN is not inherited.
    if (!SetHandleInformation(p_stdin[1], HANDLE_FLAG_INHERIT, 0))
    {
        CloseHandle(p_stdout[0]);
        CloseHandle(p_stdout[1]);
        CloseHandle(p_stdin[0]);
        CloseHandle(p_stdin[1]);
        return -1;
    }

    // additional information
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    // set the size of the structures
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    //Set Handlers for Process Start Info
    si.hStdError = p_stdout[1];
    si.hStdOutput = p_stdout[1];
    si.hStdInput = p_stdin[0];
    si.dwFlags |= STARTF_USESTDHANDLES;

    ZeroMemory(&pi, sizeof(pi));

    // start the program up
    int status = CreateProcess(NULL,           // the path
                               (LPSTR)command, // Command line
                               NULL,           // Process security attributes  (Process handle not inheritable)
                               NULL,           // Primary thread security attributes (Thread handle not inheritable)
                               TRUE,           // Set handle inheritance to FALSE
                               0,              // No creation flags
                               NULL,           // Use parent's environment block
                               NULL,           // Use parent's Working directory
                               &si,            // Pointer to STARTUPINFO structure
                               &pi             // Receives Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
    );
    sprintf(msg, "ServiceProcess::launchProcessInSeparateDaemonWithHandlers() Status : %d \n", status);
    CommonUtil::log_message(msg);
    if (!status)
    {
        CommonUtil::log_message("Process couldn't be started");
        status = pi.dwProcessId;

        // Close process and thread handles.
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        CloseHandle(p_stdout[0]);
        CloseHandle(p_stdout[1]);
        CloseHandle(p_stdin[0]);
        CloseHandle(p_stdin[1]);
        return status;
    }
    else
    {
        CommonUtil::log_message("Pcrocess started successfully");
        status = pi.dwProcessId;

        CloseHandle(p_stdout[1]);
        CloseHandle(p_stdin[0]);

        if (infp == NULL)
        {
            CommonUtil::log_message("infp == NULL");
            CloseHandle(p_stdin[1]);
        }
        else
        {
            *infp = *((int *)(&p_stdin[1])); //   (int)p_stdin[0];
        }

        if (outfp == NULL)
        {
            CommonUtil::log_message("outfp == NULL");
            CloseHandle(p_stdout[0]);
        }
        else
        {
            *outfp = *((int *)(&p_stdout[1])); //(int)p_stdout[0];
        }

        // Close handles to the child process and its primary thread
        // Some applications might keep these handles to monitor the status of the child process
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        return status;
    }
}
#else
int ServiceProcess::launchProcessInSeparateDaemonWithHandlers(const char *command, int *infp, int *outfp)
{
    int p_stdin[2];
    int p_stdout[2];
    pid_t pid;

    CommonUtil::log_message(("\n\n" + (std::string)command + "\n\n").c_str());

    // https://www.geeksforgeeks.org/c-program-demonstrate-fork-and-pipe/
    if (pipe(p_stdin) == -1)
        return -1;

    if (pipe(p_stdout) == -1)
    {
        close(p_stdin[0]);
        close(p_stdin[1]);
        return -1;
    }

    pid = fork();

    if (pid < 0)
    {
        close(p_stdin[0]);
        close(p_stdin[1]);
        close(p_stdout[0]);
        close(p_stdout[1]);
        return pid;
    }
    else if (pid == 0)
    {
        close(p_stdin[1]);
        dup2(p_stdin[0], 0);
        close(p_stdout[0]);
        dup2(p_stdout[1], 1);
        dup2(open("/dev/null", O_RDONLY), 2);
        // Close all other descriptors for the safety sake.
        int i;
        for (i = 3; i < getdtablesize(); ++i)
            close(i);

        setsid();
        execl("/bin/sh", "sh", "-c", command, NULL);
        CommonUtil::log_message("Pcrocess failed or terminated");
        _exit(1);
    }

    close(p_stdin[0]);
    close(p_stdout[1]);

    if (infp == NULL)
    {
        close(p_stdin[1]);
    }
    else
    {
        *infp = p_stdin[1];
    }

    if (outfp == NULL)
    {
        close(p_stdout[0]);
    }
    else
    {
        *outfp = p_stdout[0];
    }

    return pid;
}
#endif

#ifdef WINDOWS

BOOL WINAPI ServiceProcess::signal_handler_callback(DWORD dwType)
{
    char msg[100] = {0};
    sprintf(msg, "Signal recieved : %d \n", dwType);
    CommonUtil::log_message(msg);

    // No need to propogate signal manually, as signal automatically propogated to child processes

    switch (dwType)
    {
    case CTRL_C_EVENT:
    {
        CommonUtil::log_message("CTRL_C_EVENT signal catched");
        break;
    }
    case CTRL_BREAK_EVENT:
    {
        CommonUtil::log_message("CTRL_BREAK_EVENT signal catched");
        break;
    }
    case CTRL_CLOSE_EVENT:
    {
        CommonUtil::log_message("CTRL_CLOSE_EVENT signal catched");
        break;
    }
    case CTRL_LOGOFF_EVENT:
    {
        CommonUtil::log_message("CTRL_LOGOFF_EVENT signal catched");
        break;
    }
    case CTRL_SHUTDOWN_EVENT:
    {
        CommonUtil::log_message("CTRL_SHUTDOWN_EVENT signal catched");
        //set_done(); //signal the main thread to terminate

        //Returning would make the process exit!
        //We just make the handler sleep until the main thread exits,
        //or until the maximum execution time for this handler is reached.
        CommonUtil::_sleep(1);
    }
    default:
    {
        CommonUtil::log_message("other shutdown signal catched : : :");

        static DWORD signal_type = SIGINT;
        //static int process_pid = child_process_pid;

        struct HANDLER
        { // struct's as good as class
            static BOOL CALLBACK callback(HWND hwnd, LPARAM lParam)
            {
                //CommonUtil::log_message("EnumWindows handler called");
                DWORD dwID;
                GetWindowThreadProcessId(hwnd, &dwID);
                if (dwID == (DWORD)lParam)
                {
                    //CommonUtil::log_message("Sending signal WM_CLOSE");
                    //PostMessage(hwnd, signal_type, 0, 0);
                    // GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, process_pid);
                }
                return FALSE;
            }
        };

        // No need to propogate signal manually, as signal automatically propogated to child processes
        // EnumWindows(HANDLER::callback, child_process_pid);
        // EndTask(hwHandler, FALSE, FALSE);
        break;
    }
    }
    return FALSE;
}

#else

void ServiceProcess::signal_handler_callback(int sig)
{
    char msg[100] = {0};
    sprintf(msg, "Signal recieved : %d \n", sig);
    CommonUtil::log_message(msg);

    switch (sig)
    {
    case SIGHUP:
    {
        CommonUtil::log_message("Linux Service :: hangup signal catched");
        kill(child_process_pid, SIGHUP);
        break;
    }
    case SIGTERM:
    {
        CommonUtil::log_message("Linux Service :: terminate signal catched");
        kill(child_process_pid, SIGTERM);
        exit(0);
        break;
    }
    case SIGTTOU:
    {
        CommonUtil::log_message("Linux Service :: SIGTTOU signal catched");
        kill(child_process_pid, SIGTTOU);
        break;
    }
    case SIGTTIN:
    {
        CommonUtil::log_message("Linux Service :: SIGTTIN signal catched");
        kill(child_process_pid, SIGTTIN);
        break;
    }
    default:
    {
        CommonUtil::log_message("other shutdown signal catched : : :");
        kill(child_process_pid, sig);
    }
    }
}
#endif

void ServiceProcess::terminateApplication(int applicationShutdownTimeout)
{
    char msg[255] = {0};
    int pid = child_process_pid;
    bool result = CommonUtil::gracefullyKillProcessWithId(pid, (void *)&signal_handler_callback);
    sprintf(msg, "Inside ServiceProcess::terminateApplication(), PID : %d gracefully Kill (SIGINT) status(true/false) : %d \n", child_process_pid, result);
    CommonUtil::log_message(msg);
    int counter = 0;
    while (counter * 3 < applicationShutdownTimeout)
    {
        if (CommonUtil::ifProcessWithPIDExist(pid))
        {
            CommonUtil::_sleep(3);
        }
        else
        {
            return;
        }
        counter++;
    }
    sprintf(msg, "Inside ServiceProcess::terminateApplication(), applicationShutdownTimeout completed for PID : %d . Going to terminate process forcefully.\n", pid);
    CommonUtil::log_message(msg);
    result = CommonUtil::killProcessWithId(pid);
    sprintf(msg, "Inside ServiceProcess::terminateApplication(), PID : %d kill (SIGKILL) status(true/false) : %d \n", child_process_pid, result);
    CommonUtil::log_message(msg);
}

void *ServiceProcess::logChildProcessStdOutThreadCallback(void *objInstance)
{
    ServiceProcess *obj = (ServiceProcess *)objInstance;
    obj->logChildProcessStdOutThread();
    return NULL;
}

#ifdef WINDOWS
void ServiceProcess::logChildProcessStdOutThread()
{
    if (!child_process_std_out)
    {
        CommonUtil::log_message("Child Process STD OUT Handle is NULL");
        return;
    }
    HANDLE hStdOut = (HANDLE)(intptr_t)child_process_std_out; //*((HANDLE)(&child_process_std_out))
    CommonUtil::log_message("Logging Child Process console");
    DWORD dwRead;
    BOOL bSuccess = FALSE;
    for (;;)
    {
        char buffer[1] = {};
        string line;
        while (buffer[0] != '\n')
        {
            bSuccess = ReadFile(hStdOut, buffer, sizeof(buffer), &dwRead, NULL);
            if (bSuccess && dwRead > 0)
            {
                if (buffer[0] != '\n')
                {
                    line += buffer[0];
                }
            }
            else
            {
                CommonUtil::log_message("Read Child Process console failed.");
                break;
            }
        }
        CommonUtil::log_child_console(line.c_str());
        if (!bSuccess)
        {
            break;
        }
    }
    CancelIo(hStdOut); //hopefully this wont screw with cout calls in child proc
    CloseHandle(hStdOut);
}
#else
void ServiceProcess::logChildProcessStdOutThread()
{
    if (!child_process_std_out)
    {
        CommonUtil::log_message("Child Process STD OUT Handle is NULL");
        return;
    }
    int hStdOut = child_process_std_out;
    CommonUtil::log_message("Logging Child Process console");
    int dwRead;
    for (;;)
    {
        char buffer[1] = {};
        string line;
        while (buffer[0] != '\n')
        {
            dwRead = read(hStdOut, buffer, sizeof(buffer));
            if (dwRead > 0)
            {
                if (buffer[0] != '\n')
                {
                    line += buffer[0];
                }
            }
            else
            {
                CommonUtil::log_message("Read Child Process console failed.");
                break;
            }
        }
        CommonUtil::log_child_console(line.c_str());
        if (dwRead <= 0)
        {
            break;
        }
    }
    close(hStdOut);
}
#endif

void *ServiceProcess::healthMonitorThreadCallback(void *objInstance)
{
    ServiceProcess *obj = (ServiceProcess *)objInstance;
    obj->healthMonitorThread();
    return NULL;
}

void ServiceProcess::healthMonitorThread()
{
    char msg[255] = {0};
    sprintf(msg, "Inside Service : %s's healthMonitorThread  (Service process PID : %d) ", config.getServiceName().c_str(), CommonUtil::getPid());
    CommonUtil::log_message(msg);

    int healthTestInterval = config.fetchProcessHealthCheckInterval();
    int healthCheckRetryAttemptCount = config.fetchHealthCheckRetryAttemptCount();
    int applicationStartupTimeout = config.fetchApplicationStartupTimeout();
    int applicationShutdownTimeout = config.fetchApplicationShutdownTimeout();
    int healthCheckCounter = 0;
    CommonUtil::_sleep(applicationStartupTimeout); //First time wait for application to be started

    ProcessHealthCheckData healthCheckData;

    healthCheckData.appPort = config.fetchAppPort();
    healthCheckData.appServer = config.fetchAppServer();
    healthCheckData.requestBody = config.fetchAppHealthAPIRequestBody();
    healthCheckData.requestMethod = config.fetchAppHealthAPIRequestMethod();
    healthCheckData.requestContentType = config.fetchAppHealthAPIRequestContentType();
    healthCheckData.requestURI = config.fetchAppHealthAPIRequestURI();
    healthCheckData.responseContentTYpe = config.fetchAppHealthAPIResponseContentType();
    healthCheckData.responseKey = config.fetchAppHealthAPIResponseHealthKey();
    healthCheckData.responseValue = config.fetchAppHealthAPIResponseExpectedValue();

    sprintf(msg, "Health Test Configuration Details :: App Server : %s, App Port : %d, URI : %s ", healthCheckData.appServer.c_str(), healthCheckData.appPort, healthCheckData.requestURI.c_str());
    CommonUtil::debug_message(msg);

    while (1)
    {
        try
        {
            CommonUtil::_sleep(healthTestInterval);
            sprintf(msg, "Inside ServiceProcess::healthMonitorThread(), Going to test process health for  PID : %d \n", child_process_pid);
            CommonUtil::debug_message(msg);
            if (!healthTest(&healthCheckData))
            {
                healthCheckCounter++;
                sprintf(msg, "Inside ServiceProcess::healthMonitorThread(), Process with with PID : %d not qualified health test. Failed health test counter : %d .\n", child_process_pid, healthCheckCounter);
                CommonUtil::log_message(msg);
                if (healthCheckCounter >= healthCheckRetryAttemptCount)
                {
                    sprintf(msg, "Inside ServiceProcess::healthMonitorThread(), Going to terminate Process with with PID : %d .\n", child_process_pid);
                    CommonUtil::log_message(msg);
                    terminateApplication(applicationShutdownTimeout);
                    CommonUtil::_sleep(applicationStartupTimeout);
                    healthCheckCounter = 0;
                }
            }
            else
            {
                healthCheckCounter = 0;
                sprintf(msg, "Inside ServiceProcess::healthMonitorThread(), Process with with PID : %d successfully qualified health test.\n", child_process_pid);
                CommonUtil::debug_message(msg);
            }
        }
        catch (...)
        {
            sprintf(msg, "ERROR :: Some exception occurred inside Service : %s's ServiceProcess::healthMonitorThread()  (process PID : %d) \n", config.getServiceName().c_str(), CommonUtil::getPid());
            CommonUtil::log_message(msg);
        }
    }
    return;
}

void *ServiceProcess::processMonitorThreadCallback(void *objInstance)
{
    ServiceProcess *obj = (ServiceProcess *)objInstance;
    obj->processMonitorThread();
    return NULL;
}

void ServiceProcess::processMonitorThread()
{
    char msg[255] = {0};
    sprintf(msg, "Inside Service : %s's ProcessMonitorThread  (Service process PID : %d) ", config.getServiceName().c_str(), CommonUtil::getPid());
    CommonUtil::log_message(msg);

    std::string command = config.fetchRunnableServiceCommand();
    int pidTestInterval = config.fetchProcessPIDCheckInterval();

    int healthCheckCounter = 0;
    while (1)
    {
        try
        {
            CommonUtil::_sleep(pidTestInterval);
            //child_process_pid = CommonUtil::getPid(config.getServiceName());
            if (child_process_pid == 0 || !CommonUtil::ifProcessWithPIDExist(child_process_pid))
            {
                sprintf(msg, "Inside ServiceProcess::processMonitorThread(), Process not exist with  PID : %d ", child_process_pid);
                CommonUtil::log_message(msg);

                // child_process_pid = launchProcessInSeparateDaemon(command.c_str());

                // Use Child Process StdOutHandle to log Child Process Console
                child_process_pid = launchProcessInSeparateDaemonWithHandlers(command.c_str(), &child_process_std_in, &child_process_std_out);
                pthread_t logStdOutThreadId;
                void *child_process_std_out_ptr = (void *)(intptr_t)child_process_std_out; // *((void *)(&child_process_std_out));
                pthread_create(&logStdOutThreadId, NULL, logChildProcessStdOutThreadCallback, child_process_std_out_ptr);

                sprintf(msg, "Inside ServiceProcess::processMonitorThread(), New process launched with PID : %d ", child_process_pid);
                CommonUtil::log_message(msg);
            }
            else
            {
                sprintf(msg, "Inside ServiceProcess::processMonitorThread(), Process exists with  PID : %d ", child_process_pid);
                CommonUtil::debug_message(msg);
            }
        }
        catch (...)
        {
            sprintf(msg, "ERROR :: Some exception occurred inside Service : %s's ServiceProcess::processMonitorThread()  (process PID : %d) ", config.getServiceName().c_str(), CommonUtil::getPid());
            CommonUtil::log_message(msg);
        }
    }
    return;
}

bool ServiceProcess::healthTest(ProcessHealthCheckData *healthCheckData)
{
    char msg[255] = {0};
    try
    {
        CommonUtil::debug_message("Inside ServiceProcess::healthTest(), server response : \n\n");

        // string rec = httpUtil.fetchHTTPPostResponse("127.0.0.1", 8080, "/rest/auth/login", "application/json", "{\"user\":{\"username\":\"admin\",\"password\":\"admin\"}}");
        sprintf(msg, "Health Test :: App Server : %s, App Port : %d, URI : %s \n\n", healthCheckData->appServer.c_str(), healthCheckData->appPort, healthCheckData->requestURI.c_str());
        CommonUtil::debug_message(msg);

        string rec = "";
        if (CommonUtil::caseInsensitiveCompare(healthCheckData->requestMethod, "POST"))
        {
            rec = httpUtil.fetchHTTPPostResponse(healthCheckData->appServer, healthCheckData->appPort, healthCheckData->requestURI, healthCheckData->requestContentType, healthCheckData->requestBody);
        }
        else
        {
            rec = httpUtil.fetchHTTPGetResponse(healthCheckData->appServer, healthCheckData->appPort, healthCheckData->requestURI, healthCheckData->requestContentType);
        }
        CommonUtil::debug_message(rec.c_str());
        CommonUtil::debug_message("\n\nGoing to process above response.");
        if (rec.empty() || strlen(rec.c_str()) < 12)
        {
            CommonUtil::debug_message("Response is empty.");
            return false;
        }
        CommonUtil::debug_message("Parsing Response Header.");
        HttpResponse res = httpUtil.parseResponse(rec);
        sprintf(msg, "Response header parsed successfully. Response Code : %d, response string : \n\n", res.responseCode);
        CommonUtil::debug_message(msg);
        CommonUtil::debug_message(res.responseString.c_str());
        if (res.responseCode == 200 && strlen(res.responseString.c_str()) >= 2)
        {
            if (!CommonUtil::caseInsensitiveCompare(healthCheckData->responseContentTYpe, "application/json"))
            {
                return true;
            }
            CommonUtil::debug_message("Going to parse response body. wstring body : \n\n");
            std::wstring wsTmp(res.responseString.begin(), res.responseString.end());
            CommonUtil::debug_message((char *)wsTmp.c_str());
            JSONValue *json = JSON::Parse(wsTmp.c_str());
            std::wstring responseKey(healthCheckData->responseKey.begin(), healthCheckData->responseKey.end());
            std::string status = JSONUtil::getChildAsString(json, (wchar_t *)responseKey.c_str());
            sprintf(msg, "Response body parsed successfully. API status : %s", status.c_str());
            CommonUtil::debug_message(msg);
            if (CommonUtil::caseInsensitiveCompare(status, healthCheckData->responseValue))
            {
                CommonUtil::debug_message("Process passed health test successfully.");
                return true;
            }
        }
    }
    catch (...)
    {
        sprintf(msg, "ERROR :: Some exception occurred inside Service : %s's ServiceProcess::healthTest()  (process PID : %d) \n", config.getServiceName().c_str(), CommonUtil::getPid());
        CommonUtil::log_message(msg);
    }
    CommonUtil::debug_message("Process couldn't passed health test.");
    return false;
}

#ifdef WINDOWS
int mainServiceProcess()
#else
int mainServiceProcess()
#endif
{
    ServiceProcess serviceProcess;
    serviceProcess.run();
    return 0;
}