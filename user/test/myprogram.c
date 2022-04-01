#include<stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

static void die(const char *message)
{
    perror(message);
    exit(1);
}

int main()
{
	int i = 0;
	pid_t pid;

	for(i = 0; i < 5; i++){
		if ((pid = fork()) < 0){
			die("fork error");
		} else if (pid == 0){ //child
			while(1) ;
			return 0;
		}
	}

	pid = waitpid(-1, NULL, 0);
	pid = waitpid(-1, NULL, 0);
	pid = waitpid(-1, NULL, 0);
	pid = waitpid(-1, NULL, 0);
	pid = waitpid(-1, NULL, 0);
	return 0;
}