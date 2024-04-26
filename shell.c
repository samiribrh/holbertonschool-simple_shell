#include "main.h"
#include <dirent.h>

#define MAX_LEN 100
#define PROMPT ">> "

void handle_command(char *command)
{
	char **args = {command, NULL};
	pid_t pid;
	int status;

	pid = fork();
	if (pid == -1)
	{
		perror("Fork failed");
		return;
	}
	else if (!pid)
	{
		if (execve(command, args, NULL) == -1)
		{
			perror("Execve failed");
			exit(0);
		}
	}
	else
	{
		if (waitpid(pid, &status, 0) == -1)
		{
			perror("Waitpid failed");
			exit(0);
		}
	}
}

int main(void)
{
	char command[MAX_LEN];

	while (1)
	{
		write(STDOUT_FILENO, PROMPT, strlen(PROMPT));
		if (fgets(command, MAX_LEN, stdin) == NULL)
		{
			if (feof(stdin))
				break;
			perror("Error reading command");
			continue;
		}

		command[strcspn(command, "\n")] = '\0';
		handle_command(command);
	}
	return (0);
}
