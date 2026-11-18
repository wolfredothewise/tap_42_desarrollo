#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "tap_server.h"

int main(int argc, char **argv)
{
    if (argc != 2)
        return 1;
    const char *file_path = argv[1];

    char *raw_json = read_file_to_string(file_path);
    if (!raw_json)
        return 1;//el mensaje de error ya se hace en read_file...

    cJSON *root = cJSON_Parse(raw_json);
    free(raw_json);
    if (!root)
    {
        const char * e = cJSON_GetErrorPtr();
        if (!e)
            return 1;;

        printf("%s", e);
        return 1;
    }
    printf("JSON loaded succesfully\n");
    cJSON_Delete(root);
    return 0;
}
