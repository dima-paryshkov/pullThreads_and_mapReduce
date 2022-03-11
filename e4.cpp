#include <cstdlib>
#include <iostream>
#include <cstring>
#include <pthread.h>
#include <cmath>
#include <chrono>

using namespace std;

#define err_exit(code, str) { cerr << str << ": " << std::strerror(code) << endl; exit(EXIT_FAILURE); }

enum SyncTypes { LACK /*no sync*/, MUTEX, SPINLOCK };  
SyncTypes SyncType = SyncTypes::LACK;

const int TASKS_COUNT = 30;
int current_task = 0;

pthread_mutex_t mutex;
pthread_spinlock_t spinlock;

void do_task()
{
    int a = 2;
    for (int i = 0; i < 1E+2; i++)
    {
        a += pow(2, i);
    }  
}

void *thread_job(void *arg)
{
    int task;
    int err;
    int *thread_n = (int *)arg;
    
    while(true) 
    {
        switch (SyncType)
        {
        case LACK:
            /*no synchronization*/
            task = current_task;
            current_task++;
            break;

        case MUTEX:
            err = pthread_mutex_lock(&mutex);
            if (err != 0)
                err_exit(err, "Cannot lock mutex");
            task = current_task;
            current_task++;
            err = pthread_mutex_unlock(&mutex);
            if (err != 0)
                err_exit(err, "Cannot lock mutex");
            break;

        case SPINLOCK:
            err = pthread_spin_lock(&spinlock);
            if (err != 0)
                err_exit(err, "Cannot lock mutex");
            task = current_task;
            current_task++;
            err = pthread_spin_unlock(&spinlock);
            if (err != 0)
                err_exit(err, "Cannot lock mutex");
                break;
        }
        if (task < TASKS_COUNT)
            do_task();
        else
            return NULL;
    }
}

int main(int argc, char* argv[])
{
    int err;
    
    switch (SyncType)
    {
    case LACK:
        /*no synchronization*/
        break;

    case MUTEX:
        err = pthread_mutex_init(&mutex, NULL);
        if (err != 0)
            err_exit(err, "Cannot lock mutex");
        break;

    case SPINLOCK:
        err = pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);
        if (err != 0)
            err_exit(err, "Cannot lock mutex");
            break;
    }

    int threads_count;

    if (argc < 3) 
    {
        cout << "Too few program arguments:" << endl;
        cout << "Need to specify the count of threads." << endl;
        exit(0);
    }

    if (atoi(argv[1]) < 0 || atoi(argv[1]) > 3) 
    {
        cout << "Wrong mode of operation. 0 - no synchronization, 1 - mutex, 2 - spinlock." << endl;
        exit(0);
    }
    else 
        SyncType = (SyncTypes)atoi(argv[1]);
 
    threads_count = atoi(argv[2]);
    if (threads_count <= 0) 
    {
        cout << "Count of threads must be > 0" << endl;
        exit(0);
    }
 
 
    pthread_t* threads = new pthread_t[threads_count];

    err = pthread_mutex_init(&mutex, NULL);
    if (err != 0)
        err_exit(err, "Cannot initialize mutex");

    err = pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);
    if (err != 0)
        err_exit(err, "Cannot initialize spinlock");

    auto begin = chrono::steady_clock::now();

    for (int i = 0; i < threads_count; i++) 
    {
        err = pthread_create(threads + i, NULL, &thread_job, NULL);
        if (err != 0) 
        {
            char err_str[35];
            snprintf(err_str, strlen(err_str) * sizeof(char), "Cannot create thread %d", i);
            err_exit(err, err_str);
        }
    }

    for (int i = 0; i < threads_count; i++) 
        pthread_join(threads[i], NULL);

    auto end = chrono::steady_clock::now();

    auto time = chrono::duration_cast<std::chrono::microseconds>(end - begin);
    //cout << "Program running time " << time.count() <<  endl;
    cout << time.count();

    pthread_mutex_destroy(&mutex);
    pthread_spin_destroy(&spinlock);
    delete[] threads;
}