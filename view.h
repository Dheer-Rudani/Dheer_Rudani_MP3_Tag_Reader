#ifndef VIEW_H
#define VIEW_H

#include "common.h"

typedef struct 
{
    char buffer[50];
    int size_buffer;
    FILE *file;
}ViewInfo;

//Clear buffer
void clear_buffer(char *buff);

//Check the operation type
OperationType check_operation_type(char *op, int size);

//Validate whether its a ID3 or not
Status read_and_validate_view(char **argv, ViewInfo *viewinfo);

//Perform the viewing
Status do_viewing(ViewInfo *viewinfo);

#endif