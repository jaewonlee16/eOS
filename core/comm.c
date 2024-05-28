/********************************************************
 * Filename: core/comm.c
 *
 * Author: Jiyong Park, RTOSLab. SNU
 * Modified by: Seongsoo Hong on 03/30/24
 *
 * Description: Message queue management
 ********************************************************/

#include <core/eos.h>


void eos_init_mqueue(eos_mqueue_t *mq, void *queue_start, int16u_t queue_size, int8u_t msg_size, int8u_t queue_type){
    // To be filled by students: Project 4
    // PRINT("message queue: 0x%x at 0x%x || queue_size: %d, msg_size: %d\n", mq, queue_start, queue_size, msg_size)
    mq->queue_size = queue_size;
    mq->msg_size = msg_size;

    mq->queue_start = queue_start;
    mq->front = queue_start;
    mq->rear = queue_start;

    mq->queue_type = queue_type;
    eos_init_semaphore(&(mq->putsem), queue_size, queue_type);
    eos_init_semaphore(&(mq->getsem), 0, queue_type);
}


int8u_t eos_send_message(eos_mqueue_t *mq, void *message, int32s_t timeout) 
{
    // To be filled by students: Project 4
    if (!eos_acquire_semaphore((&mq->putsem), timeout))  return;

    // read the message by 1 byte
    char* msg = (char*)message;

    for (int i = 0; i < mq->msg_size; i++) {
        // copy the message to rear
        *(char*)(mq->rear) = msg[i];

        // Change rear by increasing and check the 
        // pointer is in range of queue by dividing with queue size
        mq->rear = (mq->rear + 1 - mq->queue_start) 
            % (mq->queue_size * mq->msg_size + 1) + mq->queue_start;
    }
    // release semaphore
    eos_release_semaphore(&mq->getsem);
}


int8u_t eos_receive_message(eos_mqueue_t *mq, void *message, int32s_t timeout)
{
    // To be filled by students: Project 4
    if (!eos_acquire_semaphore(&(mq->getsem), timeout)) return;

    // read the message by 1 byte
    char* msg = (char*)message;

    for (int i =0; i < mq->msg_size; i++) {
        // get the message from the front part of queue
        msg[i] = *((char*)(mq->front));

        // Change front by increasing and check the 
        // pointer is in range of queue by dividing with queue size
        mq->front = (mq->front + 1 - mq->queue_start) 
            % (mq->queue_size * mq->msg_size + 1) + mq->queue_start;
    }
    // release semaphore
    eos_release_semaphore(&mq->putsem);
}
