#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <iomanip>
#include <cstring>
#include <pthread.h>
#include <math.h>
#include <chrono>

using namespace std;

#define err_exit(code, str) { cerr << str << ": " << strerror(code) << endl; exit(EXIT_FAILURE); }

vector<int> basePrimeNumbers;
vector<int> result;
pthread_mutex_t mutex;

class ThreadPool 
{
private:
    vector<pthread_t> mThreads;

    queue<void*> mTaskArgs;

    void* (*mTaskPtr) (void*);
 
    pthread_mutex_t mMutex;
 
    // Внутренняя функция для потоков из пула
    void* threadJob(void* classContext) 
    {
        int err;
        size_t queueSize;
        void* taskArg;
 
        while(true) 
        {
            err = pthread_mutex_lock(&mMutex);
            if (err != 0)
                err_exit(err, "Cannot lock mutex"); 
            
            queueSize = mTaskArgs.size();
            
            if (queueSize > 0) 
            {
                taskArg = mTaskArgs.front();
                mTaskArgs.pop();
            }
            
            err = pthread_mutex_unlock(&mMutex);
            if (err != 0)
                err_exit(err, "Cannot unlock mutex");

            if (queueSize > 0) 
                mTaskPtr(taskArg);
            else 
                return NULL;
        }
    }
 
public:
    ThreadPool(void* (*taskPtr) (void*), const size_t threadCount) 
    {
 
        int err = pthread_mutex_init(&mMutex, NULL);
        if (err != 0)
            err_exit(err, "Cannot initialize mutex");
 
        mTaskPtr = taskPtr;
 
        for (size_t i = 0; i < threadCount; i++) 
        {
            pthread_t thread;
            mThreads.push_back(thread);
        }
    }
 
    void addTask(void* taskArg) 
    {
        mTaskArgs.push(taskArg);
    }
 
    void run() 
    {
        int err;
        for (size_t i = 0; i < mThreads.size(); i++) 
        {
            err = pthread_create(&mThreads[i], NULL, (void*(*)(void*))&ThreadPool::threadJob, (void*) this);
            if (err != 0)
                err_exit(err, "Cannot create thread");            
        }
 
        for (size_t i = 0; i < mThreads.size(); i++) {
            err = pthread_join(mThreads[i], NULL);
            if (err != 0)
                err_exit(err, "Cannot join thread");
        }
    }
};

void* checkNumber(void *arg) 
{
    int params = (int&)arg;

    int j;
    for (j = 1; j < basePrimeNumbers.size() - 1; j++)
        if (params % basePrimeNumbers[j] == 0)
            break;

    if (j == basePrimeNumbers.size() - 1)
    {
        int err = pthread_mutex_lock(&mutex);
            if (err != 0)
                err_exit(err, "Cannot lock mutex");

        result.push_back(params);
        err = pthread_mutex_unlock(&mutex);
            if (err != 0)
                err_exit(err, "Cannot unlock mutex");
    }
}

void getBasePrimeNumbers(int n)
{
    basePrimeNumbers.push_back(1);
    basePrimeNumbers.push_back(2);
    for (int i = 3; i < sqrt(n); i += 2)
    {
        for (int j = 1; j < basePrimeNumbers.size() - 1; j++)
            if (i % basePrimeNumbers[j] == 0)
                break;
        basePrimeNumbers.push_back(i);
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2) 
    {
        cout << "Incorrect arguments:" << endl;
        exit(0);
    }

    int n = atoi(argv[1]);

    if (n < 2) 
    {
        cout << "Wrong value of argument. n > 1" << endl;
        exit(0);
    }

    getBasePrimeNumbers(n);

    int threadsCount = 4;
    ThreadPool threadPool(&checkNumber, threadsCount);

    auto begin = chrono::steady_clock::now();
    int err = pthread_mutex_init(&mutex, NULL);
    if (err != 0)
        err_exit(err, "Cannot initialize mutex");

    for (int i = sqrt(n) + 1; i < n; i++)
    {
        threadPool.addTask((void*)(i));
    }
    threadPool.run();

    auto end = chrono::steady_clock::now();
    auto time = chrono::duration_cast<std::chrono::microseconds>(end - begin);
    cout << "Time to get prime numbers with parallelization: " << time.count() <<  endl;

    begin = chrono::steady_clock::now();

    vector<int> tmp;
    for (int i = 1; i <= n; i++)
        tmp.push_back(i);
    
    for (int i = 1; i < sqrt(n) + 1; i++)
        for (int j = i + 1; j < tmp.size(); j++)
            if (tmp[j] % tmp[i] == 0)
                tmp.erase(tmp.begin() + j);

    end = chrono::steady_clock::now();
    time = chrono::duration_cast<std::chrono::microseconds>(end - begin);
    cout << "Time to get prime numbers classic methods: " << time.count() <<  endl;

    // for (int i = 0; i < basePrimeNumbers.size(); i++)
        // cout << basePrimeNumbers[i] << " ";

    // for (int i = 0; i < result.size(); i++)
        // cout << result[i] << " ";
    // cout << endl; 
    
    exit(1);


}
