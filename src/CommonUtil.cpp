#include "../include/CommonUtil.h"
#include <stdio.h>
#include <iostream>
#include <ctime>
#include <string.h>

//#define WINDOWS /* uncomment this line to use it for windows.*/

int CommonUtil::static_init()
{
    LOG_FILE = "logs/service.log";
    CHILD_PROCESS_LOG_FILE = "console.log";

#ifdef WINDOWS
    _mkdir("logs");
#else
    mkdir("logs", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif

    // Folder with read/write/search permissions for owner and group, and with read/search permissions for others

    populateResources();
    return 0;
}

std::string CommonUtil::LOG_FILE;
FILE *CommonUtil::LOG_FILE_HANDLE = NULL;

std::string CommonUtil::CHILD_PROCESS_LOG_FILE;
FILE *CommonUtil::CHILD_PROCESS_LOG_FILE_HANDLE = NULL;

int CommonUtil::__static_init = CommonUtil::static_init();

void CommonUtil::cleanUpResources()
{
    std::cout << "Inside CommonUtil::cleanUpResources(). " << std::endl;
    if (LOG_FILE_HANDLE)
    {
        fclose(LOG_FILE_HANDLE);
        // delete LOG_FILE_HANDLE; //freed memory
        LOG_FILE_HANDLE = NULL; //pointed dangling ptr to NULL
    }

    if (CHILD_PROCESS_LOG_FILE_HANDLE)
    {
        fclose(CHILD_PROCESS_LOG_FILE_HANDLE);
        // delete CHILD_PROCESS_LOG_FILE_HANDLE; //freed memory
        CHILD_PROCESS_LOG_FILE_HANDLE = NULL; //pointed dangling ptr to NULL
    }
}

void CommonUtil::populateResources()
{
    std::cout << "Inside CommonUtil::populateResources(). " << std::endl;
    cleanUpResources();

    std::cout << "Inside CommonUtil populateResources, LOG_FILE : " << LOG_FILE << std::endl;
    LOG_FILE_HANDLE = fopen(LOG_FILE.c_str(), "a");

    char *consoleFile = (char *)LOG_FILE.c_str();
    consoleFile = dirname(consoleFile);
    // std::cout << "consoleFile dirname path : " << consoleFile << std::endl;

#ifdef WINDOWS
    strcat(consoleFile, "\\");
#else
    strcat(consoleFile, "/");
#endif

    strcat(consoleFile, CHILD_PROCESS_LOG_FILE.c_str());
    std::cout << "Inside CommonUtil populateResources, CHILD_PROCESS_LOG_FILE : " << consoleFile << std::endl;
    CHILD_PROCESS_LOG_FILE_HANDLE = fopen(consoleFile, "a");
}

CommonUtil::CommonUtil(void)
{
    std::cout << "Inside CommonUtil Constructor. " << std::endl;
}

CommonUtil::~CommonUtil(void)
{
    std::cout << "Inside CommonUtil Destructor. " << std::endl;
    cleanUpResources();
}

bool CommonUtil::ENABLE_DEBUGGING_LOG = false;

std::string CommonUtil::getCurrentTime()
{
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%d-%m-%Y %I:%M:%S", timeinfo);
    std::string str(buffer);
    return str;
}

//For logging
void CommonUtil::log_message(std::string filename, char *message)
{
    char msg[strlen(message) + 64];
    sprintf(msg, "%s :: %s \n", getCurrentTime().c_str(), message);
    int writtenBytes = -1;
    printf(msg);
    if (LOG_FILE_HANDLE)
    {
        writtenBytes = fprintf(LOG_FILE_HANDLE, "%s", msg);
        fflush(LOG_FILE_HANDLE);
    }
    // std::cout << "Inside CommonUtil::log_message(), LOG_FILE_HANDLE : " << LOG_FILE_HANDLE << ", Bytes written : " << writtenBytes << std::endl;
}

void CommonUtil::log_message(std::string filename, const char *message)
{
    char *msg = (char *)message;
    log_message(filename, msg);
}

void CommonUtil::log_message(char *message)
{
    log_message(LOG_FILE, message);
}

void CommonUtil::log_message(const char *message)
{
    log_message(LOG_FILE, message);
}

void CommonUtil::log_child_console(const char *message)
{
    char msg[strlen(message) + 25];
    sprintf(msg, "%s :: %s\n", getCurrentTime().c_str(), message);
    int writtenBytes = -1;
    if (!CHILD_PROCESS_LOG_FILE_HANDLE)
    {
        printf(msg);
    }
    else
    {
        writtenBytes = fprintf(CHILD_PROCESS_LOG_FILE_HANDLE, "%s", msg);
        fflush(CHILD_PROCESS_LOG_FILE_HANDLE);
    }
    // std::cout << "Inside CommonUtil::log_child_console(), CHILD_PROCESS_LOG_FILE_HANDLE : " << CHILD_PROCESS_LOG_FILE_HANDLE << ", Bytes written : " << writtenBytes << std::endl;
}

void CommonUtil::debug_message(char *message)
{
    if (ENABLE_DEBUGGING_LOG)
    {
        log_message(LOG_FILE, message);
    }
}

void CommonUtil::debug_message(const char *message)
{
    if (ENABLE_DEBUGGING_LOG)
    {
        log_message(LOG_FILE, message);
    }
}

// To find current path
#ifdef WINDOWS
#include <direct.h>
#include <windows.h>
#include <tlhelp32.h>
#include <algorithm>
//#include <string.h>
#define GetCurrentDir _getcwd
//#define GetPID _getpid
//#define GetPPID _getppid
#define GetPID GetCurrentProcessId
#define GetPPID GetCurrentProcessId
#define ComputerName GetComputerNameA
#define UserName GetUserName
#else
#include <unistd.h>
#include <limits.h>
#include <bits/stdc++.h>
#include <dirent.h>
#define GetCurrentDir getcwd
#define GetPID getpid
#define GetPPID getppid
#define ComputerName gethostname
#define UserName getlogin_r
#endif

std::string CommonUtil::getCurrentWorkingDir(void)
{
    char buff[FILENAME_MAX];
    GetCurrentDir(buff, FILENAME_MAX);
    std::string current_working_dir(buff);
    return current_working_dir;
}

std::string CommonUtil::getComputerName()
{
//getenv("COMPUTERNAME")
#ifdef WINDOWS
    const int ComputerNameMax = MAX_COMPUTERNAME_LENGTH + 1;
    char buff[ComputerNameMax] = "";
    DWORD size = ComputerNameMax;
    ComputerName(buff, &size);
#else
    const int ComputerNameMax = HOST_NAME_MAX;
    char buff[ComputerNameMax];
    ComputerName(buff, ComputerNameMax);
#endif
    std::string computerName(buff);
    return computerName;
}

std::string CommonUtil::getUserName()
{
    //getenv("USERNAME")

#ifdef WINDOWS
    const int UserNameMax = 255;
    char buff[UserNameMax] = "";
    DWORD size = UserNameMax;
    UserName(buff, &size);
#else
    const int UserNameMax = LOGIN_NAME_MAX;
    char buff[UserNameMax];
    UserName(buff, UserNameMax);
#endif
    std::string userName(buff);
    return userName;
}

std::string CommonUtil::getComputerName_env()
{
    return getenv("COMPUTERNAME");
}

std::string CommonUtil::getUserName_env()
{
    return getenv("USERNAME");
}

int CommonUtil::getPid(std::string processName)
{
#ifdef WINDOWS
    DWORD pid = 0;

    // Create toolhelp snapshot.
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 process;
    ZeroMemory(&process, sizeof(process));
    process.dwSize = sizeof(process);

    // Walkthrough all processes.
    if (Process32First(snapshot, &process))
    {
        do
        {
            // Compare process.szExeFile based on format of name, i.e., trim file path
            // trim .exe if necessary, etc.
            std::string strExeName = process.szExeFile;

            //std::cout << "\ns1: " << strExeName << " s2: " << processName << "\n";
            if (caseInsensitiveCompare(strExeName, processName))
            {
                //std::cout << "\nstring matched\n";
                pid = process.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &process));
    }

    CloseHandle(snapshot);

    int processId = (int)pid;
#else
    int MAX_BUF = 1024;
    DIR *dirp;
    FILE *fp;
    struct dirent *entry;
    int pid;
    char path[MAX_BUF];
    char read_buf[MAX_BUF];
    char pname[MAX_BUF];
    //wcstombs(pname, processName.c_str(), processName.size() + 1);
    strcpy(pname, processName.c_str());

    dirp = opendir("/proc/");
    if (dirp == NULL)
    {
        perror("Fail");
        return 0;
    }

    bool bFoundPID = false;

    while ((entry = readdir(dirp)) != NULL)
    {
        char *endptr;
        strtol(entry->d_name, &endptr, 10);
        if ('\0' == *endptr)
        {
            strcpy(path, "/proc/");
            strcat(path, entry->d_name);
            strcat(path, "/comm");

            /* A file may not exist, it may have been removed.
       * dut to termination of the process. Actually we need to
       * make sure the error is actually file does not exist to
       * be accurate.
       */
            fp = fopen(path, "r");
            if (fp != NULL)
            {
                fscanf(fp, "%s", read_buf);
                //std::cout << "\ns1: " << read_buf << " s2: " << pname << "\n";
                if (strcmp(read_buf, pname) == 0)
                {
                    //std::cout << "\nstring matched\n";
                    /* add to list and expand list if needed */
                    pid = atoi(entry->d_name);
                    bFoundPID = true;
                }
                else
                {
                    pid = 0;
                }
                fclose(fp);

                if (bFoundPID)
                    break;
            }
        }
    }

    closedir(dirp);
    int processId = (int)pid;
#endif
    return processId;
}

bool CommonUtil::killProcessWithId(int pid, int &exitCode)
{
#ifdef WINDOWS
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, false, (DWORD)pid);
    if (hProcess == NULL)
        return false;

    bool result = TerminateProcess(hProcess, exitCode);

    CloseHandle(hProcess);
#else

    bool result = kill(pid, SIGKILL);
    exitCode = 0;
#endif
    return result;
}

bool CommonUtil::gracefullyKillProcessWithId(int pid, int &exitCode, void *signalHandler = NULL)
{
    bool result = false;
#ifdef WINDOWS
    /*
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, false, (DWORD)pid);
    if (hProcess == NULL)
        return false;

    bool result = TerminateProcess(hProcess, exitCode);

    CloseHandle(hProcess);
    */

    static DWORD signal_type = SIGINT;
    static int process_pid = pid;

    struct EnumChildWindowsHANDLER
    { // struct's as good as class
        static BOOL CALLBACK callback(HWND hwnd, LPARAM lParam)
        {
            //CommonUtil::log_message("EnumWindows handler called");
            // std::cout << "hwnd : " << hwnd << " , lParam : " << lParam << " , process_pid : " << process_pid << std::endl;
            DWORD dwID;
            GetWindowThreadProcessId(hwnd, &dwID);
            // std::cout << "dwID : " << dwID << " , lParam : " << lParam << " , process_pid : " << process_pid << std::endl;
            if (dwID == (DWORD)lParam)
            {
                CommonUtil::log_message("Sending signal WM_CLOSE");
                PostMessage(hwnd, WM_CLOSE, 0, 0);
                //SendMessageTimeout();
                // GenerateConsoleCtrllEvent(CTRL_BREAK_EVENT, process_pid);
                return FALSE;
            }
            else
            {
                std::cout << "dwID : " << dwID << " , lParam : " << lParam << " , process_pid : " << process_pid << std::endl;
                // CommonUtil::log_message("PID not matched");
            }
            return TRUE;
        }
    };

    struct EnumWindowsHANDLER
    { // struct's as good as class
        static BOOL CALLBACK callback(HWND hwnd, LPARAM lParam)
        {
            EnumChildWindows(hwnd, EnumChildWindowsHANDLER::callback, process_pid);
            return TRUE;
        }
    };
    CommonUtil::log_message("Calling EnumWindows()");
    EnumWindows(EnumWindowsHANDLER::callback, process_pid);

    // EndTask(hwHandler, FALSE, FALSE);
    CommonUtil::log_message("Called EnumWindows()");

#else

    result = kill(pid, SIGINT);
    exitCode = 0;
#endif
    return result;
}

bool CommonUtil::ifProcessWithPIDExist(int pid)
{
    bool exists = false;
    try
    {
#ifdef WINDOWS
        HANDLE pss = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
        PROCESSENTRY32 pe = {0};
        pe.dwSize = sizeof(pe);

        if (Process32First(pss, &pe))
        {
            do
            {
                // pe.szExeFile can also be useful
                if (pe.th32ProcessID == pid)
                {
                    exists = true;
                    break;
                }
            } while (Process32Next(pss, &pe));
        }
        CloseHandle(pss);
#else
        exists = (kill(pid, 0) == 0);
#endif
    }
    catch (...)
    {
        log_message("ERROR :: Some exception occurred in CommonUtil::ifProcessWithPIDExist()");
    }
    return exists;
}

bool CommonUtil::killProcessWithId(int pid)
{
    int exitCode = 0;
    return killProcessWithId(pid, exitCode);
}

bool CommonUtil::gracefullyKillProcessWithId(int pid, void *signalHandler = NULL)
{
    int exitCode = 0;
#ifdef WINDOWS
    exitCode = CTRL_BREAK_EVENT;
#endif
    return gracefullyKillProcessWithId(pid, exitCode, signalHandler);
}

bool CommonUtil::killProcessWithName(std::string processName, int &exitCode)
{
    int pid = getPid(processName);

    if (pid == 0)
        return false;

    bool result = killProcessWithId(pid, exitCode);

    return result;
}

bool CommonUtil::killProcessWithName(std::string processName)
{
    int exitCode = 0;
    return killProcessWithName(processName, exitCode);
}

int CommonUtil::getPid()
{
    return GetPID();
}

int CommonUtil::getPPid()
{
#ifdef WINDOWS
    int dwProcessID = getPid();
    DWORD dwParentProcessID = -1;
    HANDLE hProcessSnapshot;
    PROCESSENTRY32 processEntry32;

    hProcessSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnapshot != INVALID_HANDLE_VALUE)
    {
        processEntry32.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hProcessSnapshot, &processEntry32))
        {
            do
            {
                if (dwProcessID == processEntry32.th32ProcessID)
                {
                    dwParentProcessID = processEntry32.th32ParentProcessID;
                    break;
                }
            } while (Process32Next(hProcessSnapshot, &processEntry32));

            CloseHandle(hProcessSnapshot);
        }
    }
    int pid = dwParentProcessID;
#else
    int pid = GetPPID();
#endif
    return pid;
}

// trim from start (in place)
void CommonUtil::ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
                return !std::isspace(ch);
            }));
}

// trim from end (in place)
void CommonUtil::rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
                return !std::isspace(ch);
            })
                .base(),
            s.end());
}

// trim from both ends (in place)
void CommonUtil::trim(std::string &s)
{
    ltrim(s);
    rtrim(s);
}

bool CommonUtil::caseInsensitiveCompare(std::string str1, std::string str2)
{
    if (str1.size() != str2.size())
        return false;
    else
        return (str1.empty() | str2.empty()) ? false : std::equal(str1.begin(), str1.end(), str2.begin(), [](char a, char b) {
            return tolower(a) == tolower(b);
        });
}

void CommonUtil::_sleep(int seconds)
{
#ifdef WINDOWS
    Sleep(seconds * 1000);
#else
    sleep(seconds);
#endif
}

std::string CommonUtil::toString(std::wstring wstr)
{
    std::string str;
    std::copy(wstr.begin(), wstr.end(), str.begin());
    return str;
}

std::wstring CommonUtil::toWString(std::string str)
{
    std::wstring wstr;
    std::copy(str.begin(), str.end(), wstr.begin());
    return wstr;
}