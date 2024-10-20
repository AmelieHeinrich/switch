//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-20 19:13:54
//

#include <switch.h>
#include <stdio.h>

alignas(16) u8 __nx_exception_stack[0x1000];
u64 __nx_exception_stack_size = sizeof(__nx_exception_stack);

void __libnx_exception_handler(ThreadExceptionDump *ctx)
{
    int i;
    FILE *f = fopen("exception_dump", "w");
    if(f==NULL)return;

    fprintf(f, "error_desc: 0x%x\n", ctx->error_desc);//You can also parse this with ThreadExceptionDesc.
    //This assumes AArch64, however you can also use threadExceptionIsAArch64().
    for(i=0; i<29; i++)fprintf(f, "[X%d]: 0x%lx\n", i, ctx->cpu_gprs[i].x);
    fprintf(f, "fp: 0x%lx\n", ctx->fp.x);
    fprintf(f, "lr: 0x%lx\n", ctx->lr.x);
    fprintf(f, "sp: 0x%lx\n", ctx->sp.x);
    fprintf(f, "pc: 0x%lx\n", ctx->pc.x);

    //You could print fpu_gprs if you want.

    fprintf(f, "pstate: 0x%x\n", ctx->pstate);
    fprintf(f, "afsr0: 0x%x\n", ctx->afsr0);
    fprintf(f, "afsr1: 0x%x\n", ctx->afsr1);
    fprintf(f, "esr: 0x%x\n", ctx->esr);

    fprintf(f, "far: 0x%lx\n", ctx->far.x);

    fclose(f);
}
