/*
Name: Dheer Rudani
Date: 20/11/2024
Discription: MP3 Tag reader
*/

#include <stdio.h>
#include <string.h>
#include "view.h"
#include "edit.h"
#include "common.h"

int main(int argc, char *argv[])
{
    //Step1: Read the CLA
    if(argc < 2)
    {
        printf("-------------------------------------------------------------------------------\n");
        printf("INVALID NUMBER OF ARGUMENTS!\n");
        printf("INFO: To view -> ./a.out -v mp3filename\n");
        printf("INFO: To edit -> ./a.out -e -t\\-a\\-A\\-y\\-c\\-C mp3filename new_content\n");
        printf("INFO: To get help -> ./a.out -h\n");
        printf("-------------------------------------------------------------------------------\n");
        return failure;
    }

    //Step2: Check operation type
    OperationType op = check_operation_type(argv[1],argc);

    //Step3: Check the op type is view
    //Step4: If yes -> start viewing, else Goto Step5
    if(op == view)
    {
        ViewInfo viewinfo;
        if(read_and_validate_view(argv, &viewinfo) == failure)
        {
            printf("Error!: Failed to read and validate the mp3 file\n");
            return failure;
        }
        printf("-----------------------------SELECTED VIEW DETAILS-----------------------------\n\n");
        if(do_viewing(&viewinfo) == failure)
        {
            printf("Error!: Failed to view the data.\n");
            return failure;
        }
        printf("\n--------------------------DETAILS VIEWED SUCCESSFULLY--------------------------\n");
    }

    //Step6: Check the op type is edit
    //Step7: If yes -> start editing, else Goto Step8
    else if(op == edit)
    {
        //Creating the structure variable
        EditInfo editinfo;
        if(read_and_validate_edit(argv, &editinfo) == failure)
        {
            printf("Error!: Failed to open the file.\n");
            return failure;
        }
        printf("-----------------------------SELECTED EDIT DETAILS-----------------------------\n\n");
        if(do_editing(argv, &editinfo) == failure)
        {
            printf("Error!: Failed to edit the file.\n");
            return failure;
        }
        printf("\n-----------------------EDITED DETAILS VIEWED SUCCESSFULLY-----------------------\n");
        if(copy_to_original(argv) == failure)
        {
            printf("Error!: Failed to copy the data to original\n");
            return failure;
        }
    }

    //Step8: Check the op type is help
    //Step9: If yes -> display help, else Goto Step10
    else if(op == help)
    {
        printf("-------------------------------------------------------------------------------\n");
        printf("INFO: To view -> ./a.out -v mp3filename\n");
        printf("INFO: To edit -> ./a.out -e -t\\-a\\-A\\-y\\-c\\-C mp3filename new_content\n");
        printf("-t\tModifies a Title tag\n");
        printf("-a\tModifies a Artist tag\n");
        printf("-A\tModifies a Album tag\n");
        printf("-y\tModifies a Year tag\n");
        printf("-c\tModifies a Content_Type tag\n");
        printf("-C\tModifies a Comment tag\n");
        printf("-------------------------------------------------------------------------------\n");
    }

    else if(op == unsupported)
    {
        return failure;
    }

    //Step10: Print Error! message and stop the process
    else
    {
        printf("-------------------------------------------------------------------------------\n");
        printf("INVALID ARGUMENT INPUT...\nINFO: To get help type -> ./a.out -h\n");
        printf("-------------------------------------------------------------------------------\n");
        return failure;
    }

    return success;
}

OperationType check_operation_type(char *op, int size)
{
    //Step1: Compare size with -v
    //Step2: If yes -> return view, else Goto Step3
    if(!strcmp(op,"-v"))
    {
        if(size < 3)
        {
            printf("-------------------------------------------------------------------------------\n");
            printf("INFO: To view - pass minimum 3 arguments like ./a.out -v mp3filename new_content\n");
            printf("-------------------------------------------------------------------------------\n");
            return unsupported;
        }
        return view;
    }

    //Step3: Compare size with -e
    //Step4: If yes -> return edit, else Goto Step5
    else if(!strcmp(op,"-e"))
    {
        if(size < 5)
        {
            printf("-------------------------------------------------------------------------------\n");
            printf("INFO: To view - pass minimum 5 arguments like ./a.out -e -t\\-a\\-A\\-m\\-y\\-c\\-C mp3filename new_content\n");
            printf("-------------------------------------------------------------------------------\n");
            return unsupported;
        }
        return edit;
    }

    //Step5: Compare size with -h
    //Step6: If yes -> return help, else Goto Step7
    else if(!strcmp(op,"-h"))
    {
        return help;
    }

    //Step8: return unsupported
    else
    {
        return invalid;
    }
}