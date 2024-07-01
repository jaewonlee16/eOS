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
   
2. **Project 2: Multi-tasking**
   - Implement and understand multi-tasking within the eOS kernel.

3. **Project 3: Periodic Task and Priority Scheduler**
   - [Report](https://github.com/jaewonlee16/eOS/blob/master/eOS_project3_report.pdf)
   - Implement periodic tasks.
   - Develop a priority-based scheduler.

5. **Project 4: Synchronization and Communication Primitives**
   - [Report](https://github.com/jaewonlee16/eOS/blob/master/eOS_project4_report.pdf)
   - Implement synchronization mechanisms.
   - Develop communication primitives.
