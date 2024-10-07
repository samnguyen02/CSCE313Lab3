#include <threading.h>


void t_init()
{
        for (int32_t i = 0; i < NUM_CTX; i++){
                contexts[i].state = INVALID;
                contexts[i].context.uc_stack.ss_sp = allocate_memory(STK_SZ);
                contexts[i].context.uc_stack.ss_size = STK_SZ;
                contexts[i].context.uc_stack.ss_flags = 0;
                getcontext(&(contexts[i].context));
        }
        current_context_idx = 0;
}

int32_t t_create(fptr foo, int32_t arg1, int32_t arg2)
{
        for (uint8_t i = 0; i < NUM_CTX; ++i) {
                if (contexts[i].state == INVALID) {
                        getcontext(&contexts[i].context);

                        contexts[i].context.uc_stack.ss_sp = malloc(STK_SZ);
                        if (contexts[i].context.uc_stack.ss_sp == NULL) {
                                return -1; 
                        }
                        contexts[i].context.uc_stack.ss_size = STK_SZ;
                        contexts[i].context.uc_stack.ss_flags = 0;
                        contexts[i].context.uc_link = NULL;  
                        makecontext(&contexts[i].context, (void (*)(void))foo, 2, arg1, arg2);
                        contexts[i].state = VALID;  
                        return i;                   
                }
        }
        return -1;
}

int32_t t_yield()
{
        for (uint8_t i = 0; i < NUM_CTX; ++i){
                if (contexts[i].state == VALID && i != current_context_idx){
                        uint8_t idx = current_context_idx;
                        current_context_idx = i;

                        swapcontext(&contexts[idx].context, &contexts[i].context);
                        break;
                }
        }
        int count = 0;
        for (uint8_t j = 1; j < NUM_CTX; ++j){
                if (contexts[j].state == VALID){
                        ++count;
                }
        }

        return count;
}

void t_finish()
{
        int idx = current_context_idx;

        if (contexts[idx].state == VALID){
                if (contexts[idx].context.uc_stack.ss_sp != NULL){
                        free(contexts[idx].context.uc_stack.ss_sp);
                        contexts[idx].context.uc_stack.ss_sp = NULL; 
                }
        }
        
        contexts[idx].state = DONE;  

        t_yield();  
}
