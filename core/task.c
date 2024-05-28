/********************************************************
 * Filename: core/task.c
 *
 * Author: Jiyong Park, RTOSLab. SNU
 * Modified by: Seongsoo Hong on 04/06/24
 *
 * Description: task creation and scheduling
 ********************************************************/

#include <core/eos.h>

#define READY		1
#define RUNNING		2
#define WAITING		3
#define SUSPENDED       4

/**
 * Runqueue of ready tasks
 */
static _os_node_t *_os_ready_queue[LOWEST_PRIORITY + 1];

/**
 * Pointer to TCB of the running task
 */
static eos_tcb_t *_os_current_task;


int32u_t eos_create_task(eos_tcb_t *task, addr_t sblock_start, size_t sblock_size, void (*entry)(void *arg), void *arg, int32u_t priority)
{
    PRINT("task: 0x%x, priority: %d\n", (int32u_t)task, priority);
    // To be filled by students: Projects 2 and 3
    addr_t sp = _os_create_context(sblock_start, sblock_size, entry, arg);
 
    task->sp = sp;
    task->period = 0;

    task->node = (_os_node_t*)malloc(sizeof(_os_node_t));
    task->node->priority = priority;
    task->node->ptr_data = (void*) task;
    
    task->alarm = (eos_alarm_t*)malloc(sizeof(eos_alarm_t));
    task->alarm->timeout = 0;
    task->alarm->handler = NULL;
    task->alarm->arg = NULL;
    task->alarm->alarm_queue_node.ptr_data = (void*) task->alarm;
    task->alarm->alarm_queue_node.priority = priority;

    task->state = READY;
    _os_add_node_tail(&_os_ready_queue[task->node->priority], task->node);
    _os_set_ready(task->node->priority);

    return 0;
}


int32u_t eos_destroy_task(eos_tcb_t *task) {
    // To be filled by students: not covered
}


void eos_schedule() {
    // To be filled by students: Projects 2 and 3

    if (_os_current_task != NULL){
        addr_t sp = _os_save_context();

        if (sp == NULL)
            return;

        _os_current_task->sp = sp;

        // store the current process in wait list if RUNNING (not WAITING)
        if(_os_current_task->state == RUNNING) { 
            _os_current_task->state = READY;
            _os_add_node_tail(&_os_ready_queue[_os_current_task->node->priority], _os_current_task->node);
            _os_set_ready(_os_current_task->node->priority);
        }
    }

    // Select a next task from waiting list
    _os_node_t* node;
    int32u_t highest_priority = _os_get_highest_priority();
    node = _os_ready_queue[highest_priority];
    _os_remove_node(&_os_ready_queue[highest_priority], node);
    if(_os_ready_queue[highest_priority] == NULL)
        _os_unset_ready(highest_priority);

    // Change the selected task to _os_current_task
    _os_current_task = (eos_tcb_t *)node->ptr_data;
    _os_current_task->state = RUNNING;
    _os_restore_context(_os_current_task->sp);
}


eos_tcb_t *eos_get_current_task() {
	return _os_current_task;
}


void eos_change_priority(eos_tcb_t *task, int32u_t priority) {
    // To be filled by students: not covered
}


int32u_t eos_get_priority(eos_tcb_t *task) {
    // To be filled by students: not covered
}


void eos_set_period(eos_tcb_t *task, int32u_t period){
    // To be filled by students: Project 3
    task->period = period;
}


int32u_t eos_get_period(eos_tcb_t *task) {
    // To be filled by students: not covered
}


int32u_t eos_suspend_task(eos_tcb_t *task) {
    // To be filled by students: not covered
}


int32u_t eos_resume_task(eos_tcb_t *task) {
    // To be filled by students: not covered
}


void eos_sleep(int32u_t tick) {
    // To be filled by students: Project 3
    eos_counter_t* system_timer = eos_get_system_timer();
    _os_current_task->state = WAITING;
    int32u_t wait_time = (tick == 0) ? _os_current_task->period : 0;
    eos_set_alarm(
        system_timer, 
        _os_current_task->alarm, 
        eos_get_system_timer()->tick + wait_time + tick, // wait time is 0 + tick if tick > 0
        _os_wakeup_sleeping_task, 
        _os_current_task
    );
    eos_schedule();
}


void _os_init_task() {
    PRINT("Initializing task module\n");

    /* Initializes current_task */
    _os_current_task = NULL;

    /* Initializes multi-level ready_queue */
    for (int32u_t i = 0; i < LOWEST_PRIORITY; i++) {
        _os_ready_queue[i] = NULL;
    }
}


void _os_wait(_os_node_t **wait_queue) {
    // To be filled by students: not covered
}


void _os_wakeup_single(_os_node_t **wait_queue, int32u_t queue_type) {
    // To be filled by students: not covered
}


void _os_wakeup_all(_os_node_t **wait_queue, int32u_t queue_type) {
    // To be filled by students: not covered
}


void _os_wakeup_sleeping_task(void *arg) {
    // To be filled by students: Project 3
    eos_tcb_t *task = (eos_tcb_t *) arg;

    // Add task to waiting queue
    task->state = READY;
    _os_add_node_tail(&_os_ready_queue[task->node->priority], task->node);
    _os_set_ready(task->node->priority);

    eos_schedule();
}