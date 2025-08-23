#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h> 


/**
 * Write a program that executes the command ls -l /tmp in 5 different child processes. 
 * Each child should be created by the same process (the father). 
 * Wait for a child to exit before creating a new child.
 */


int main(void)
{
	char *cmd[] = {"/bin/ls", "-l", NULL};
	pid_t pid;
	int status;
	int i = 0;

	while (i < 5){
		pid = fork();
		if (pid == -1)
			return(EXIT_FAILURE);
		else if (pid == 0){
			if (execve(cmd[0], cmd, NULL) == -1)
				return (EXIT_FAILURE);
		}
		else
			wait(&status);
		i++;
	}
	return (EXIT_SUCCESS);
} 
