#include <threading.h>


void t_init()
{
        // TODO
            // Initialize all contexts as INVALID and prepare their stacks
        for (int i = 0; i < NUM_CTX; i++) {
                contexts[i].state = INVALID;
                contexts[i].context.uc_stack.ss_sp = malloc(STK_SZ);
                if (contexts[i].context.uc_stack.ss_sp == NULL) {
                        continue;
                }
                contexts[i].context.uc_stack.ss_size = STK_SZ;
                contexts[i].context.uc_stack.ss_flags = 0;
        }


        current_context_idx = 0; // Start with the first context
}

int32_t t_create(fptr foo, int32_t arg1, int32_t arg2)
{
        // TODO
            // Find an available slot for a new context
        for (int i = 0; i < NUM_CTX; i++) {
                if (contexts[i].state == INVALID) {
                        // Initialize context with the current one
                        getcontext(&(contexts[i].context));

                        // Set the context to execute `foo` with `arg1` and `arg2`
                        contexts[i].context.uc_link = NULL; // No next context to jump to
                        makecontext(&(contexts[i].context), (ctx_ptr)foo, 2, arg1, arg2);

                        // Mark the context as valid
                        contexts[i].state = VALID;
                        return 0; // Success
                }
        }
        return 1; // No available context slots
}

int32_t t_yield()
{
        // TODO
            // Find the next valid context to switch to
        int valid_count = 0;
        for (int i = 1; i <= NUM_CTX; i++) {
                int next_idx = (current_context_idx + i) % NUM_CTX;
                if (contexts[next_idx].state == VALID) {
                        valid_count++;
                        uint8_t previous_context_idx = current_context_idx;
                        current_context_idx = next_idx;

                        // Swap the current context with the next one
                        swapcontext(&contexts[previous_context_idx].context, &contexts[current_context_idx].context);
                        break;
                }
        }
        return valid_count; // Return the number of valid contexts
}

void t_finish()
{
        // TODO
        // Mark the current context as DONE
        contexts[current_context_idx].state = DONE;

        // Yield to the next available context
        t_yield();
}
