#include<stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <sched.h>


static void die(const char *message)
{
	perror(message);
	exit(1);
}

int main(void)
{
	int i = 0;
	pid_t pid;
	struct sched_param real_time_param;

	real_time_param.sched_priority = 30;

	for (i = 0; i < 5; i++) {
		pid = fork();
		if (pid < 0) {
			die("fork error");
		} else if (pid == 0) { //child
			while (1)
				;
			return 0;
		}
	}

	pid = fork();
	if (pid < 0) {
		die("fork error");
	} else if (pid == 0) { //child
		pid = fork();
		if (pid < 0) {
			die("fork error");
		} else if (pid == 0) { //child
			sched_setscheduler(0, SCHED_FIFO, &real_time_param);
			while (1)
				;
			return 0;
		}
		pid = waitpid(-1, NULL, 0);
	}

	for (i = 0; i < 6; i++)
		pid = waitpid(-1, NULL, 0);

	return 0;
}
