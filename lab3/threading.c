#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>
#include <string.h>
#include "threading.h"

/*t init(): You can use this function to initialize various 
data structures that your library uses such as contexts and current context idx.*/
void t_init() 
{
    current_context_idx = 0;
    volatile uint8_t i;
    for (i = 0; i<NUM_CTX; i++)
    {
        if (i == 0)
        {
            contexts[i].state = VALID;
        }
        else
        {
            contexts[i].state = INVALID;
        }
    }
    getcontext(&contexts[0].context);
}

int32_t t_create(fptr foo, int32_t arg1, int32_t arg2) 
{
    volatile uint8_t ctx_num;
    for (ctx_num = 0; ctx_num < NUM_CTX; ctx_num++)
    {
        if (contexts[ctx_num].state != VALID)
        {
            getcontext(&contexts[ctx_num].context);
            contexts[ctx_num].context.uc_stack.ss_sp = (char*)malloc(STK_SZ);
            contexts[ctx_num].context.uc_stack.ss_flags = 0;
            contexts[ctx_num].context.uc_link = NULL;
            current_context_idx = ctx_num;
            makecontext(&contexts[ctx_num].context, (ctx_ptr)foo, 2, arg1, arg2);
            contexts[ctx_num].state = VALID;
            return 0;

        }
    }
    return -1;
}


int32_t t_yield() 
{
    volatile uint8_t nextCtx;
    for (nextCtx = 0; nextCtx < NUM_CTX; nextCtx++)
    {
        if((nextCtx != current_context_idx) && (contexts[nextCtx].state == VALID))
        {
            swapcontext(&contexts[current_context_idx].context, &contexts[nextCtx].context);
            current_context_idx = nextCtx;
            break;
        }

    }
    int countValidCtx = 0;
    for (int i = 0; i<NUM_CTX; i++)
    {
        if(contexts[i].state == VALID)
        {
            countValidCtx++;
        }
    }
    return countValidCtx;
}

// Mark the current context as DONE and yield control to another valid context
void t_finish() 
{
    contexts[current_context_idx].state = DONE;
    t_yield();
}
