#include <stdio.h>
#include <string.h>
#include "common.h"
#include "edit.h"

Status read_and_validate_edit(char **argv, EditInfo *editinfo)
{
    char data[3];
    editinfo->src_fptr = fopen(argv[3],"rb");
    
    if(editinfo->src_fptr == NULL)
    {
        printf("Error!: Failed to open the file.\n");
        return failure;
    }

    if(fread(data, 3, 1, editinfo->src_fptr) != 1)
    {
        printf("Error!: Failed to read data from the file.\n");
        return failure;
    }

    if(strcasecmp(data, "ID3") != 0)
    {
        return failure;
    }
    //printf("INFO: Read and validation successful\n");
    return success;    
}

Status copy_header(FILE *src, FILE *temp)
{
    char arr[10];//Buffer
    fseek(src, 0, SEEK_SET);
    if(fread(arr, 10, 1, src) != 1)//Reading 10 bytes header
    {
        return failure;
    }
    if(fwrite(arr, 10, 1, temp) != 1)//Writing the 10 bytes of data to temp file
    {
        return failure;
    }
    return success;
}

Status copy_to_original(char **argv)
{
    char chr[20480];//Reading the 20KB of data (to reduce the time)
    unsigned int read;//Buffer to store the data read
    FILE *src = fopen("temp.mp3", "rb");//opening the temp file in read mode
    FILE *temp = fopen(argv[3], "wb");//opening the original file in write mode
    if(!src || !temp)
    {
        return failure;
    }
    while((read = fread(chr, 1, sizeof(chr), src)) > 0)//Copying the whole data from temp to original
    {
        if(fwrite(chr, 1, read, temp) != read)
        {
            return failure;
        }
    }
    fclose(src);
    fclose(temp);
    return success;
}

Status copy_remaining_data(FILE *src, FILE *temp)
{
    char ch[20480];//Reading the 20KB of data (to reduce the time)
    unsigned int read_data;//Buffer to store the data read
    while((read_data = fread(ch, 1, sizeof(ch), src)) > 0)//Reading 20KB of data at once
    {
        if(fwrite(ch, 1, read_data, temp) != read_data)//Writing the data of size stored in read buffer
        {
            return failure;
        }
    }
    return success;
}

int big_to_little_edit(int size)
{
    size = ((size >> 24) & 0x000000FF) | ((size >> 8) & 0x0000FF00) | ((size << 8) & 0x00FF0000) | ((size << 24) & 0xFF000000);//BIG->LITTLE or LITTLE->BIG
    return size;
}

Status copy_data(EditInfo *editinfo)
{
    char data;
    if(fread(&editinfo->size_buffer, 4, 1, editinfo->src_fptr) != 1)//Reading the other tag data size
    {
        printf("Error! Failed to copy data\n");
        return failure;
    }
    editinfo->size_buffer = (big_to_little_edit(editinfo->size_buffer) - 1);//Converting to little endian
    fseek(editinfo->src_fptr, -8, SEEK_CUR);
    for(int i = 0; i < (editinfo->size_buffer + 11); i++)//Copying the TAG, its data size, flag data and TAG_data
    {
        if(fread(&data, 1, 1, editinfo->src_fptr) != 1)
        {
            printf("Error!: Failed to read the data(copy_data)\n");
            return failure;
        }
        if(fwrite(&data, 1, 1, editinfo->temp_fptr) != 1)
        {
            printf("Error!: Failed to write the data(copy_data)\n");
            return failure;
        }
    }
    return success;
}

Status edit_content(EditInfo *editinfo)
{
    char data;
    if(fread(&editinfo->size_buffer, 4, 1, editinfo->src_fptr) != 1)//Reading the size of the data stored in the tag
    {
        printf("Failed to read the size\n");
        return failure;
    }
    editinfo->size_buffer = big_to_little_edit(editinfo->size_buffer);//Converting the data from big to little
    fseek(editinfo->src_fptr, editinfo->size_buffer + 2, SEEK_CUR);//Moving the source file ptr to next tag
    int temp = big_to_little_edit(editinfo->size_content + 1);//Converting it from little to big (Applying the same function)
    
    if(fwrite(&temp, 4, 1, editinfo->temp_fptr) != 1)//Writing the size of new data
    {
        printf("Failed to write the size\n");
        return failure;
    }

    char arr[3] = {'\0', '\0', '\0'};
    if(fwrite(arr, 3, 1, editinfo->temp_fptr) != 1)//Writing the flag data + NULL
    {
        printf("Failed to add flag value\n");
        return failure;
    }

    if(fwrite(editinfo->content, editinfo->size_content, 1, editinfo->temp_fptr) != 1)//Writing the new content
    {
        printf("Failed to write the content\n");
        return failure;
    }

    return success;
}

Status edit_data(int j,EditInfo *editinfo)
{
    char data[4];//Buffer data for edit
    char *mod_data[] = { "Title","Artist", "Album", "Year", "Content_type", "Comment"};
    int test = 1;//buffer integer
    for(int i = 0; i < 6; i++)//Performing the operation for 6 tags
    {
        if(fread(data, 4, 1, editinfo->src_fptr) != 1)
        {
            return failure;
        }
        test = strcmp(data,tag[j]);//Comparing the tags

        if(test == 0)
        {
            if(fwrite(data, 4, 1, editinfo->temp_fptr) != 1)//Writing the tag 
            {
                printf("Error!: Failed to write the tag\n");
                return failure;
            }
            if(edit_content(editinfo) == failure)//Function call to edit
            {
                printf("Error!: Failed to edit the data\n");
                return failure;
            }
            printf("-------------------------------------------------------------------------------\n");
            printf("MP3 Tag Reader and Editor for ID3v2\n");
            printf("-------------------------------------------------------------------------------\n");
            printf("%-20s: %s\n", mod_data[j], editinfo->content);
            printf("-------------------------------------------------------------------------------\n");
            break;
        }
        else
        {
            if(copy_data(editinfo) == failure)//Function call to copy tag data
            {
                printf("Error!: Failed to copy the data\n");
                return failure;
            }
        }
    }

    if(test == 1)
    {
        printf("Error!: Tag not found\n");
        return failure;
    }
    
    return success;  
}

Status match_tag(char *argv, EditInfo *editinfo)
{
    //Compare the tag from argv and then perform desired operation
    if(strstr(argv, "-t") != NULL)
    {
        edit_data(0, editinfo);
    } 
    else if(strstr(argv, "-a") != NULL)
    {
        edit_data(1, editinfo);
    }
    else if(strstr(argv, "-A") != NULL)
    {
        edit_data(2, editinfo);
    }
    else if(strstr(argv, "-y") != NULL)
    {
        edit_data(3, editinfo);
    }
    else if(strstr(argv, "-c") != NULL)
    {
        edit_data(4, editinfo);
    }
    else if(strstr(argv, "-C") != NULL)
    {
        edit_data(5, editinfo);
    }
    else
    {
        printf("Error! Tag not found\nINFO: To get help type -> ./a.out -h\n");
        return failure;
    }
    return success;
}

Status do_editing(char **argv,EditInfo *editinfo)
{
    editinfo->temp_fptr = fopen("temp.mp3", "wb");
    editinfo->content = argv[4];
    editinfo->size_content = strlen(argv[4]);
    
    //Copy the mp3 header
    if(copy_header(editinfo->src_fptr,editinfo->temp_fptr) == failure)
    {
        printf("Error!: Failed to copy the mp3 header\n");
        return failure;
    }

    //Read the tags and if matched, then perform the desired operation
    if(match_tag(argv[2], editinfo) == failure)
    {
        return failure;
    }

    //Copy the remaining data
    if(copy_remaining_data(editinfo->src_fptr, editinfo->temp_fptr) == failure)
    {
        printf("Error!: Failed to copy the remaining data\n");
        return failure;
    }
    fclose(editinfo->src_fptr);
    fclose(editinfo->temp_fptr);
    return success;
}