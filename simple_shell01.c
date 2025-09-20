#include <stddef.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

/**
 * print_prompt - prints the shell prompt
 */
void printPrompt(void)
{
	write(STDOUT_FILENO, "#cisfun$ ", 9);
}

char *readCommand(void)
{
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	read = getline(&line, &len, stdin);
	if (read == -1){
		/* Handle EOF (Ctr+D) */
		if (line)
			free(line);
		return (NULL);
	}

	/* Remove newline character */
	if (read > 0 && line[read - 1] == '\n')
		line[read - 1] = '\0';

	return (line);
}

/**
 * execute_command - executes a command
 * @command: the command to execute
 */
void excuteCommand(char *cmd, char **env)
{
	pid_t pid;
	int status;
	char *argv[2];

	/* Skip empty commands */
    if (strlen(cmd) == 0)
        return;
	argv[0] = cmd;
	argv[1] = NULL;

	pid = fork();
	if (pid == -1){
		perror("fork");
		return ;
	}
	if (pid == 0) {
		/* Child process */
		if (execve(cmd, argv, env) == -1){
			write(STDERR_FILENO, "./shell: No such file or directory\n", 35);
			exit(EXIT_FAILURE);
		}
	}
	else {
		/* Parent process - wait for child */
		waitpid(pid, &status, 0);
	}
}

/**
 * main - simple shell main function
 * Return: 0 on success
 */
int main(int ac __attribute__((unused)), char **av __attribute__((unused)), char **env)
{
	char *cmd;

	while (true) {
		printPrompt();
		cmd = readCommand();
		if (!cmd){
			write(STDOUT_FILENO, "\n", 1);
			break ;
		}
		excuteCommand(cmd, env);
		free(cmd);
	}
	return (0);
}