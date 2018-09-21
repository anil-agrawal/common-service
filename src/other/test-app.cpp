// Command : g++ -DWINDOWS=\"1\" -o test test.cpp
// Command : g++ -o test.exe test.cpp
#include <stdio.h>
#include <cstdlib>

//#define WINDOWS /* uncomment this line to use it for windows.*/

#ifdef WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#include <signal.h>
#endif

#define LOG_FILE "test-app.log"

void log_message(char *message)
{

    // printf(message);
    FILE *logfile;
    logfile = fopen(LOG_FILE, "a");
    if (!logfile)
    {
        return;
    }
    fprintf(logfile, "%s\n", message);
    fclose(logfile);
}

#ifdef WINDOWS
BOOL WINAPI signal_handler(DWORD dwType)
{

    char msg[100] = {0};
    sprintf(msg, "Signal recieved : %d \n", dwType);
    log_message(msg);

    switch (dwType)
    {
    case CTRL_C_EVENT:
    {
        log_message("CTRL_C_EVENT signal catched");
        break;
    }
    case CTRL_BREAK_EVENT:
    {
        log_message("CTRL_BREAK_EVENT signal catched");
        break;
    }
    case CTRL_CLOSE_EVENT:
    {
        log_message("CTRL_CLOSE_EVENT signal catched");
        break;
    }
    case CTRL_LOGOFF_EVENT:
    {
        log_message("CTRL_LOGOFF_EVENT signal catched");
        break;
    }
    case CTRL_SHUTDOWN_EVENT:
    {
        log_message("CTRL_SHUTDOWN_EVENT signal catched");
        //set_done(); //signal the main thread to terminate

        //Returning would make the process exit!
        //We just make the handler sleep until the main thread exits,
        //or until the maximum execution time for this handler is reached.
        Sleep(1000);

        return TRUE;
    }
    default:
    {
        log_message("other shutdown signal catched : : ");
        break;
    }
    }
    return FALSE;
}
#else
//To test signal:	kill -HUP pid
//To terminate:	kill pid

void signal_handler(int sig)
{
    switch (sig)
    {
    case SIGHUP:
    {
        log_message("hangup signal catched");
        break;
    }
    case SIGTERM:
    {
        log_message("terminate signal catched");
        exit(0);
        break;
    }
    case SIGTTOU:
    {
        log_message("SIGTTOU signal catched");
        break;
    }
    case SIGTTIN:
    {
        log_message("SIGTTIN signal catched");
        break;
    }
    }
}
#endif

int main()
{

#ifdef WINDOWS
    SetConsoleCtrlHandler(signal_handler, TRUE);
#else
    signal(SIGCHLD, SIG_IGN); /* ignore child */
    signal(SIGTSTP, SIG_IGN); /* ignore tty signals */
    signal(SIGTTOU, signal_handler);
    signal(SIGTTIN, signal_handler);
    signal(SIGHUP, signal_handler);  /* catch hangup signal */
    signal(SIGTERM, signal_handler); /* catch kill signal */
#endif

    char msg[100] = {0};
    int i;
    while (1)
    {
        sprintf(msg, "Inside test.c with counter : %d", ++i);
        log_message(msg);
#ifdef WINDOWS
        Sleep(2000);
#else
        sleep(2);
#endif
    }
}