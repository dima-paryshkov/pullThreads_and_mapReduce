#include <cstdlib>
#include <iostream>
#include <cstring>
#include <pthread.h>
#include <cmath>

using namespace std;

#define err_exit(code, str) { cerr << str << ": " << std::strerror(code) << endl; exit(EXIT_FAILURE); }

const int TASKS_COUNT = 10;
int task_list[TASKS_COUNT]; // Массив заданий

int current_task = 0; // Указатель на текущее задание
pthread_mutex_t mutex; // Мьютекс

void do_task(int task_n, int thread_n)
{
    cout << "The thread № " << thread_n << " is executing task № " << task_n << endl;
    int a = 2;
    for (int i = 0; i < 1E+6; i++)
    {
        a = pow(2, i);
    }  
}

void *thread_job(void *arg)
{
    int task_no;
    int err;
    int *thread_n = (int *)arg;
    // Перебираем в цикле доступные задания
    while(true) 
    {
        // Захватываем мьютекс для исключительного доступа
        // к указателю текущего задания (переменная
        // current_task)
        //err = pthread_mutex_lock(&mutex);
        // if(err != 0)
            // err_exit(err, "Cannot lock mutex");
        // Запоминаем номер текущего задания, которое будем исполнять
        task_no = current_task;
        // Сдвигаем указатель текущего задания на следующее
        current_task++;
        // Освобождаем мьютекс
        // err = pthread_mutex_unlock(&mutex);
        // if(err != 0)
            // err_exit(err, "Cannot unlock mutex");
        // Если запомненный номер задания не превышает
        // количества заданий, вызываем функцию, которая
        // выполнит задание.
        // В противном случае завершаем работу потока
        if(task_no < TASKS_COUNT)
            do_task(task_no, *thread_n);
        else
            return NULL;
    }
}

int main()
{
    pthread_t thread1, thread2; // Идентификаторы потоков
    const int thread_1_number = 1, thread_2_number = 2;
    int err; // Код ошибки
    // Инициализируем массив заданий случайными числами
    for(int i=0; i<TASKS_COUNT; ++i)
        task_list[i] = rand() % TASKS_COUNT;
    // Инициализируем мьютекс
    err = pthread_mutex_init(&mutex, NULL);
    if(err != 0)
        err_exit(err, "Cannot initialize mutex");
    // Создаём потоки
    err = pthread_create(&thread1, NULL, thread_job, (void *)&thread_1_number);
    if(err != 0)
    err_exit(err, "Cannot create thread 1");
        err = pthread_create(&thread2, NULL, thread_job, (void *)&thread_2_number);
    if(err != 0)
        err_exit(err, "Cannot create thread 2");
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    // Освобождаем ресурсы, связанные с мьютексом
    pthread_mutex_destroy(&mutex);
}
