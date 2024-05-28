/********************************************************
 * Filename: core/sync.c
 *
 * Author: Jiyong Park, RTOSLab. SNU
 * Modified by: Seongsoo Hong on 03/31/24
 *
 * Description: Routines for semaphores and condition variables
 ********************************************************/

#include <core/eos.h>
#define WAITING 3

void eos_init_semaphore(eos_semaphore_t *sem, int32u_t initial_count, int8u_t queue_type)
{
    // To be filled by students: Project 4
    sem->count = initial_count;
	sem->wait_queue = NULL;
	sem-> queue_type = queue_type;
}


int32u_t eos_acquire_semaphore(eos_semaphore_t *sem, int32s_t timeout)
{
    // To be filled by students: Project 4
    hal_disable_interrupt();

	// if the resource is available
	if (sem->count > 0) {
		sem->count--;
		hal_enable_interrupt();
		return 1;
	}

	// if timeout is negative
	if (timeout < 0) {
		hal_enable_interrupt();
		return 0;
	}

	// Original semaphore: waiting until resource is available
	if (timeout == 0) {
		// execute until reesource is available
		while (1) {
			// push current task into waiting queue, and yield a CPU
			eos_tcb_t* cur_task = eos_get_current_task();
			cur_task->state = WAITING; // Not call eos_sleep() because alarm is not set

			if (sem->queue_type == FIFO)
				_os_add_node_tail(&(sem->wait_queue), (cur_task->node));
			else if (sem->queue_type == PRIORITY)
				_os_add_node_priority(&(sem->wait_queue), (cur_task->node));

			hal_enable_interrupt();
			eos_schedule();

			// after waking up, check if the resource is available
			hal_disable_interrupt();
			if (sem->count > 0) {
				sem->count--;
				hal_enable_interrupt();
				return 1;
			}
		}
	}

	// if timeout is positive, wait until timeout
	while (1) {
		hal_enable_interrupt();
		// call eos_sleep() for setting alarm
		eos_sleep(timeout);
		hal_disable_interrupt();
		if (sem->count > 0) {
			sem->count--;
			hal_enable_interrupt();
			return 1;
		}
	}
}


void eos_release_semaphore(eos_semaphore_t *sem)
{
    // To be filled by students: Project 4
    sem->count++;
	if (sem->wait_queue) {
        //PRINT("delete alarm and wake up\n")
		eos_tcb_t* wake_task = (eos_tcb_t*)(sem->wait_queue->ptr_data);
		eos_set_alarm(eos_get_system_timer(), wake_task->alarm, 0, NULL, NULL);
		_os_remove_node(&(sem->wait_queue), sem->wait_queue);
		_os_wakeup_sleeping_task((void*)wake_task);
	}
}


/**
 * Condition variables are not covery in the OS course
 */

void eos_init_condition(eos_condition_t *cond, int32u_t queue_type)
{
    cond->wait_queue = NULL;
    cond->queue_type = queue_type;
}


void eos_wait_condition(eos_condition_t *cond, eos_semaphore_t *mutex)
{
    /* Releases acquired semaphore */
    eos_release_semaphore(mutex);
    /* Waits on condition's wait_queue */
    _os_wait(&cond->wait_queue);
    /* Acquires semaphore before returns */
    eos_acquire_semaphore(mutex, 0);
}


void eos_notify_condition(eos_condition_t *cond)
{
    /* Selects a task in wait_queue and wake it up */
    _os_wakeup_single(&cond->wait_queue, cond->queue_type);
}

int8u_t eos_lock_scheduler() {
    int32u_t flag = hal_disable_interrupt();
    int8u_t temp = _os_scheduler_lock;
    _os_scheduler_lock = LOCKED;
    hal_restore_interrupt(flag);
    return temp;
}


void eos_restore_scheduler(int8u_t scheduler_state) {
    int32u_t flag = hal_disable_interrupt();
    _os_scheduler_lock = scheduler_state;
    hal_restore_interrupt(flag);
    eos_schedule();
}


int8u_t eos_get_scheduler_lock() {
    return _os_scheduler_lock;
}

