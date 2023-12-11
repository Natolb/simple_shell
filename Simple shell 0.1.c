#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024
#define PROMPT "#cisfun$ "

void display_prompt(void);
char *read_line(void);

int main(void)
{
    char *command;

    while (1)
    {
        display_prompt();
        command = read_line();

        if (!command)
            break;

        if (fork() == 0)
        {
            /* Child process */
            if (execve(command, NULL, NULL) == -1)
            {
                perror("./shell");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            /* Parent process */
            wait(NULL);
        }

        free(command);
    }

    return 0;
}

void display_prompt(void)
{
    write(STDOUT_FILENO, PROMPT, strlen(PROMPT));
}

char *read_line(void)
{
    char *buffer = NULL;
    size_t bufsize = 0;

    if (getline(&buffer, &bufsize, stdin) == -1)
    {
        free(buffer);
        return NULL; // NULL indicates end of file
    }

    // Remove newline character
    buffer[strcspn(buffer, "\n")] = '\0';

    return buffer;
}
