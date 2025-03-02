#include <stdio.h>
#include <string.h>
#include "common.h"
#include "view.h"

void clear_buffer(char *buff)
{
    memset(buff, 0, 50);
}

Status read_and_validate_view(char **argv, ViewInfo *viewinfo)
{
    clear_buffer(viewinfo->buffer);
    viewinfo->file = fopen(argv[2],"rb");
    
    if(viewinfo->file == NULL)
    {
        printf("Error!: Failed to open the file.\n");
        return failure;
    }

    if(fread(viewinfo->buffer, 3, 1, viewinfo->file) != 1)
    {
        printf("Error!: Failed to read data from the file.\n");
        return failure;
    }

    if(strcasecmp(viewinfo->buffer, "ID3") != 0)
    {
        return failure;
    }
    //printf("INFO: Read and validation successful\n");
    return success;    
}

int big_to_little(int size)
{
    size = ((size >> 24) & 0x000000FF) | ((size >> 8) & 0x0000FF00) | ((size << 8) & 0x00FF0000) | ((size << 24) & 0xFF000000);
    return size;
}

Status get_data(ViewInfo *viewinfo)
{
    if(fread(&viewinfo->size_buffer, 4, 1, viewinfo->file) != 1)
    {
        printf("Error!: Failed to read the size.\n");
        return failure;
    }   
    viewinfo->size_buffer = big_to_little(viewinfo->size_buffer);

    fseek(viewinfo->file, 3, SEEK_CUR);

    if(fread(viewinfo->buffer, (viewinfo->size_buffer - 1), 1, viewinfo->file) != 1)
    {
        printf("Error!: Failed to read data\n");
        return failure;
    }
    return success;
}

//Convert the read data from big Endian to little Endian
//Skip 3bytes
//Read size number of bytes and Print 
Status do_viewing(ViewInfo *viewinfo)
{
    //Skip 10 bytes
    fseek(viewinfo->file, 10, SEEK_SET);
    printf("-------------------------------------------------------------------------------\n");
    printf("MP3 Tag Reader and Editor for ID3v2\n");
    printf("-------------------------------------------------------------------------------\n");
    //Read the tag
    for(int j = 0; j < 6;j++)
    {
        clear_buffer(viewinfo->buffer);
        if(fread(viewinfo->buffer, 4, 1, viewinfo->file) != 1)
        {
            printf("Error!: Failed to read the Tag\n");
            return failure;
        }
        for(int i = 0; i < 6; i++)
        {
            //Compare the tag -> if yes,read next 4 bytes, if no, stop
            if(strcasecmp(viewinfo->buffer, tag[i]) == 0)
            {
                if (strcmp(tag[i], "TPE1") == 0) 
                {
                    if(get_data(viewinfo) == failure)
                    {
                        return failure;
                    }
                    printf("%-20s: %s\n", "Artist",viewinfo->buffer);
                    break;
                }
                else if (strcmp(tag[i], "TIT2") == 0) 
                {
                    if(get_data(viewinfo) == failure)
                    {
                        return failure;
                    }
                    printf("%-20s: %s\n", "Title",viewinfo->buffer);
                    break;
                }
                else if (strcmp(tag[i], "TALB") == 0) 
                {
                    if(get_data(viewinfo) == failure)
                    {
                        return failure;
                    }
                    printf("%-20s: %s\n","Album", viewinfo->buffer);
                    break;
                }
                else if (strcmp(tag[i], "TYER") == 0) 
                {
                    if(get_data(viewinfo) == failure)
                    {
                        return failure;
                    }
                    printf("%-20s: %s\n", "Year", viewinfo->buffer);
                    break;
                }
                else if (strcmp(tag[i], "TCON") == 0) 
                {
                    if(get_data(viewinfo) == failure)
                    {   
                        return failure;
                    }
                    printf("%-20s: %s\n", "Content_type", viewinfo->buffer);
                    break;
                }
                else if (strcmp(tag[i], "COMM") == 0) 
                {
                    if(get_data(viewinfo) == failure)
                    {
                        return failure;
                    }
                    printf("%-20s: %s\n", "Comment", viewinfo->buffer);
                    break;
                }
            }   
        }
    }
    printf("-------------------------------------------------------------------------------\n");
}
