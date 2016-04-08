# Inline comment
/* Block comment */

# void swap(long * x, long * y);
.globl swap
swap:
	pushq %rbx           # callee-save
	movq (%rdi), %rax    # %rax = *x 
	movq (%rsi), %rbx    # %rbx = *y
	movq  %rbx, (%rdi)   # *x = %rbx
	movq  %rax, (%rsi)   # *y = %rax
	popq  %rbx           # callee-restore
	ret                  # return

/* thread_switch_asm:
	switch to a new thread, which is already running */
# void thread_switch_asm(thread* old, thread* new)
.globl thread_switch_asm
thread_switch_asm:	
 	pushq %rbx          # callee-save regs
	pushq %rbp         
	pushq %r12 	   
	pushq %r13         
	pushq %r14         
	pushq %r15
	
	movq %rsp, (%rdi)   # save current stack pointer in old
	movq (%rsi), %rsp   # load new's stack pointer
	
	popq %r15           # callee-restore regs	
	popq %r14           	
	popq %r13          
	popq %r12 	   
	popq %rbp          
	popq %rbx          
	ret         

/* thread_start_asm:
	start a new thread and switch to it */
# void thread_start_asm(thread* old, thread* new)
.globl thread_start_asm
thread_start_asm:
	pushq %rbx          # callee-save regs
	pushq %rbp         
	pushq %r12 	   
	pushq %r13         
	pushq %r14         
	pushq %r15

	movq %rsp, (%rdi)   # save current stack pointer in old
	movq (%rsi), %rsp   # load new's stack pointer

	jmp thread_wrap     # calls the starting function of new
	
	
