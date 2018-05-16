#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
//仓库中装产品编号，没装产品的位置，置为-1，装了的地方置为产品的编号


#define PRO_COUNT 3
#define CON_COUNT 2
#define BUFSIZE 5


sem_t sem_full;     //标识可生产的产品个数
sem_t sem_empty;    //表示可消费的产品个数
pthread_mutex_t mutex;  //互斥量
int num = 0;        //产品编号
int buf[BUFSIZE];   //仓库
int wr_idx;     //写索引
int rd_idx;     //读索引

void* pro(void* arg)
{
  int i = 0;
  int id = *(int*)arg;
  free(arg);
  while(1)
  {
      sem_wait(&sem_full);    //先判断仓库是否满
      pthread_mutex_lock(&mutex); //互斥的访问具体的仓库的空闲位置
      printf("%d生产者开始生产%d\n", id, num);
      for(i = 0; i < BUFSIZE; i++)
      {
        printf("\tbuf[%d]=%d", i, buf[i]);
        if(i == wr_idx)
        {
          printf("<=====");
        }
        printf("\n");
      }
      buf[wr_idx] = num++;      //存放产品
      wr_idx = (wr_idx + 1) % BUFSIZE;
      printf("%d生产者结束生产\n", id);
      pthread_mutex_unlock(&mutex);
      sem_post(&sem_empty);
      sleep(rand()%3);
    }
}

void* con(void* arg)
{
  int i = 0;
    int id = *(int*)arg;
    free(arg);
    while(1)
    {
        sem_wait(&sem_empty);
        pthread_mutex_lock(&mutex);
        
        printf("%d消费者开始消费%d\n", id, num);
        for(i = 0; i < BUFSIZE; i++)
        {
          printf("buf[%d]=%d", i, buf[i]);
          if(i == rd_idx)
          {
            printf("=====>");
          }
          printf("\n");
        }
        int r = buf[rd_idx];
        buf[rd_idx] = -1;
        rd_idx = (rd_idx+1)%BUFSIZE;
        sleep(rand()%4);
        printf("%d\n消费者消费完%d\n", id, r);
        pthread_mutex_unlock(&mutex);
        sem_post(&sem_full);
        sleep(rand()%2);
    }
}

int main()
{
    pthread_t tid[PRO_COUNT+CON_COUNT];
    pthread_mutex_init(&mutex, NULL); //初始化
    sem_init(&sem_empty, 0, 0);
    sem_init(&sem_full, 0, BUFSIZE);
    srand(getpid());

    int i = 0;
    for(i = 0; i < BUFSIZE; i++)      //初始化仓库  -1表示没有品
        buf[i] = -1;

    for(i = 0; i < PRO_COUNT; i++)    //产生生产者
    {
        int *p = (int*)malloc(sizeof(int));
        *p = i;
        pthread_create(&tid[i], NULL, pro, p);
    }

    for(i = 0; i < CON_COUNT; i++)
    {
        int *p = (int*)malloc(sizeof(int));
        *p = i;
        pthread_create(&tid[i+CON_COUNT], NULL, con, p);
    }
    
    for(i = 0; i < PRO_COUNT + CON_COUNT; i++)
    {
        pthread_join(tid[i], NULL);
    }

    pthread_mutex_destroy(&mutex);  //销毁
    sem_destroy(&sem_empty);
    sem_destroy(&sem_full);

    return 0;
}
