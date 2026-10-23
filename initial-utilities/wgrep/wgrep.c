#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_INIT_CAPACITY 256
#define BUFFER_RESIZE_AMT 128

typedef struct
{
    char* str;
    size_t size;
    size_t capacity;
} Buffer;

bool customgetline(FILE* file, Buffer* buffer)
{  
    memset(buffer->str, 0, buffer->capacity);
    buffer->size = 0;

    int bytes_read = fread(buffer->str + buffer->size, sizeof(char), 1, file);
    if (bytes_read == 0) return false;

    buffer->size++;

    while(buffer->str[buffer->size - 1] != '\n')
    {
        int bytes_read = fread(buffer->str + buffer->size, sizeof(char), 1, file);
        if (bytes_read == 0) break; 

        buffer->size++;

        if (buffer->size == buffer->capacity)
        {
            char* temp = (char*)realloc(buffer->str, buffer->capacity + BUFFER_RESIZE_AMT);

            if (temp == NULL)
            {
                printf("Could not resize buffer! Error: %s\n", strerror(errno));
                return false;
            }

            buffer->capacity += BUFFER_RESIZE_AMT;
            buffer->str = temp;
        }
    }
    
    buffer->str[buffer->size] = '\0';
    
    return true;
}

bool stdingetline(Buffer* buffer)
{
    buffer->size = 0;

    buffer->str[buffer->size] = (char)fgetc(stdin);
    if (buffer->str[buffer->size] == EOF)
    {
        return false;
    }

    buffer->size++;

    while(buffer->str[buffer->size - 1] != '\n')
    {
        buffer->str[buffer->size] = (char)fgetc(stdin);

        buffer->size++;

        if (buffer->size == buffer->capacity)
        {
            char* temp = (char*)realloc(buffer->str, buffer->capacity + BUFFER_RESIZE_AMT);

            if (temp == NULL)
            {
                printf("Could not resize buffer! Error: %s\n", strerror(errno));
                return false;
            }

            buffer->capacity += BUFFER_RESIZE_AMT;
            buffer->str = temp;
        }
    }
    
    buffer->str[buffer->size] = '\0';
    
    return true;
}

bool contains(char* str, char* search_for_me)
{
    /*
    char* split = strdup(str);
    char* word = strtok(split, " \n\t");

    while (word != NULL)
    {
        if (strcmp(word, search_for_me) == 0)
        {
            free(split);
            return true;
        }

        word = strtok(NULL, " \n\t");
    }

    free(split);
    return false;
    */
    if(strlen(search_for_me) > strlen(str)) return false;

    char* begin = str;
    char* end = str + strlen(search_for_me);

    while (end != str + strlen(str))
    {
        char* new_str = malloc(strlen(search_for_me));
        memcpy(new_str, begin, end - begin);

        //printf("%s\n", new_str);

        if (strncmp(new_str, search_for_me, strlen(search_for_me)) == 0)
        {
            free(new_str);
            return true;
        }

        begin++;
        end++;
        free(new_str);
    }

    return false;
}

int main(int argc, char* argv[])
{
    Buffer buffer;
    buffer.size = 0;
    buffer.capacity = BUFFER_INIT_CAPACITY;
    buffer.str = (char*)malloc(BUFFER_INIT_CAPACITY * sizeof(char));

    if (argc < 2)
    {
        printf("wgrep: searchterm [file ...]\n");
        return 1;
    }

    if (argc == 3)
    {
        FILE* file = fopen(argv[2], "r");
        if (file == NULL)
        {
            printf("wgrep: cannot open file\n");
            return 1;
        }

        while(customgetline(file, &buffer))
        {
            if (contains(buffer.str, argv[1]))
            {
                printf("%s", buffer.str);
            }
        }
    }

    if (argc == 2)
    {
        while(stdingetline(&buffer))
        {
            if (contains(buffer.str, argv[1]))
            {
                printf("%s", buffer.str);
            }
        }
    }

    free(buffer.str);
    return 0;
}