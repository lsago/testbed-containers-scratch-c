/*
 * Just a reference example of clone()
 * Based on/from:
 * https://eli.thegreenplace.net/2018/launching-linux-threads-and-processes-with-clone/  (h/t)
 * And: man clone(2)
 */

// TODO: clone in depth & threads
//
#define _GNU_SOURCE			/* Must be on top of all other #include's (Why? TODO) */
#include <stdio.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>


#define STACK_SIZE (1UL << 16)

static int child_function(void *arg);


static int child_function(void *arg)
{
	//struct utsname uts; /* TODO: hostname.. */
	char *buf = (char *) arg;

	printf("> Hey there, talking from child process\n");
	/* The point as the blog post (top link) implies is that since virtual
	 * memory is the same for both processes, the same mem addr point to
	 * the same objects. So mem addr of buf should be same.
	 */
	printf("> Here's mem addr of buf from child process: @%p\n", (void *) buf);

	return 0;
}

int main(int argc, char *argv[])
{
	/* We first allocate mem for the stack of the child process */
	char *stack = malloc(STACK_SIZE);

	if (!stack) {
		fprintf(stderr, "Couldn't allocate memory for the stack for the child process\n");
		perror("malloc() returned null\n");
		exit(1);
	}

	/* flags for clone() */
	unsigned long flags = 0;
	/* "If CLONE_VM is set, the calling process and the child process run
	 * in the  same memory  space.   In particular, memory writes performed
	 * by the calling process or by the child process are also visible in
	 * the other process.  Moreover,  any memory  mapping  or unmapping
	 * performed with mmap(2) or munmap(2) by the child or calling process
	 * also affects the other process." -- man clone(2)
	 */
	flags |= CLONE_VM;

	/* "The SIGCHLD signal is sent to the parent of a child process when it
	 * exits"
	 * -- https://en.wikipedia.org/wiki/Child_process
	 * We tell the kernel to send that signal when the child process dies.
	 */
	flags |= SIGCHLD;

	char buf[1000];
	memset(buf, 0, sizeof(buf));
	strncpy(buf, "Hey there. This side of the Mississippi, Maine, Vermont. Random.", 64);

	printf("Info:\n");
	printf("\tstack is at: @%p\n", (void *) stack);
	printf("\tstack + STACK_SIZE is at: @%p\n", (void *) stack + STACK_SIZE);
	printf("\tbuf is at: @%p\n", (void *) buf);

	/* this new line is outputted right before we create child process */
	printf("\n");
	if (clone(child_function, stack + STACK_SIZE, flags, buf) == -1) {
		fprintf(stderr, "Something went wrong when issuing clone() system call.\n");
		perror("clone() failed\n");
		exit(1);
	}

	/* Ok, now at this point of the code path, a new process has been created.
	 * As the parent process, we wait() [syscall] for our child to die.
	 * I assume the ..alerting part is done via SIGCHLD? (TODO)
	 */

	/* See: man wait(3p) and link at the top */
	int wstatus;

	if (wait(&wstatus) == -1) {
		perror("wait() failed\n");
		exit(1);
	}
	/* this new line is for right after child process dies */
	printf("\n");

	printf("Information of child process after it finished execution:\n");
	printf("\tExit status: %d\n", WEXITSTATUS(wstatus));
	printf("Buffer (as seen from parent): \"%s\"\n", buf);

	return 0;
}
