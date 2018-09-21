#include "../../include/ServiceProcess.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <limits.h>

int daemonize(ServiceProcess *serviceProcess)
{
    int i, lfp;
    char msg[100] = {0};

    sprintf(msg, "Starting Daemonization, Parent Process PID : %d \n", getpid());
    CommonUtil::log_message(msg);

    if (serviceProcess->config.forkWrapperAsDaemon())
    {
        i = 0; // To fork daemon
    }
    else
    {
        i = 1; // To test program in main thread
    }

    if (i == 1)
    {
        sprintf(msg, "PID : %d already a daemon \n", getpid());
        CommonUtil::log_message(msg);
        return -1; /* already a daemon */
    }

    i = fork();
    sprintf(msg, "Fork status : %d , PID : %d \n", i, getpid());
    CommonUtil::log_message(msg);

    if (i < 0)
    {
        sprintf(msg, "Error while forking :: Fork status : %d , PID : %d \n", i, getpid());
        CommonUtil::log_message(msg);
        exit(EXIT_FAILURE); /* fork error */
    }

    if (i > 0)
    {
        sprintf(msg, "This is main thread :: (Daemon) Child process id : %d , Parent PID : %d \n", i, getpid());
        CommonUtil::log_message(msg);
        sprintf(msg, "Exiting from Parent Thread (PID : %d), (Daemon) Child process (PID : %d) will continue \n", getpid(), i);
        CommonUtil::log_message(msg);
        exit(EXIT_SUCCESS); /* exit parent thread */
    }
    sprintf(msg, "After Exiting from Parent Thread. Inside (Daemon) Child process PID : %d \n", getpid());
    CommonUtil::log_message(msg);

    sprintf(msg, "Daemon thread continues with PID : %d \n", getpid());
    CommonUtil::log_message(msg);

    setsid(); /* obtain a new process group */

    for (i = getdtablesize(); i >= 0; --i)
    {
        close(i); /* close all descriptors */
    }

    i = open("/dev/null", O_RDWR);
    dup(i);     /* handle standart I/O */
    umask(027); /* set newly created file permissions */
    std::string currentpath = CommonUtil::getCurrentWorkingDir();
    chdir(serviceProcess->config.getDaemonPIDDir().c_str()); /* change running directory */
    char full_path[PATH_MAX];
    realpath(serviceProcess->config.getLockFileName().c_str(), full_path);
    const char *lockFile = full_path;
    lfp = open(lockFile, O_RDWR | O_CREAT, 0640);

    if (lfp < 0)
    {
        sprintf(msg, "Unable to open lock file : %s  :: Daemon PID : %d \n", lockFile, getpid());
        CommonUtil::log_message(msg);
        return 1; /* can not open */
    }

    if (lockf(lfp, F_TLOCK, 0) < 0)
    {
        sprintf(msg, "Unable to acquire lock over file : %s :: Daemon PID : %d \n", lockFile, getpid());
        CommonUtil::log_message(msg);
        return 0; /* can not lock */
    }

    sprintf(msg, "Lock acquired over file : %s :: Daemon PID : %d \n", lockFile, getpid());
    CommonUtil::log_message(msg);

    /* record pid to lockfile */
    // char str[10] = {0};
    // sprintf(str, "%d\n", getpid());
    // write(lfp, str, strlen(str));

    /* first instance continues */
    realpath(serviceProcess->config.getPIDFileName().c_str(), full_path);
    const char *pidFileName = full_path;
    FILE *pidfile = fopen(pidFileName, "w"); // a-> append, r-> read, w -> write, r+ -> read/update, w+ -> write/update, a+ -> append/update
    if (!pidfile)
    {
        sprintf(msg, "Unable to open PID file : %s :: Daemon PID : %d \n", pidFileName, getpid());
        CommonUtil::log_message(msg);
        return 1; /* can not open */
    }
    fprintf(pidfile, "%d\n", getpid());
    fclose(pidfile);

    sprintf(msg, "Daemon PID recorded to PID file : %s :: Daemon PID : %d \n", pidFileName, getpid());
    CommonUtil::log_message(msg);

    chdir(currentpath.c_str());
    return -1;
}

int main()
{
    //Ref to Debugging options : https://sourceware.org/gdb/onlinedocs/gdb/Forks.html
    ServiceProcess serviceProcess;

    chdir(serviceProcess.config.getWrapperWorkingDir().c_str()); /* change working directory */

    char msg[100] = {0};
    sprintf(msg, "Starting Service %s \n", serviceProcess.config.getServiceDisplayName().c_str());
    CommonUtil::log_message(msg);

    int daemonizationStatus = daemonize(&serviceProcess);
    if (daemonizationStatus > -1)
    {
        sprintf(msg, "Exiting from Daemon Process PID : %d \n", getpid());
        CommonUtil::log_message(msg);
        exit(daemonizationStatus);
    }
    sprintf(msg, "Service %s is daemonized, Daemon Process PID : %d \n", serviceProcess.config.getServiceName().c_str(), getpid());
    CommonUtil::log_message(msg);

    serviceProcess.run();
}