.text


# syscall jump table used by handler to execute correct syscall indicated by eax
.globl systemcall_jump_table
syscall_jump_table:
  .long system_halt
  .long system_execute
  .long system_read
  .long system_write
  .long system_open
  .long system_close 
  .long system_getargs
  .long system_vidmap 
  .long system_set_handler
  .long system_sigreturn


/*
systemcall_handler
description: general systemcall handler linkage
    input: systemcall number through EAX
    output: -
    effect: execute syscall passed by eax
*/

.globl systemcall_handler
systemcall_handler:
    # valid syscall number check
    cmpl $0, %eax
    jle syscall_num_out_of_bound
    cmpl $10, %eax
    jg syscall_num_out_of_bound

    pushl %ebp
    pushl %ebx
    pushl %esi
    pushl %edi
    pushfl

    decl %eax
    pushl %edx
    pushl %ecx
    pushl %ebx
    call *syscall_jump_table(, %eax, 4) # jump to actual hander indicated by eax
    addl $12, %esp

    popfl
    popl %edi
    popl %esi
    popl %ebx
    popl %ebp

    iret # back to user progarm

.globl syscall_num_out_of_bound
syscall_num_out_of_bound:
    movl $-1, %eax
    iret



  
  
  
    
