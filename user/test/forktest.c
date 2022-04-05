#include <stdio.h>      /* for printf() and fprintf() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <unistd.h>     /* for close() */
#include <time.h>       /* for time() */
#include <signal.h>     /* for signal() */
#include <sys/stat.h>   /* for stat() */
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sched.h>
#include <string.h>

int main( int argc, char *argv[] ){
	//fork when receive keyboard interrupt
	pid_t parpid = getpid();
	int ret;
	int i;
	int j;
	int k;
	int sched_policy;
	struct sched_param freezer_param;
	if(argc == 2){
		sched_policy = sched_getscheduler(parpid);
		freezer_param.sched_priority = 0;
		ret = sched_setscheduler(parpid, 7, &freezer_param);
		if(ret == 0){
         printf("[%d] sched policy changed: %d --> %d\n", parpid, sched_policy, 7);
		}
		else{
			printf("%s \n", strerror(errno));
		}
		int count = strtol(argv[1], NULL, 10);
		pid_t pid[count];
		for(i = 0; i<count; i++) {
			pid[i] = fork();
			if(!pid[i]){
				for(k = 1; k < 10000000; k++){}
				return 0;
			}
		}
		for(j = 0; j < count; j++) {
			waitpid(pid[j], 0, 0);
		}
	}
	
   else {
      printf("One argument expected.\n");
   }
   return 0;
}
