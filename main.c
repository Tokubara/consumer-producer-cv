#include <stdio.h>
#include <pthread.h>
#include <assert.h>

int buffer;
int counter = 0; // initially, empty

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
    for ( int i = 0; i < *ntasks; ++i )
        put(i);
    return NULL;
}

void *consumer(void *arg)
{
    int *ntasks = (int *) arg;
    for ( int i = 0; i < *ntasks; ++i )
    {
        printf("get: %d\n", get());
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
