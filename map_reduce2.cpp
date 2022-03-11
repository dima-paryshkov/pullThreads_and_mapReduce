#include <pthread.h>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <limits>
#include <list>

using namespace std;

#define err_exit(code, str)                        \
  {                                                \
    cerr << str << ": " << strerror(code) << endl; \
    exit(EXIT_FAILURE);                            \
  }

pthread_spinlock_t spinlock;  // Спинлок
std::list<int> map_result = {};

int array_sum = 0;

void *MapFunction(void *arg) {
  int d = *(int *)arg;
  int err;
  err = pthread_spin_lock(&spinlock);
  if (err != 0) err_exit(err, "Cannot lock spinlock");
  map_result.push_front(d);
  err = pthread_spin_unlock(&spinlock);
  if (err != 0) err_exit(err, "Cannot unlock spinlock");
}

void *ReduceFunction(void *arg) {
  int d = *(int *)arg;
  int err;
  err= pthread_spin_lock(&spinlock);
  if (err != 0) err_exit(err, "Cannot lock spinlock");

  array_sum+=d;
  err=pthread_spin_unlock(&spinlock);
  if (err != 0) err_exit(err, "Cannot unlock spinlock");
}

template <class FuncMap, class FuncReduce>
void MapReduce(int *array, FuncMap Map, FuncReduce Reduce,
               const int num_threads, const int array_size) {
  int err;
  auto threads = new pthread_t[num_threads];
  auto thread_attrs = new pthread_attr_t[num_threads];

  int parts = ceil((double)array_size / num_threads);

  for (int i = 0; i < num_threads; i++) {
    err = pthread_attr_init(&thread_attrs[i]);
    if (err != 0) err_exit(err, "Cannot create thread attribute");
    err = pthread_attr_setdetachstate(
        &thread_attrs[i], PTHREAD_CREATE_JOINABLE);  // set detach state
    if (err != 0) err_exit(err, "Setting detach state attribute failed");
    err = pthread_attr_setstacksize(&thread_attrs[i], 1 * 1024 * 1024);
    if (err != 0) err_exit(err, "Setting stack size attribute failed");
  }

  int num_elem_to_calc = 0;
  for (int i = 0; i < parts; i++) {
    if ((array_size - i * num_threads) < num_threads)
      num_elem_to_calc = array_size - i * num_threads;
    else
      num_elem_to_calc = num_threads;
    for (int j = 0; j < num_elem_to_calc; j++) {
      err =
          pthread_create(&threads[j], &thread_attrs[j], Map,
                         (void *)&array[i * num_threads + j]);  // start threads
      if (err != 0) err_exit(err, "Cannot create a thread");
    }
    for (int j = 0; j < num_elem_to_calc; j++) {
      err = pthread_join(threads[j], NULL);  // wait for threads completion
      if (err != 0) err_exit(err, "Cannot join a thread");
    }
  }

  parts = ceil((double)map_result.size() / num_threads);
  auto iter = map_result.begin();
  for (int i = 0; i < parts; i++) {
    if ((map_result.size() - i * num_threads) < num_threads)
      num_elem_to_calc = map_result.size() - i * num_threads;
    else
      num_elem_to_calc = num_threads;
    for (int j = 0; j < num_elem_to_calc; j++) {
      
      err = pthread_create(&threads[j], &thread_attrs[j], Reduce,
                           (void *)&(*iter));  // start threads
     
      if (err != 0) err_exit(err, "Cannot create a thread");
      iter++;
    }
    for (int j = 0; j < num_elem_to_calc; j++) {
      err = pthread_join(threads[j], NULL);  // wait for threads completion
      if (err != 0) err_exit(err, "Cannot join a thread");
    }
  }
}

int main() {
  std::srand(std::time(nullptr));
  int err;  // Код ошибки
            // Инициализируем спинлок
  err = pthread_spin_init(&spinlock, NULL);
  
  if (err != 0) err_exit(err, "Cannot initialize spinlock");

  int array_size, num_threads;
  std::cout << "Enter array size: ";
  std::cin >> array_size;
  std::cout << "Enter the amount of threads: ";
  std::cin >> num_threads;
  auto arr = new int[array_size];
  for (int i = 0; i < array_size; i++) arr[i] = 1 + std::rand() % array_size;
  MapReduce(arr, MapFunction, ReduceFunction, num_threads, array_size);
  pthread_spin_destroy(&spinlock);
  
  for (int n : map_result) std::cout << n << "\t";
  std::cout << std::endl;
  std::cout << "Array sum: " << array_sum;
}