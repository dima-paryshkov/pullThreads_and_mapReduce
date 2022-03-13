#include <cstdlib>
#include <iostream>
#include <cstring>
#include <pthread.h>
#include <unistd.h>
#include <cmath>

using namespace std;

#define err_exit(code, str)                                 \
    {                                                       \
        cerr << str << ": " << std::strerror(code) << endl; \
        exit(EXIT_FAILURE);                                 \
    }

const int TASKS_COUNT = 10;

int current_task = 0;
pthread_mutex_t mutex, mutex_out;

void do_task(int task_n, int thread_n)
{
    int err = pthread_mutex_lock(&mutex_out);
    if (err != 0)
        err_exit(err, "Cannot lock mutex");
    cout << "The thread № " << thread_n << " is executing task № " << task_n << endl;
    err = pthread_mutex_unlock(&mutex_out);
    if (err != 0)
        err_exit(err, "Cannot lock mutex");

    int a = 2;
    for (int i = 0; i < 1E+6; i++)
        a = pow(2, i);
}

void *thread_job(void *arg)
{
    int task_no;
    int err;
    int *thread_n = (int *)arg;

    while (true)
    {
        err = pthread_mutex_lock(&mutex);
        if (err != 0)
            err_exit(err, "Cannot lock mutex");

        task_no = current_task;
        if (*thread_n == 1)
            sleep(2);
        current_task++;
        err = pthread_mutex_unlock(&mutex);
        if (err != 0)
            err_exit(err, "Cannot unlock mutex");

        if (task_no < TASKS_COUNT)
            do_task(task_no, *thread_n);
        else
            return NULL;
    }
}

int main()
{
    int threadsCount = 5;
    int threadNumber[threadsCount];
    pthread_t thread[threadsCount];
    int err;

    err = pthread_mutex_init(&mutex, NULL);
    if (err != 0)
        err_exit(err, "Cannot initialize mutex");

    err = pthread_mutex_init(&mutex_out, NULL);
    if (err != 0)
        err_exit(err, "Cannot initialize mutex");

    for (int i = 0; i < threadsCount; i++)
    {
        threadNumber[i] = i;
        err = pthread_create(&thread[i], NULL, thread_job, (void *)&threadNumber[i]);
        if (err != 0)
            err_exit(err, "Cannot create thread");
    }
    for (int i = 0; i < threadsCount; i++)
        pthread_join(thread[i], NULL);

    pthread_mutex_destroy(&mutex);
}
