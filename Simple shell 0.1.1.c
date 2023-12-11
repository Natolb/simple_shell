#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARG_SIZE 64
#define PROMPT ":) "

char *custom_getline(void);
void display_prompt(void);
char **tokenize_input(char *input);

int main(void)
{
    char *input;
    char **args;

    while (1)
    {
        display_prompt();
        input = custom_getline();

        if (!input)
            break;

        args = tokenize_input(input);

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
        else if (strcmp(args[0], "env") == 0)
        {
            char **env = environ;

            // Print the current environment
            while (*env != NULL)
            {
                printf("%s\n", *env);
                env++;
            }
        }
        else
        {
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
        }

        // Free allocated memory
        free(input);
        for (int i = 0; args[i] != NULL; i++)
        {
            free(args[i]);
        }
        free(args);
    }

    return 0;
}

char *custom_getline(void)
{
    static char buffer[MAX_INPUT_SIZE];
    static size_t pos = 0;

    ssize_t read_chars;

    // Reset buffer if it's the first call or we've reached the end
    if (pos == 0)
        memset(buffer, 0, MAX_INPUT_SIZE);

    // Read characters into the buffer
    read_chars = read(STDIN_FILENO, buffer + pos, MAX_INPUT_SIZE - pos);

    // Check for errors or end of file
    if (read_chars == -1 || read_chars == 0)
        return NULL;

    // Find the newline character or return if not found
    char *newline = strchr(buffer + pos, '\n');
    if (newline != NULL)
    {
        *newline = '\0'; // Replace newline with null terminator
        pos = 0;         // Reset position for the next call
        return buffer;
    }

    // Update position for the next call
    pos += read_chars;

    // If the buffer is full, consider it as a complete line
    if (pos == MAX_INPUT_SIZE)
    {
        pos = 0; // Reset position for the next call
        return buffer;
    }

    // If newline is not found yet, wait for more input
    return custom_getline();
}

void display_prompt(void)
{
    write(STDOUT_FILENO, PROMPT, strlen(PROMPT));
}

char **tokenize_input(char *input)
{
    char **args = (char **)malloc(MAX_ARG_SIZE * sizeof(char *));
    if (!args)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    char *token = strtok(input, " ");
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

    return args;
}
