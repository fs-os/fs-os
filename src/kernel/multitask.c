
#include <stdint.h>
#include <stdio.h>
#include <kernel/multitask.h>

void dump_task_list(void) {
    puts("Dumping task list:");

    /* 0 will be the current one, not the first task we created */
    int i = 0;

    Ctx* first_ctx = mt_current_task;

    /* We at least want to print one */
    printf("[%d] %s | ctx: %p | prev: %p | next: %p | stack: 0x%lX | esp: "
           "0x%lX | cr3: 0x%lX | fxdata: 0x%lX\n",
           i++, first_ctx->name, first_ctx, first_ctx->prev, first_ctx->next,
           first_ctx->stack, first_ctx->esp, first_ctx->cr3, first_ctx->fxdata);

    for (Ctx* cur_ctx = first_ctx->next; cur_ctx != first_ctx;
         cur_ctx      = cur_ctx->next, i++) {
        printf("[%d] %s | ctx: %p | prev: %p | next: %p | stack: 0x%lX | esp: "
               "0x%lX | cr3: 0x%lX | fxdata: 0x%lX\n",
               i, cur_ctx->name, cur_ctx, cur_ctx->prev, cur_ctx->next,
               cur_ctx->stack, cur_ctx->esp, cur_ctx->cr3, cur_ctx->fxdata);
    }
}
