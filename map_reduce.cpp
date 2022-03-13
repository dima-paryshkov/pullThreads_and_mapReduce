#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <chrono>

using namespace std;

int redRes = 0;
typedef void *(mapFunc)(void *);

struct mapData
{
    vector<int> array;
    int result;
};

void *map(void *arg)
{
    mapData *data = (mapData *)arg;
    vector<int> arr = (*data).array;
    int res = 0;
    for (int i = 0; i < arr.size(); i++)
        res += arr[i];
    (*data).result = res;
}

void reduce(mapData *data, int countThreads)
{
    for (int i = 0; i < countThreads; i++)
        redRes += data[i].result;
}

int mapReduse(vector<int> arr, mapFunc map, int threadCount)
{
    pthread_t *threads;
    threads = new pthread_t[threadCount];
    mapData *data = new mapData[threadCount];
    int err;
    int pointer = 0;
    for (int i = 0; i < threadCount; i++)
    {
        do
        {
            data[i].array.push_back(arr[pointer]);
            pointer++;
        } while (pointer % (arr.size() / threadCount) != 0);
    }

    if (pointer < arr.size())
    {
        while (pointer != arr.size())
        {
            data[threadCount - 1].array.push_back(arr[pointer]);
            pointer++;
        }
    }
    for (int i = 0; i < threadCount; i++)
    {
        err = pthread_create(&threads[i], NULL, map, &data[i]);
        if (err != 0)
            cout << "Cannot create thread " << i << endl;
    }
    for (int i = 0; i < threadCount; i++)
        pthread_join(threads[i], NULL);

    vector<int> mapRes;
    for (int i = 0; i < threadCount; i++)
        mapRes.push_back(data[i].result);

    reduce(data, threadCount);
}

int main()
{
    vector<int> arr;
    int threadCount, arrSize = 1E+6;
    // std::cout << "Enter array size\n";
    // cin >> arrSize;
    cout << "Enter quantity of threads\n";
    cin >> threadCount;
    if (threadCount > arrSize)
        threadCount = arrSize;
    for (int i = 0; i < arrSize; i++)
        arr.push_back(i);

    auto begin = chrono::steady_clock::now();

    mapReduse(arr, map, threadCount);

    auto end = chrono::steady_clock::now();
    auto time = chrono::duration_cast<std::chrono::microseconds>(end - begin);

    // cout << "Answer 1:\t" << redRes << endl;
    cout << "Time 1:  \t" << time.count() << endl;

    // begin = chrono::steady_clock::now();
    // int sum = 0;
    // for (int i = 0; i < arrSize; i++)
    //     sum += arr[i];
    // end = chrono::steady_clock::now();
    // time = chrono::duration_cast<std::chrono::microseconds>(end - begin);

    // cout << "Time 2:  \t" << time.count() << endl;
    // cout << "Answer 2:\t" << sum << endl;
}