#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#define BUFFER_SIZE 32

int buffer[BUFFER_SIZE];
int c_pos=0, p_pos=0;
int counter = 0; // initially, empty

pthread_cond_t cond_c = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_p = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void put(int value)
{
    assert(counter<BUFFER_SIZE);
    counter++;
    p_pos=(p_pos+1)%BUFFER_SIZE;
    buffer[p_pos] = value; // 这两句得用锁保护起来, 确保捆绑执行
}

int get()
{
    assert(counter > 0);
    counter--;
    c_pos=(c_pos+1)%BUFFER_SIZE;
    return buffer[c_pos];
}

void *producer(void *arg)
{
    int *ntasks = (int *)arg;
    for ( int i = 0; i < *ntasks; ++i ) {
      pthread_mutex_lock(&mutex);
      while(counter==BUFFER_SIZE) {
        pthread_cond_wait(&cond_p, &mutex);
      }
      put(i);
      pthread_cond_signal(&cond_c);
      pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *consumer(void *arg)
{
    int *ntasks = (int *) arg;
    for ( int i = 0; i < *ntasks; ++i ) {
      pthread_mutex_lock(&mutex);
      while(counter==0) {
        pthread_cond_wait(&cond_c, &mutex);
      }
      printf("get: %d\n", get());
      pthread_cond_signal(&cond_p);
      pthread_mutex_unlock(&mutex);
        // do something
    }
    return NULL;
}

int main() {
  pthread_t   tid1, tid2, tid3;
  int ntasks_1 = 10;
  pthread_create(&tid1, NULL, consumer, &ntasks_1);
  int ntasks_2 = 10;
  pthread_create(&tid2, NULL, consumer, &ntasks_2);
  int ntasks_3 = 20;
  pthread_create(&tid3, NULL, producer, &ntasks_3);
  pthread_join(tid1,NULL);
  pthread_join(tid2,NULL);
}
