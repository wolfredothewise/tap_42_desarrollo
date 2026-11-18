#include <stdio.h>
#include <stdlib.h>

char *read_file_to_string(const char *file_path)
{
    FILE *fp = fopen(file_path, "rb");
    if (!fp)
        {
            printf("File not found.\n");
            return NULL;
        }

    fseek(fp, 0, SEEK_END);//move to the end
    long size = ftell(fp); //tamaño
    rewind(fp);
    if(size < 0)
        {
            fclose(fp);
            printf("Error: Empty file.\n");
            return NULL;
        }
    char *content = malloc(size + 1);
    if (!content)
    {
        printf("Failling reservating memory with malloc.\n");
        fclose(fp);
        return NULL;
    }
    size_t read_bytes = fread(content, 1, size, fp);
    fclose(fp);
    if (read_bytes != (size_t)size)
        {
            printf("Failure related with bytes syze\n");
            free(content);
            return NULL;
        }
    content[size] = '\0';
    return content;
}