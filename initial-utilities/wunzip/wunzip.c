#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <errno.h>

#define BUFFER_INIT_CAPACITY 256
#define BUFFER_RESIZE_AMT 128

typedef struct
{
    char* str;
    ssize_t size;
    ssize_t capacity;
} Buffer;

void initBuffer(Buffer* buffer)
{
    buffer->capacity = BUFFER_INIT_CAPACITY;
    buffer->size = 0;

    buffer->str = (char*)malloc(buffer->capacity * sizeof(char));
    memset(buffer->str, 0, buffer->capacity);
}

void readFileToBuffer(Buffer* buffer, const char* path)
{
    FILE* file = fopen(path, "r");

    if (file == NULL)
    {
        printf("Could not open file!\n");
        exit(1);
    }

    ssize_t bytes_read = 0;

    do
    {
        bytes_read = fread(buffer->str + buffer->size, sizeof(char), buffer->capacity - buffer->size, file);
        buffer->size += bytes_read / sizeof(char);

        if (buffer->size == buffer->capacity)
        {
            char* temp = realloc(buffer->str, buffer->size + BUFFER_RESIZE_AMT);

            if (temp == NULL)
            {
                printf("Could not resize buffer! Error: %s", strerror(errno));
                exit(1);
            }

            buffer->capacity += BUFFER_RESIZE_AMT;
            buffer->str = temp;
        }
    } 
    while (bytes_read != 0);
    
    buffer->str[buffer->size] = '\0';

}

void clearBuffer(Buffer* buffer)
{
    buffer->capacity = 0;
    buffer->size = 0;
    free(buffer->str);
}

void unZipBuffer(Buffer* buffer)
{
    char* current = buffer->str;
    while (*current != '\0')
    {
        u_int32_t amt_char = 0;
    
        memcpy(&amt_char, current, sizeof(u_int32_t));

        current += sizeof(u_int32_t);
        
        for (int i = 0; i < (int)amt_char; ++i)
            fwrite(current, sizeof(char), 1, stdout);

        ++current;
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("wunzip: file1 [file2 ...]\n");
        return 1;
    }

    Buffer buffer;
    initBuffer(&buffer);

    for (int i = 1; i < argc; ++i)
    {
        readFileToBuffer(&buffer, argv[i]);
    }

    unZipBuffer(&buffer);
    clearBuffer(&buffer);
}