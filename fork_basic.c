/*
 * See: man fork(2). "creates a new process by duplicating the calling process"
 */
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	pid_t fork_ret_val = 0;

	printf("Pre-forking message.\n");

	/* parent process will get the PID of child process as return value
	 * from fork(). Child will get 0. */
	fork_ret_val = fork();

	printf("Post-forking message. (both child and parent will execute this)\n");
	printf("fork() returned %d\n", fork_ret_val);

	return 0;
}
