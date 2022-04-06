#include<stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <sched.h>
#include <errno.h>
#include <string.h>
#include <linux/sched.h>

int main(int argc, char *argv[])
{
	int ret;
	int sched_policy;
	struct sched_param freezer_param;

	freezer_param.sched_priority = 0;

	if (argc == 2) {
		pid_t pid = strtol(argv[1], NULL, 10);

		sched_policy = sched_getscheduler(pid);
		ret = sched_setscheduler(pid, 7, &freezer_param);
		if (ret == 0)
			printf("[%d] sched policy changed: %d --> %d\n", pid, sched_policy, 7);
		else
			printf("%s\n", strerror(errno));
	} else if (argc > 2) {
		printf("Too many arguments supplied.\n");
	} else {
		printf("One argument expected.\n");
	}
	return 0;
}
