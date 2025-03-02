#ifndef EDIT_H
#define EDIT_H

#include "common.h"

typedef struct 
{
    int size_buffer;
    int size_content;
    FILE *src_fptr;
    FILE *temp_fptr;
    char *content;
}EditInfo;

//Clear buffer
void clear_buffer_data(char *buff);

//Read and validate the source file
Status read_and_validate_edit(char **argv,EditInfo *editinfo);

//Do_editing
Status do_editing(char **argv,EditInfo *editinfo);

//Copy_remaining_data
Status copy_remaining_data(FILE *src, FILE *temp);

//Copy_to_original
Status copy_to_original(char **argv);

#endif