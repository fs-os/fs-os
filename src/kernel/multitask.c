
#include <stdint.h>
#include <stdio.h>
#include <kernel/multitask.h>

void mt_dump_tasks(void) {
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

void mt_print_fpu_data(fpu_data_t* p) {
    printf("*(fpu_data_t*)%p = {\n"
           "  .fcw        = 0x%X\n"
           "  .fsw        = 0x%X\n"
           "  .ftw        = 0x%X\n"
           "  .fop        = 0x%X\n"
           "  .fip        = 0x%lX\n"
           "  .fcs        = 0x%X\n"
           "  .fdp        = 0x%lX\n"
           "  .fds        = 0x%X\n"
           "  .mxcsr      = 0x%lX\n"
           "  .mxcsr_mask = 0x%lX\n"
           "  .registers  = (void*)%p\n"
           "}\n",
           p, p->fcw, p->fsw, p->ftw, p->fop, p->fip, p->fcs, p->fdp, p->fds,
           p->mxcsr, p->mxcsr_mask, &p->registers);
}
