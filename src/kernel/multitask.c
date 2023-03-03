
#include <stdint.h>
#include <stdio.h>
#include <kernel/multitask.h>

/* dump_task_list: print the list of tasks starting with the current one */
void dump_task_list(void) {
    puts("Dumping task list:");

    /* 0 will be the current one, not the first task we created */
    int i = 0;

    Ctx* const first_ctx = mt_current_task;

    /* We at least want to print one */
    printf("[%d] %s | prev: %p | next: %p | stack: %ld | esp: %ld | cr3: %ld | "
           "state: %ld\n",
           i++, first_ctx->name, first_ctx->prev, first_ctx->next,
           first_ctx->stack, first_ctx->esp, first_ctx->cr3, first_ctx->state);

    for (Ctx* cur_ctx = mt_current_task; cur_ctx->next != first_ctx;
         cur_ctx      = cur_ctx->next, i++) {
        printf("[%d] %s | prev: %p | next: %p | stack: %ld | esp: %ld | cr3: "
               "%ld | "
               "state: %ld\n",
               i, cur_ctx->name, cur_ctx->prev, cur_ctx->next, cur_ctx->stack,
               cur_ctx->esp, cur_ctx->cr3, cur_ctx->state);
    }
}

