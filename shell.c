#include "main.h"

#define MAX_LEN 100
#define PROMPT ">> "

void handle_command(char *u_command)
{
	char *args[MAX_LEN], *command = strtok(u_command, " \t");
	char *paths[] = {"/bin/", "/usr/bin/", "/usr/local/bin/", ""};
	pid_t pid;
	int status, i = 0, found = 0;
	long unsigned int j;

	args[0] = NULL;
	while (command != NULL && i < MAX_LEN - 1)
	{
		args[i] = command;
		i++;
		command = strtok(NULL, " \t");
	}

	if (args[0] == NULL)
		return;

	args[i] = NULL;
	for (j = 0; j < sizeof(paths) / sizeof(paths[0]); j++)
	{
		char *path = malloc(strlen(paths[j]) + strlen(args[0]) + 1);
		if (path == NULL)
		{
			perror("Malloc failed");
			exit(EXIT_FAILURE);
		}
		strcpy(path, paths[j]);
		strcat(path, args[0]);
		if (access(path, X_OK) != -1)
		{
			found = 1;
			free(path);
			break;
		}
		free(path);
	}
	if (!found)
	{
		fprintf(stderr, "./hsh: 1: %s: not found\n", args[0]);
		exit(127);
	}
	
	pid = fork();
	if (pid == -1)
	{
		perror("fork failed");
		exit(EXIT_FAILURE);
	}
	else if (pid == 0)
	{
		if (execvp(args[0], args) == -1)
		{
			fprintf(stderr, "./hsh: 1: %s: not found\n", args[0]);
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		if (wait(&status) == -1)
		{
			perror("wait failed");
			exit(EXIT_FAILURE);
		}
	}
}

int main(void)
{
	char commands[MAX_LEN];
	char *command;
	char *commands_array[MAX_LEN];
	int a = 0, status = 0;

	while (1)
	{
		ssize_t read_size;

		read_size = read(STDIN_FILENO, commands, MAX_LEN);
		if (read_size == -1)
		{
			perror("Error reading command");
			exit(EXIT_FAILURE);
		}
		else if (read_size == 0)
			break;
		commands[read_size] = '\0';
		a = 0;
		command = strtok(commands, "\n");
		while (command != NULL)
		{
			commands_array[a] = command;
			command = strtok(NULL, "\n");
			a++;
		}
		commands_array[a] = NULL;
		
		a = 0;
		if (strcmp(commands_array[a], "exit") == 0)
			exit(status);
		while (commands_array[a] != NULL)
		{
			command = commands_array[a];
			if (strcmp(command, "exit") == 0 && a > 0)
				exit(2);
			handle_command(command);
			a++;
		}
	}
	return (0);
}
