#include <stdio.h>
#include <pthread.h>
#include <assert.h>

int buffer;
int counter = 0; // initially, empty

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void put(int value)
{
    assert(counter == 0);
    counter = 1;
    buffer = value; // 这两句得用锁保护起来, 确保捆绑执行
}

int get()
{
    assert(counter == 1);
    int ret_buffer = buffer;
    counter = 0;
    return ret_buffer;
}

void *producer(void *arg)
{
    int *ntasks = (int *)arg;
    for ( int i = 0; i < *ntasks; ++i ) {
      pthread_mutex_lock(&mutex);
      if(counter==1) {
        pthread_cond_wait(&cond, &mutex);
      }
      put(i);
      pthread_cond_signal(&cond);
      pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *consumer(void *arg)
{
    int *ntasks = (int *) arg;
    for ( int i = 0; i < *ntasks; ++i ) {
      pthread_mutex_lock(&mutex);
      if(counter==0) {
        pthread_cond_wait(&cond, &mutex);
      }
      printf("get: %d\n", get());
      pthread_cond_signal(&cond);
      pthread_mutex_unlock(&mutex);
        // do something
    }
    return NULL;
}

int main() {
  pthread_t   tid1, tid2;
  int ntasks = 10;
  pthread_create(&tid1, NULL, consumer, &ntasks);
  pthread_create(&tid2, NULL, producer, &ntasks);
  pthread_join(tid1,NULL);
  pthread_join(tid2,NULL);
}
