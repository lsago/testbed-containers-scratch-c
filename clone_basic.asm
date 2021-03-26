; Trying to issue a clone syscall in x86_64 assembly.
; Goal was C.. but we're just experimenting here..
;
; vim: - auto/tab completion was annoying me, for now just wipe out completeopt
;        with  :set completeopt=
;      - just in case, number of chars selected in visual is displayed at
;        bottom right (:set showcmd)
;
; A few resources used:
;   https://www.cs.fsu.edu/~langley/CNT5605/2017-Summer/assembly-example/assembly.html
;   file:///usr/include/asm/unistd_64.h
;   https://cs.lmu.edu/~ray/notes/nasmtutorial/
;   https://www.nasm.us/xdoc/2.13.01/html/nasmdoc3.html#section-3.4.4
;

; From https://github.com/torvalds/linux/blob/master/arch/x86/entry/entry_64.S {{
;  * Registers on entry:
;  * rax  system call number
;  * rcx  return address
;  * r11  saved rflags (note: r11 is callee-clobbered register in C ABI)
;  * rdi  arg0
;  * rsi  arg1
;  * rdx  arg2
;  * r10  arg3 (needs to be moved to rcx to conform to C ABI)
;  * r8   arg4
;  * r9   arg5
; }} -- So, we should put our arguments for syscalls at rdi, rsi, rdx... etc


global _start


section .bss
child_stack:    resb    1024    ; Stack for the child (no idea if this
                                ; is the right place for stack.. good enough for now)


section .text


child_function:
        mov     rax, 1          ; 1 is write() [asm/unistd_64.h]
        mov     rdi, 1          ; int fd = 1 (stdout)
        mov     rsi, child_str  ; mem addr of our string
        mov     rdx, 53         ; for now, we just count them,
        syscall                 ; execute the 'syscall' instruction

        ; exit(1)
        mov     rdi, 1          ; first argument to sycall is expected in %rdi
        mov     rax, 60         ; 60 is exit()
        syscall


_start:

        ; issue a write() syscall
        ; standardized by a POSIX specification:
        ;     ssize_t write(int fd, const void *buf, size_t nbytes);
        ; (See: https://en.wikipedia.org/wiki/Write_(system_call) )

        mov     rax, 1          ; 1 is write() [asm/unistd_64.h]
        mov     rdi, 1          ; int fd = 1 (stdout)
        mov     rsi, greeting   ; mem addr of our string
        mov     rdx, 17         ; for now, we just count them..
        syscall                 ; execute the 'syscall' instruction

        ; from man clone(2) (See NOTES section for raw syscall for x86_64):
        ; long clone(unsigned long flags, void *stack,
        ;             int *parent_tid, int *child_tid,
        ;             unsigned long tls);

        mov     rax, 56         ; 56 is clone() [asm/unistd_64.h]
        mov     rdi, 0          ; flags, we'll worry about the flags later
        mov     rsi, child_stack; *stack
        mov     rdx, 0          ; See CLONE_PIDFD ;  TODO
        mov     r10, 0          ; See CLONE_CHILD_SETTID ;  TODO
        mov     r8, 0           ; See CLONE_SETTLS ;  TODO
        syscall
        ; This behaves like a fork(). Clones the process and keeps executing
        ; instructions where the parent left.
        ; glibc's clone:
        ;   int clone(int (*fn)(void *), void *stack, int flags, void *arg, ...
        ; Its first argument is a function pointer.
        ; skimming over glibc/sysdeps/unix/sysv/linux/x86_64/clone.S it seems like
        ; that whole "just start executing here" is a userland thing.

        ; here's the "essence" of what clone.S (glibc) I think, it does:
        test    rax, rax
        jz      child_function
        ; I'm guessing that the kernel wipes out rax for the new process
        ; So we can leverage that fact to distinguish between the two
        ; (child and parent)..  if rax has zero, 'zero' flag (ZF) will be set
        ; to 1, and jz will jump (no?)
        ; TODO: this right?

        mov     rax, 1          ; 1 is write() [asm/unistd_64.h]
        mov     rdi, 1          ; int fd = 1 (stdout)
        mov     rsi, post_clone ; mem addr of our string
        mov     rdx, 50         ; for now, we just count them..
        syscall                 ; execute the 'syscall' instruction

        ; exit(0)
        mov     rdi, 0          ; first argument to sycall is expected in %rdi
        mov     rax, 60         ; 60 is exit()
        syscall



section .data
greeting: db "clone() from asm", 10     ; 10 is \n
child_str: db "This string should be printed out from child process", 10
post_clone: db "Printing this after having issued clone() syscall", 10


