/********************************************************
 * Filename: core/timer.c
 *
 * Author: Jiyong Park, RTOSLab. SNU
 * Modified by: Seongsoo Hong on 03/30/24
 *
 * Description: User interface for handling interrupts
 ********************************************************/

#include <core/eos.h>

static eos_counter_t system_timer;


int8u_t eos_init_counter(eos_counter_t *counter, int32u_t init_value)
{
    counter->tick = init_value;
    counter->alarm_queue = NULL;

    return 0;
}


void eos_set_alarm(eos_counter_t* counter, eos_alarm_t* alarm, int32u_t timeout, void (*entry)(void *arg), void *arg)
{
    // To be filled by students: Project 3
    _os_remove_node(&counter->alarm_queue, &alarm->alarm_queue_node);

    if(timeout == 0 || entry == NULL) {
        return;
    }

    alarm->timeout = timeout;
    alarm->handler = entry;
    alarm->arg = arg;

    // Enqueue by the increasing order of 64*timeout + priority
    // This makes the queue sorted by timeout.
    // If the timeout is same, it is then sorted by priority.
    alarm->alarm_queue_node.priority 
        = (int32u_t)((LOWEST_PRIORITY + 1) * timeout + ((eos_tcb_t*)arg)->node->priority);
    //PRINT("alarm added priority: %u, timeout: %d, priority: %d\n", alarm->alarm_queue_node.priority, timeout,((eos_tcb_t*)arg)->node->priority )

    _os_add_node_priority(&counter->alarm_queue, &alarm->alarm_queue_node);
}


eos_counter_t* eos_get_system_timer()
{
    return &system_timer;
}


void eos_trigger_counter(eos_counter_t* counter)
{
    // PRINT("tick %d\n", counter->tick + 1);
    PRINT("tick\n");
    // To be filled by students: Project 3
    counter->tick++;
    

    // There could be more than 1 task at the queue, 
    // so interation is needed
    while(counter->alarm_queue != NULL) {
        _os_node_t * alarm_node = counter->alarm_queue;
        
        eos_alarm_t* alarm = (eos_alarm_t*)(alarm_node->ptr_data);
        if(alarm->timeout > counter->tick)
            break;

        // Remove alarm from queue
        eos_counter_t* system_timer = eos_get_system_timer();
        eos_set_alarm(
            system_timer, 
            alarm, 
            0, // This means remove
            NULL, 
            NULL
        );

        // call handler
        alarm->handler(alarm->arg);
    }
    eos_schedule();

}

/* Timer interrupt handler */
static void timer_interrupt_handler(int8s_t irqnum, void *arg)
{
    /* Triggers alarms */
    eos_trigger_counter(&system_timer);
}


void _os_init_timer()
{
    eos_init_counter(&system_timer, 0);

    /* Registers timer interrupt handler */
    eos_set_interrupt_handler(IRQ_INTERVAL_TIMER0,
		    timer_interrupt_handler, NULL);
}
