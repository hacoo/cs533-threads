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


# void thread_switch_asm(thread* old, thread* new)
.globl thread_switch_asm
thread_switch_asm:	
	pushq %rbx          # callee-save reg
	pushq %rbp         
	pushq %r12 	   
	pushq %r13         
	pushq %r14         
	pushq %r15
	
	movq %rsp, (%rdi)   # save current stack pointer in old
	movq (%rsi), %rsp   # load new's stack pointer
	
	popq %rbx           # pop callee-save regs
	popq %rbp          
	popq %r12 	   
	popq %r13          
	popq %r14          
	popq %r15         
	ret         

		
	
	
