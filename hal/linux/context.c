/********************************************************
 * Filename: hal/linux/context.c
 *
 * Author: Jiyong Park, RTOSLab. SNU
 * Modified by: Seongsoo Hong on 03/31/24
 *
 * Description: Context management (define, create, switch)
 ********************************************************/

#include <core/eos.h>
#include "emulator_asm.h"

typedef struct _os_context {
    // To be filled by students: Project 2
    /* low address */
    int32u_t edi;
    int32u_t esi;
    int32u_t ebp;
    int32u_t esp;
    int32u_t ebx;
    int32u_t edx;
    int32u_t ecx;
    int32u_t eax;
    int32u_t _eflags;
    int32u_t eip;
    /* high address */
} _os_context_t;


void print_context(addr_t context)
{
    if(context == NULL)
        return;

    _os_context_t *ctx = (_os_context_t *)context;

    // To be filled by students if needed: Project 2
 
    //PRINT("reg1  =0x%x\n", ctx->reg1);
    //PRINT("reg2  =0x%x\n", ctx->reg2);
    //PRINT("reg3  =0x%x\n", ctx->reg3);
    //...
}


addr_t _os_create_context(addr_t stack_base, size_t stack_size, void (*entry)(void *), void *arg)
{
    // To be filled by students: Project 2
    addr_t stack_top = (addr_t)((char*)stack_base + stack_size);

    _os_context_t *ctx = (_os_context_t *)(stack_top - 48);

    // Initialize all registers to 0 (NULL)
    ctx->edi = 0;
    ctx->esi = 0;
    ctx->ebp = 0;
    ctx->esp = 0;
    ctx->ebx = 0;
    ctx->edx = 0;
    ctx->ecx = 0;
    ctx->eax = 0;

    // Set the _eflags and eip
    ctx->_eflags = 1; 
    ctx->eip = (size_t)entry;  // Set the entry point address
    PRINT("Entry: 0x%x\n", ctx->eip);

    *((size_t *)(stack_top - 4)) = (size_t)arg;
    *((size_t *)(stack_top - 8)) = 0;

    return stack_top - 48;
}


addr_t _os_save_context()
{
    __asm__ __volatile__ (
                	"push %%ebp            \n\t"
                	"mov %%esp, %%ebp      \n\t"
                	"push $resume_point    \n\t"
                	"push (_eflags)	       \n\t"
                    	"mov $0, %%eax         \n\t"
                	"pusha		       \n\t"
                    	"mov %%esp, %%eax      \n\t"
                    	"push 4(%%ebp)         \n\t"
                    	"push (%%ebp)          \n\t"
                	"mov %%esp, %%ebp      \n\t"
                	"resume_point:	       \n\t"
                    	"leave                 \n\t"
                      //"ret                   \n\t"
		        :
                	:
            );

    return;
}


void _os_restore_context(addr_t sp)
{
    // To be filled by students: Project 2
    __asm__ __volatile__ (
                    	"mov %0, %%esp   \n\t"
                	"popa		 \n\t"
                	"pop _eflags	 \n\t"
                    	"ret             \n\t"
			:
                	: "r"(sp)
            );

    return;
}
