#include "decode_task.h"
#include "decode.h"

#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdio.h>

#define DECODE_TASK_PRIORITY (80)

#define SNDRCV_MQ "/send_receive_mq"
#define MAX_MSG_SIZE 256

#define ERROR (-1)

struct mq_attr mq_attr;

pthread_t decode_thread;
pthread_attr_t decode_attr;
struct sched_param decode_param;

void* decode_task(void* param)
{
    decode_init();

    mqd_t mymq = decode_task_init_message_queue();
    char buffer[MAX_MSG_SIZE];
    unsigned prio;
    int nbytes;
    
    while (1)
    {
        // wait for message
        nbytes = mq_receive(mymq, buffer, MAX_MSG_SIZE, &prio);
        if(nbytes== ERROR)
        {
            perror("mq_receive");
        }

        decode_run(buffer[0]);
    }
}

mqd_t decode_task_init_message_queue(void)
{
    /* setup common message q attributes */
    mq_attr.mq_maxmsg = 100;
    mq_attr.mq_msgsize = MAX_MSG_SIZE;

    mq_attr.mq_flags = 0;

    mqd_t mymq = mq_open(SNDRCV_MQ, O_CREAT | O_RDWR , S_IRWXU , &mq_attr);

    if(mymq == (mqd_t)ERROR)
        perror("mq_open");

    return mymq;
}

bool decode_task_start(void)
{
    pthread_attr_init(&decode_attr);
    pthread_attr_setinheritsched(&decode_attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&decode_attr, SCHED_FIFO);

    decode_param.sched_priority = DECODE_TASK_PRIORITY;
    pthread_attr_setschedparam(&decode_attr, &decode_param);


    pthread_create(&decode_thread,   // pointer to thread descriptor
                    &decode_attr,     // use default attributes
                    decode_task, // thread function entry point
                    NULL // parameters to pass in
                    );

    return true;
}
