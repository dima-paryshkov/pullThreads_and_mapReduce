#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <vector>
#include <iostream>

using namespace std;

int redRes = 0;
typedef void *(mapFunc)(void*);
typedef void *(reduceFunc)(void*);

struct mapData
{
    vector<int> array;
    int result;
};

void *map(void *arg)
{
    mapData *data = (mapData*) arg;
    vector<int> arr = (*data).array;
    int res = 0;
    for (int i = 0; i < arr.size(); i++)
        res += arr[i];
    (*data).result = res;
}

void *reduce(void *arg)
{
    mapData *data = (mapData*) arg;
    int mapRes = (*data).result;
    redRes += mapRes;
}

int mapReduse(vector<int> arr, mapFunc map, reduceFunc reduce, int threadCount)
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
        }
        while (pointer % (arr.size() / threadCount) != 0 );
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
        if(err != 0)
            cout << "Cannot create thread " << i << endl;
    }
    for (int i = 0; i < threadCount; i++)
        pthread_join(threads[i], NULL);

    vector<int> mapRes;
    for (int i = 0; i < threadCount; i++)
        mapRes.push_back(data[i].result);
    
    for (int i = 0; i < threadCount; i++)
    {
        err = pthread_create(&threads[i], NULL, reduce, &data[i]);
        if(err != 0)
            cout << "Cannot create thread " << i << endl;
    }
    for (int i = 0; i < threadCount; i++)
        pthread_join(threads[i], NULL);

}

int main()
{
    vector<int> arr;
    int threadCount, arrSize;
    std::cout << "Enter arrey size\n";
    cin >> arrSize;
    cout << "Enter quantity of threads\n";
    cin >> threadCount;
    if (threadCount > arrSize)
        threadCount = arrSize;
    for (int i = 0; i<arrSize; i++)
        arr.push_back(i);
    mapReduse(arr, map, reduce, threadCount);
    cout << "Answer:" << redRes << endl;
}
