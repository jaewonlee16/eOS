# Operating System 2024 Spring
Project in SNU ECE Introduction to OS (430.318) lecture, at 2024 spring.

## eOS Project Introduction

### Overview
**eOS (Educational Operating System)** is a real-time, preemptive, multitasking kernel developed primarily for educational purposes in the field of Real-Time Operating Systems (RTOS). 
The project is designed to be highly portable, with most of its kernel code written in ANSI C, allowing it to run on various hardware platforms with minimal changes. 

### What is eOS?
eOS is designed to run as a process on Linux, providing an execution environment where both user code and kernel code are linked to form a single executable file. 
The kernel code is provided in the form of a library, facilitating the development of RTOS applications.

### Architecture
The architecture of eOS is layered, with a hardware abstraction layer (HAL) that allows it to run on different hardware platforms. The main components include:
- **Kernel Core Modules**: Core functionalities such as task management, scheduling, synchronization, and communication.
- **Hardware Abstraction Layer (HAL)**: Abstracts hardware-specific details, enabling eOS to operate on various hardware platforms.
- **User Applications**: Application code that runs on top of the eOS kernel.

### Development Environment
To set up the development environment for eOS, follow these steps:
1. **Install Ubuntu Linux on a Virtual Machine**:
   - Use VMware Player or Virtual Box to create a new virtual machine.
   - Download the latest Ubuntu ISO file (64-bit) from [here](https://ubuntu.com/download/desktop).
   - Install Ubuntu using the downloaded ISO file.
   
2. **Set Up the eOS Source Tree**:
   - Use shared folders, network, or a flash drive to transfer the `eos.tar.gz` file to the virtual machine.
   - Extract the eOS source tree using the command: `tar –xvf eos.tar.gz`.

3. **Build and Execution Environment**:
   - Install necessary libraries for a 32-bit build on Ubuntu:
     ```sh
     sudo apt install gcc-multilib g++-multilib
     ```
   - Verify that the installed Linux supports IA32 execution:
     ```sh
     grep CONFIG_IA32_EMULATION /boot/config-$(uname -r)
     ```
   - Build the eOS kernel:
     ```sh
     cd eos
     make clean
     make all
     ```

### Projects Schedule
The eOS project is divided into several subprojects, each focusing on different aspects of the operating system:
1. **Project 1: Code Analysis**
   - Analyze the eOS code.
   - Study the i386 architecture and calling conventions.
   - For details see the [section below](#eos-project-1-code-analysis)
  
2. **Project 2: Multi-tasking**
   - Implement and understand multi-tasking within the eOS kernel.
   - For details see the [section below](#eos-project-2-context-switching-implementation)

3. **Project 3: Periodic Task and Priority Scheduler**
   - [Report](https://github.com/jaewonlee16/eOS/blob/master/eOS_project3_report.pdf)
   - Implement periodic tasks.
   - Develop a priority-based scheduler.

5. **Project 4: Synchronization and Communication Primitives**
   - [Report](https://github.com/jaewonlee16/eOS/blob/master/eOS_project4_report.pdf)
   - Implement synchronization mechanisms.
   - Develop communication primitives.


# eOS Project 1 Code Analysis
**Author**: 2019-12172 Jaewon Lee

## 1. eOS Source Code Analysis

### 1.1 System Initialization Routine

#### System Initialization Code Execution Sequence
1. **Directory: `hal/linux/emulator/main.c`**
   - **Function / Code:**
     ```c
     jmp *%_vector[0]
     ```

2. **Directory: `hal/linux/entry.S`**
   - **Vector Definition:**
     ```assembly
     _vector:
       .long _os_reset_handler
       .long _os_reset_handler
       .long _os_reset_handler
       .long _os_irq_handler
     ```

3. **Directory: `hal/linux/entry.S`**
   - **Reset Handler:**
     ```assembly
     _os_reset_handler:
       _CLI
       lea _os_init_stack_end, %esp
       call _os_init
     ```

4. **Directory: `hal/linux/emulator_asm.h`**
   - **Macro Definition:**
     ```c
     #define _CLI \
       movl $0, _eflags;
     ```

5. **Directory: `core/init.c`**
   - **Function:**
     ```c
     void _os_init() {
         // Some code
     }
     ```

6. **Directory: `hal/linux/interrupt_asm.S`**
   - **Disable Interrupt:**
     ```assembly
     hal_disable_interrupt:
       mov _eflags, %eax
       _CLI
       ret
     ```

7. **Directory: `hal/linux/init_hal.c`**
   - **Function:**
     ```c
     void _os_init_hal() {
         PRINT("Initializing hal module\n");
         _init_timer_interrupt();
         hal_enable_irq_line(IRQ_INTERVAL_TIMER0);
     }
     ```

8. **Directory: `hal/linux/emulator/timer_emulator.c`**
   - **Function:**
     ```c
     void _init_timer_interrupt() {
         // Some code
     }
     ```

9. **Directory: `hal/linux/interrupt.c`**
   - **Function:**
     ```c
     void hal_enable_irq_line(int32u_t irq) {
         _irq_mask &= ~(0x1 << irq);
     }
     ```

10. **Directory: `core/interrupt.c`**
    - **Function:**
      ```c
      void _os_init_icb_table() {
          PRINT("Initializing interrupt module\n");
          for (int8s_t i = 0; i < IRQ_MAX; i++) {
              _os_icb_t *p = &_os_icb_table[i];
              p->irqnum = i;
              p->handler = NULL;
          }
      }
      ```

11. **Directory: `core/scheduler.c`**
    - **Function:**
      ```c
      void _os_init_scheduler() {
          PRINT("Initializing scheduler module\n");
          _os_ready_group = 0;
          for (int8u_t i = 0; i < READY_TABLE_SIZE; i++) {
              _os_ready_table[i] = 0;
          }
          _os_scheduler_lock = UNLOCKED;
      }
      ```

12. **Directory: `core/task.c`**
    - **Function:**
      ```c
      void _os_init_task() {
          PRINT("Initializing task module\n");
          _os_current_task = NULL;
          for (int32u_t i = 0; i < LOWEST_PRIORITY; i++) {
              _os_ready_queue[i] = NULL;
          }
      }
      ```

13. **Directory: `core/timer.c`**
    - **Function:**
      ```c
      void _os_init_timer() {
          eos_init_counter(&system_timer, 0);
          eos_set_interrupt_handler(IRQ_INTERVAL_TIMER0, timer_interrupt_handler, NULL);
      }
      ```

### Analysis
1. Jump to `vector[0]` which is `_os_reset_handler` defined in `entry.S`.
2. `_os_reset_handler` disables interrupts, initializes the stack pointer, and calls `_os_init()`.
3. Functions called by `_os_init()`:
   - Disable all interrupts.
   - Initialize HAL module, including timer interrupt emulation.
   - Enable interrupt requests.
   - Initialize the interrupt control block table.
   - Initialize scheduler and task-related structures.
   - Register the timer interrupt handler.

### 1.2 Interrupt Handling Routine

#### Interrupt Management Module API
- **Directory: `hal/linux/interrupt.c`**
  - **Functions:**
    ```c
    void hal_ack_irq() {
        _irq_pending &= ~(0x1 << irq);
    }

    int hal_get_irq() {
        for (int i = 0; i < 32; i++) {
            if (_irq_pending & ~_irq_mask & (0x1 << i)) {
                return i;
            }
        }
        return -1;
    }

    void hal_disable_irq_line(int32u_t irq) {
        _irq_mask |= (0x1 << irq);
    }

    void hal_enable_irq_line(int32u_t irq) {
        _irq_mask &= ~(0x1 << irq);
    }
    ```

- **Directory: `hal/linux/interrupt_asm.S`**
  - **Functions:**
    ```assembly
    hal_enable_interrupt:
      _STI
      ret

    hal_disable_interrupt:
      mov _eflags, %eax
      _CLI
      ret

    hal_restore_interrupt:
      mov 0x4(%esp), %eax
      mov %eax, _eflags
      ret
    ```

#### Explanation
- **`hal_ack_irq()`**: Clears the pending bit for the given IRQ.
- **`hal_get_irq()`**: Returns the highest priority pending IRQ that is not masked, or -1 if none.
- **`hal_disable_irq_line()`**: Masks the given IRQ line.
- **`hal_enable_irq_line()`**: Unmasks the given IRQ line.
- **`hal_enable_interrupt()`**: Enables interrupts by setting the interrupt flag.
- **`hal_disable_interrupt()`**: Disables interrupts and returns the previous interrupt flag state.
- **`hal_restore_interrupt()`**: Restores the interrupt flag from the stack, ensuring proper interrupt handling.

This analysis provides an in-depth look at the initialization and interrupt handling routines within the eOS kernel, highlighting key functions and their roles in the system.



# eOS Project 2: Context Switching Implementation

## Overview
This project involves implementing key structures and functions required for context switching in the eOS kernel. The goal is to understand and create the essential components for task management and context switching in a real-time operating system (RTOS).

## Implemented Structures and Functions

### 1. `hal/linux/context.c`
- **Structure:** `_os_context_t`
- **Functions:**
  - `_os_create_context()`
  - `_os_save_context()`
  - `_os_restore_context()`

### 2. `core/eos.h`
- **Structure:** `eos_tcb_t`

### 3. `core/task.c`
- **Functions:**
  - `eos_create_task()`
  - `eos_schedule()`

## Detailed Description

### `_os_context_t` Structure
The `_os_context_t` structure defines the information that needs to be saved during a context switch. This includes various CPU registers.


### `_os_create_context()` Function
- **Prototype:** `addr_t _os_create_context(addr_t stack_base, size_t stack_size, void (*entry)(void *arg), void *arg);`
- **Defined in:** `core/eos_internal.h`
- **Implemented in:** `hal/linux/context.c`
- **Description:** Creates a new context on the given stack.
- **Input:**
  - `stack_base`: Start address of the stack (low address)
  - `stack_size`: Size of the stack (in bytes)
  - `entry`: Address of the function to be executed when the context is restored
  - `arg`: Argument to be passed to the entry function
- **Output:** Start address of the created context.

#### Detailed Steps:
- Create a fake stack for the new task without using inline assembly.
- Save the argument (`arg`) for the `entry` function at the bottom of the stack.
- Save a fake return address (NULL) for the `entry` function since it should not return in eOS.
- Save the initial register values (context) for the `entry` function on the stack:
  - Only `_eflags` and `%eip` have meaningful values.
  - `_eflags` is set to 1.
  - `%eip` is set to the address of the `entry` function.
  - Other registers can be set to NULL.
- Return the address where the context is saved and exit the function.

### `_os_save_context()` Function
- **Prototype:** `addr_t _os_save_context(void)`
- **Defined in:** `core/eos_internal.h`
- **Implemented in:** `hal/linux/context.c`
- **Description:** Saves the current context to the stack and returns the stack pointer.
- **Input:** None
- **Output:** Pointer to the saved context on the stack, or `NULL`/`0` when restoring context.

#### Detailed Steps:
- Use inline assembly to save individual registers.
- Save the necessary registers (context) to the stack:
  - `%eip` is the address to resume execution upon context restoration.
  - `%eax` should store `0` for the return value upon context restoration.
- Save the stack pointer to `%eax`.
- To protect the saved context, push the `%eip` and `%ebp` at the bottom of the stack frame to the top and modify `%ebp`.
- Use `leave` and `ret` instructions to return.


### `_os_restore_context()` Function
- **Prototype:** `void _os_restore_context(addr_t sp)`
- **Defined in:** `core/eos_internal.h`
- **Implemented in:** `hal/linux/context.c`
- **Description:** Restores the context from the saved stack content.
- **Input:** `sp`: Top address of the stack where the context is saved.
- **Output:** None

#### Detailed Steps:
- Use inline assembly to restore the saved registers.
- Change the stack pointer to the location where the context is saved.
- Pop the values of the saved registers sequentially.
- Jump to the `resume_point` in `_os_save_context()` (effectively a return).
- Return from `_os_save_context()` to `eos_schedule()`.
  - Before returning, restore `%ebp` from the stack using the `leave` instruction.

### `eos_create_task()` Function
- **Prototype:** `int32u_t eos_create_task(eos_tcb_t *task, addr_t sblock_start, size_t sblock_size, void (*entry)(void *arg), void *arg, int32u_t priority)`
- **Defined in:** `core/task.c`
- **Description:** Creates and initializes a task.
- **Input:**
  - `task`: TCB structure holding the task information.
  - `sblock_start`: Start address of the stack allocated to the task.
  - `sblock_size`: Size of the stack.
  - `entry`: Address of the function to be executed as the task.
  - `arg`: Argument to be passed to the `entry` function.
  - `priority`: Priority of the task.
- **Output:** `0`

### `eos_schedule()` Function
- **Description:** Switches context from the current task to a new task.
- **Detailed Steps:**
  - Call `_os_save_context()` to save the current task's context and handle the return value as follows:
    - If the return value is not NULL (context saved successfully):
      - Record the return value (stack pointer) in the TCB.
      - Select the highest priority task from the ready queue.
      - Call `_os_restore_context()` to restore the context of the selected task.
    - If the return value is NULL (context just restored):
      - Simply return from the function.
  - During the first task execution after boot, there is no currently executing task. Thus, only the first task's context needs to be restored without saving any context.

## Usage
change the *eos/work.c* to this code and build the eOS
```c
#include <core/eos.h>
#define STACK_SIZE 8096
static int8u_t stack0[STACK_SIZE]; // stack for task0
static int8u_t stack1[STACK_SIZE]; // stack for task1
static eos_tcb_t tcb0; // tcb for task0
static eos_tcb_t tcb1; // tcb for task1
void print_numbers(void *arg) { // Function for task0: Print numbers from 1 to 20 repeatedly
 int i = 1;
 while(1) {
 printf("%d", i)
eos_schedule(); // Yield CPU to task1
if (i++ == 20) { i = 1; }
 }
}
void print_alphabet(void *arg) { // Function for task1: Print alphabet from ‘a’ to ‘z’ repeatedly
 int i = 97;
 while(1) {
 printf("%c", i);
eos_schedule(); // Yield CPU to task0
if (i++ == 122) { i = 97; }
 }
}
void eos_user_main() {
 // This is the test code for Project 2
 // Note that you must use a priority of 0 only since the priority-based scheduling hasn’t been implemented yet
 eos_create_task(&tcb0, (addr_t) stack0, STACK_SIZE, print_numbers, NULL, 0);
eos_create_task(&tcb1, (addr_t) stack1, STACK_SIZE, print_alphabet, NULL, 0);
}
```
