#include <cstdlib>
#include <iostream>
#include <cstring>
#include <pthread.h>
#include <cmath>

using namespace std;

#define err_exit(code, str)                                 \
    {                                                       \
        cerr << str << ": " << std::strerror(code) << endl; \
        exit(EXIT_FAILURE);                                 \
    }

const int TASKS_COUNT = 10;
int task_list[TASKS_COUNT];

int current_task = 0;
pthread_mutex_t mutex;

pthread_cond_t cond;

void do_task(int task_n, int thread_n)
{
    cout << "The thread № " << thread_n << " is executing task № " << task_n << endl;
    int a = 2;
    for (int i = 0; i < 1E+6; i++)
    {
        a = pow(2, i);
    }
}

void *cond_thread_job(void *arg)
{
    int err = pthread_mutex_lock(&mutex);
    if (err != 0)
        err_exit(err, "Cannot lock mutex");

    pthread_cond_wait(&cond, &mutex);

    cout << "The program is closing." << endl;

    err = pthread_mutex_unlock(&mutex);
    if (err != 0)
        err_exit(err, "Cannot unlock mutex");
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
    pthread_t thread1, thread2;
    const int thread_1_number = 1, thread_2_number = 2;
    int err;

    for (int i = 0; i < TASKS_COUNT; ++i)
        task_list[i] = rand() % TASKS_COUNT;

    err = pthread_mutex_init(&mutex, NULL);
    if (err != 0)
        err_exit(err, "Cannot initialize mutex");

    err = pthread_cond_init(&cond, NULL);
    if (err != 0)
        err_exit(err, "Cannot initialize condition");

    pthread_t cond_thread;
    err = pthread_create(&cond_thread, NULL, cond_thread_job, NULL);
    if (err != 0)
        err_exit(err, "Cannot create thread 2");

    pthread_cond_signal(&cond);

    pthread_join(cond_thread, NULL);

    err = pthread_create(&thread1, NULL, thread_job, (void *)&thread_1_number);
    if (err != 0)
        err_exit(err, "Cannot create thread 1");
    err = pthread_create(&thread2, NULL, thread_job, (void *)&thread_2_number);
    if (err != 0)
        err_exit(err, "Cannot create thread 2");
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
}
