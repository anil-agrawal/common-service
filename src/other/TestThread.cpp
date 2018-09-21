#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

// Copmpile on Windows : g++ -g -D WINDOWS=\"1\" TestThread.cpp -std=c++11 -pthread
// Copmpile on Linux : g++ -g TestThread.cpp -std=c++11 -pthread

#ifdef WINDOWS
#include <windows.h>
#endif

// A normal C function that is executed as a thread
// when its name is specified in pthread_create()

void _sleep(int interval)
{
#ifdef WINDOWS
    Sleep(interval * 1000);
#else
    sleep(interval);
#endif
}

class TestThread
{
    static void *myThreadFun1Wrapper(void *vargp)
    {
        TestThread *test = (TestThread *)vargp;
        test->myThreadFun1();
    }
    static void *myThreadFun2Wrapper(void *vargp)
    {
        TestThread *test = (TestThread *)vargp;
        test->myThreadFun2();
    }

    void myThreadFun1()
    {
        int i;
        for (i = 0; i < 2; i++)
        {
            _sleep(1);
            printf("Printing from Thread 1 counter : %d \n", i);
        }

        return;
    }

    void myThreadFun2()
    {
        int i;
        for (i = 0; i < 3; i++)
        {
            _sleep(1);
            printf("Printing from Thread 2 counter : %d \n", i);
        }

        return;
    }

  public:
    void testThread()
    {
        pthread_t thread_id1, thread_id2;
        printf("Before Thread\n");
        pthread_create(&thread_id1, NULL, TestThread::myThreadFun1Wrapper, (void *)&*this);
        pthread_create(&thread_id2, NULL, TestThread::myThreadFun2Wrapper, (void *)&*this);
        pthread_join(thread_id1, NULL);
        printf("After Thread 1\n");

        // pthread_create(&thread_id2, NULL, myThreadFun2, NULL);
        pthread_join(thread_id2, NULL);
        printf("After Thread 2\n");
    }
};

// Copmpile as : gcc -pthread TestThread.cpp
int main()
{
    TestThread test;
    test.testThread();

    exit(0);
}
