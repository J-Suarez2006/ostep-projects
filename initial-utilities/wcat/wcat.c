#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024
#define BUFFER_RESIZE_INCREASE 512

int main(int argc, char* argv[])
{

    if (argc < 2)
    {
        return 0;
    }

    for (int i = 1; i < argc; ++i)
    {
        char* buffer = malloc(BUFFER_SIZE * sizeof(char));
        int size = 0;
        int capacity = BUFFER_SIZE;

        FILE* file = fopen(argv[i], "r");

        if (file == NULL)
        {
            printf("wcat: cannot open file\n");
            return 1;
        }

        int bytes_read = 0;

        do
        {
            bytes_read = fread(buffer + size, sizeof(char), capacity - size, file);
            size += bytes_read / sizeof(char);

            if (size == capacity)
            {
                char* temp = realloc(buffer, size + BUFFER_RESIZE_INCREASE);

                if (temp == NULL)
                {
                    printf("Could not resize buffer! Error: %s", strerror(errno));
                    return -1;
                }

                capacity += BUFFER_RESIZE_INCREASE;
                buffer = temp;
            }
        } 
        while (bytes_read != 0);
        
        buffer[size] = '\0';

        printf("%s", buffer);
        free(buffer);
    }    
}