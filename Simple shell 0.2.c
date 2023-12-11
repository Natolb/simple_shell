#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARG_SIZE 64
#define PROMPT ":) "

void display_prompt(void);
char **read_line(void);

int main(void)
{
    char **args;

    while (1)
    {
        display_prompt();
        args = read_line();

        if (!args)
            break;

        if (strcmp(args[0], "exit") == 0)
        {
            // Free allocated memory
            for (int i = 0; args[i] != NULL; i++)
            {
                free(args[i]);
            }
            free(args);

            exit(EXIT_SUCCESS);
        }

        pid_t pid = fork();
        if (pid == 0)
        {
            /* Child process */
            if (execvp(args[0], args) == -1)
            {
                perror(args[0]);
                exit(EXIT_FAILURE);
            }
        }
        else if (pid < 0)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else
        {
            /* Parent process */
            int status;
            waitpid(pid, &status, 0);

            // Check for command not found
            if (WIFEXITED(status) && WEXITSTATUS(status) == 127)
            {
                fprintf(stderr, "%s: command not found\n", args[0]);
            }
        }

        // Free allocated memory
        for (int i = 0; args[i] != NULL; i++)
        {
            free(args[i]);
        }
        free(args);
    }

    return 0;
}

void display_prompt(void)
{
    write(STDOUT_FILENO, PROMPT, strlen(PROMPT));
}

char **read_line(void)
{
    char *buffer = NULL;
    size_t bufsize = 0;

    if (getline(&buffer, &bufsize, stdin) == -1)
    {
        free(buffer);
        return NULL; // NULL indicates end of file (Ctrl+D)
    }

    // Remove newline character
    buffer[strcspn(buffer, "\n")] = '\0';

    // Tokenize the input into arguments
    char **args = (char **)malloc(MAX_ARG_SIZE * sizeof(char *));
    if (!args)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    char *token = strtok(buffer, " ");
    while (token != NULL)
    {
        args[i] = strdup(token);
        if (!args[i])
        {
            perror("strdup");
            exit(EXIT_FAILURE);
        }
        i++;

        token = strtok(NULL, " ");
    }
    args[i] = NULL;

    free(buffer);

    return args;
}
