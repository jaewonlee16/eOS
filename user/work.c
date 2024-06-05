#include <core/eos.h>

#define STACK_SIZE 8096

int32u_t stack0[STACK_SIZE]; // stack for task0
int32u_t stack1[STACK_SIZE]; // stack for task1
int32u_t stack2[STACK_SIZE]; // stack for task2

eos_tcb_t tcb0; // tcb for task0
eos_tcb_t tcb1; // tcb for task1
eos_tcb_t tcb2; // tcb for task2

static int32u_t queue0[10];
static int32u_t queue1[10];
eos_mqueue_t mq0;
eos_mqueue_t mq1;

/*
void task0() {
    while(1) {
        PRINT("A\n");
        eos_sleep(0);
    }
}

void task1() {
    while(1) {
        PRINT("B\n");
        eos_sleep(0);
    }
}

void task2() {
    while(1) {
        PRINT("C\n");
        eos_sleep(0);
    }
}

void eos_user_main() {
    eos_create_task(&tcb0, stack0, STACK_SIZE, task0, NULL, 1); 
    eos_set_period(&tcb0, 2);
    
    eos_create_task(&tcb1, stack1, STACK_SIZE, task1, NULL, 10); 
    eos_set_period(&tcb1, 4);

    eos_create_task(&tcb2, stack2, STACK_SIZE, task2, NULL, 50); 
    eos_set_period(&tcb2, 8);
}

*/
static void sender_task(void* arg) {
	int32u_t* data = "xy";

	while(1) {
		PRINT("send message to mq0\n");
		eos_send_message(&mq0, data, 0);
		PRINT("send message to mq1\n");
		eos_send_message(&mq1, data, 0);
		eos_sleep(0);
	}
}

static void receiver_task0(void* arg) {
	int32u_t data[2];

	while(1) {
		PRINT("receive message from mq0\n");
		eos_receive_message(&mq0, data, 0);
		PRINT("received message: %s\n", data);
		eos_sleep(0);
	}
}

static void receiver_task1(void* arg) {
	int32u_t data[2];

	while(1) {
		PRINT("receive message from mq1\n");
		eos_receive_message(&mq1, data, 0);
		PRINT("received message: %s\n", data);
		eos_sleep(0);
	}
}

void eos_user_main() {
    eos_create_task(&tcb0, stack0, STACK_SIZE, sender_task, NULL, 50); 
    eos_set_period(&tcb0, 2);
    
    eos_create_task(&tcb1, stack1, STACK_SIZE, receiver_task0, NULL, 10); 
    eos_set_period(&tcb1, 4);

    eos_create_task(&tcb2, stack2, STACK_SIZE, receiver_task1, NULL, 20); 
    eos_set_period(&tcb2, 6);

    eos_init_mqueue(&mq0, queue0, 5, 3, 1);
	eos_init_mqueue(&mq1, queue1, 5, 3, 1);
}