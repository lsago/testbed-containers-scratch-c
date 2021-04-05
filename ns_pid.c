/*
 * Playground to tinker with / learn about PID ns.  Mostly following Michael
 * Kerrisk's pidns_init_sleep.c which can be found at:
 * https://lwn.net/Articles/532741/ - gplv2
 */

#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <sched.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

#define STACK_SIZE (1UL << 16)

static int child_function(void *arg);

int child_function(void *arg)
{
	printf(" child> PID: %d\n", getpid());
	printf(" child> PPID: %d\n", getppid());

	/*execlp("sleep", "sleep", "6", (char *) NULL);*/

	system("/bin/bash");
	/*
	 * Meah, ended up being just like clone_example.c
	 * But once you're on the shell you can do:
	 * 	# echo $$      (should be 2)
	 * 	You can check that /proc contains the original PID namespace.
	 * 	You can still access it, but syscalls (think: kill etc..) use
	 * 	the new PIDs.
	 * 	Also, you can mount a "new" proc and this one will contain new PID ns
	 * 	# mount -t proc proc /mnt/proc_new_ns
	 * 	And you can check that /mnt/proc_new_ns/$$/ns/pid*
	 * 	are not the same in this shell than in the original name space
	 */

	return 0;
}

int main(int argc, char *argv[])
{
	char child_stack[STACK_SIZE];
	unsigned int flags = 0;
	pid_t child_pid;

	if (geteuid() != 0) {
		fprintf(stderr, "You need to be root to create a new PID namespace (?)\n");
		exit(1);
	}

	printf("parent> just started\n");
	printf("parent> PID: %d\n", getpid());
	printf("parent> PPID: %d\n", getppid());

	/* tells the kernel to send the signal SIGCHLD to the parent once the child ends */
	flags |= SIGCHLD;

	/* create new PID ns */
	flags |= CLONE_NEWPID;

	child_pid = clone(child_function, child_stack + STACK_SIZE, flags, argv[1]);

	if (child_pid == -1) {
	        fprintf(stderr, "Something went wrong when issuing clone() system call.\n");
	        perror("clone() failed\n");
	        exit(1);
	}


	printf("parent> child's PID from parent: %d\n", child_pid);


	if (waitpid(child_pid, NULL, 0) == -1) {
		fprintf(stderr, "Something went wrong while trying to wait for child process to finish\n");
		perror("waitpid");
		exit(1);
	}

	return 0;
}


