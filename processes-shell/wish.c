#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define BUFFER_INIT_CAPACITY 256
#define BUFFER_RESIZE_AMT 128

#define CHILD 0

typedef struct 
{
    char** args;
    int size;
    int capacity;
} da_str;

void da_str_INIT(da_str* da)
{
    da->size = 0;
    da->capacity = BUFFER_INIT_CAPACITY;
    da->args = malloc(sizeof(char**) * da->capacity);
}

void da_str_CLEAR(da_str* da)
{
    da->size = 0;
    da->capacity = BUFFER_INIT_CAPACITY;
    free(da->args);
    da->args = malloc(sizeof(char**) * da->capacity);
}

void da_str_APPEND(da_str* da, char* str)
{
    if (da->size == da->capacity)
    {
        char** temp = realloc(da->args, da->capacity + BUFFER_RESIZE_AMT);

        if (temp == NULL)
        {
            printf("Could not reallocate memory for dynamic array! Error:%s\n", strerror(errno));
            exit(1);
        }

        da->args = temp;
        da->capacity += BUFFER_RESIZE_AMT;
    }

    da->args[da->size] = str;
    da->size++;
}


int main(int argc, char* argv)
{
    char error_message[30] = "An error has occurred\n";
    da_str paths;
    da_str_INIT(&paths);

    if (argc == 1)
    {
        char* buffer = NULL;
        ssize_t size = 0;

        while (true)
        {
            // read input
            printf("wish> ");
            getline(&buffer, &size, stdin);
            
            // get command and args from buffer
            char* command = strsep(&buffer, " \n\t");

            da_str args;
            da_str_INIT(&args);

            

            char* arg;
            while ((arg = strsep(&buffer, " \n\t")) != NULL)
            {
                if (isspace(arg[0]) || (arg[0] == '\0')) continue;
                da_str_APPEND(&args, arg);
            }

            /*
            for (int i = 0; i < args.size; ++i)
            {
                printf("arg #%d: %s\n", i, args.args[i]);
            }
                */

            if (strcmp(command, "exit") == 0)
            {
                if (args.size != 0)
                {
                    fprintf(stderr, "An error has occurred\n");
                }
                else
                    exit(0);
            }
            else if (strcmp(command, "cd") == 0)
            {
                if (args.size != 1)
                {
                    write(STDERR_FILENO, error_message, strlen(error_message)); 
                    //fprintf(stderr, "An error has occurred\n");
                }
                else
                {
                    if (chdir(args.args[0]) != 0)
                    {
                        write(STDERR_FILENO, error_message, strlen(error_message)); 
                    }
                }
                
            }
            else if (strcmp(command, "path") == 0)
            {
                for (int i = 0; i < args.size; ++i)
                {   
                    if (access(args.args[i], F_OK) == 0)
                    {
                        char* path = malloc(sizeof(char) * strlen(args.args[i]));
                        memcpy(path, args.args[i], strlen(args.args[i]));

                        //printf("added path:%s\n", path);
                        da_str_APPEND(&paths, path);
                    }
                }
            }
            else
            {
                if (command == NULL) continue;

                pid_t pid = fork();

                if (pid < CHILD)
                {
                    fprintf(stderr, "An error has occurred\n");
                }
                else if (pid == CHILD)
                {
                    char* path = strdup(command);

                    if (access(path, F_OK) == 0)
                    {
                        execv(path, args.args);
                    }

                    free(path);

                    for (int i = 0; i < paths.size; ++i)
                    {
                        char* path = malloc((strlen(command) + strlen(paths.args[i]) + 1) * sizeof(char));
                        memcpy(path, paths.args[i], sizeof(char) * strlen(paths.args[i]));
                        strcat(path, "/");
                        strcat(path, command);

                        if (access(path, F_OK) == 0)
                        {
                            execv(path, args.args);
                            free(path);
                            exit(0);
                        }

                        free(path);
                    }

                    exit(0);
                }
                else
                {
                    wait(NULL);
                }
            }

            /*
            for (int i = 0; i < paths.size; ++i)
            {
                printf("path #%d: %s\n", i, paths.args[i]);
            }
                */

            free(buffer);
            free(args.args);
            free(command);
        }
    }

    for (int i = 0; i < paths.size; ++i)
    {
        free(paths.args[i]);
    }
    free(paths.args);
}