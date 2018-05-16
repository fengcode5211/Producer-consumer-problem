#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define PRODUDERS_COUNT 3 //生产者
#define CONSUMERS_COUNT 2 //消费者
//仓库大小无上限，但是当仓库空时，消费者不能取,等待，产品大于0时，消费者可以消费

pthread_cond_t cond;
pthread_mutex_t mutex;

int g_data = 0;     //生产者生产的产品编号

void* produce(void* arg)
{
    int id = *(int*)arg;
    while(1)
    {
        pthread_mutex_lock(&mutex);
        printf("%d生产者线程生产%d\n", id, g_data+1);
        g_data++;
        sleep(rand()%3);
        printf("%d生产者线程生产%d完毕\n", id, g_data);
        pthread_cond_signal(&cond);     //每次生产完即发出通知
        pthread_mutex_unlock(&mutex);
        sleep(rand()%2);
    }
    return arg;
}

void* consumer(void* arg)
{
    int id = *(int*)arg;
    while(1)
    {
        pthread_mutex_lock(&mutex);
        while(g_data <= 0)    //条件不满足（g_data > 0）进入
        {
            printf("%d消费者等待消费\n", id);
            pthread_cond_wait(&cond, &mutex);   //等待(被信号打断，g_data 可能还是<0 ，继续等待，如果是被signal唤醒，
                                                //继续执行下面的代码),mutex为了让wait返回时解锁，防止产生死锁.
            printf("%d消费者等待到消费产品\n", id);
        }
        printf("%d消费者开始消费%d\n", id, g_data);
        //g_data--;
        sleep(rand()%3);
        printf("%d\n消费者消费%d产品完毕\n", id, g_data--);
        pthread_mutex_unlock(&mutex);
        sleep(rand()%2);
    }   
    return arg;
}


int main()
{
    int i = 0;
    srand(getpid());
    pthread_t tid[PRODUDERS_COUNT + CONSUMERS_COUNT];
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    for(; i < PRODUDERS_COUNT; i++)   //生产者线程的创建
    {   
        int *p = (int*)malloc(sizeof(int));
        *p = i;
        pthread_create(&tid[i], NULL, produce, p);
    }
    for(i = 0; i< CONSUMERS_COUNT; i++)   //消费者线程的创建
    {
        int *p = (int*)malloc(sizeof(int));
        *p = i;
        pthread_create(&tid[PRODUDERS_COUNT + i], NULL,consumer, p);
    }
    for(i = 0; i < PRODUDERS_COUNT + CONSUMERS_COUNT; i++)   //回收生产者和消费者线程
    {
        int* p = NULL;     //从哪里申请,在哪里释放
        pthread_join(tid[i], (void**)&p);
        free(p);
        p = NULL;
    }

    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);

    return 0;
}
