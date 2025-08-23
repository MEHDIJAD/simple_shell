#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>  
#include <stdbool.h>
#include <string.h>


int main(int ac __attribute__((unused)) , char **av __attribute__((unused)))
{
	char *line = NULL;
	size_t len = 0;
	pid_t pid;
	int status;

	while (true)
	{
		printf("#cisfun$ ");
		if (getline(&line, &len, stdin) == -1)
			return (EXIT_FAILURE);
		line[strcspn(line, "\n")] = '\0';
        if (strlen(line) == 0)
            continue;
		pid = fork();
		if (pid == -1)
			return (EXIT_FAILURE);
		else if (pid == 0){
			char *cmd[] = {line, NULL};
			if (execve(line, cmd, NULL)){
				perror("execv");
				exit(EXIT_FAILURE);
			}
		}
		else{
			wait(&status);
		}
	}
	free(line);
	return(EXIT_SUCCESS);	
}

