#ifndef TYPE_H
#define TYPE_H

static char tag[6][5]= {"TIT2", "TPE1", "TALB", "TYER", "TCON", "COMM"}; 

typedef enum
{
    view,
    edit,
    help,
    unsupported,
    invalid
}OperationType;

typedef enum
{
    success,
    failure
}Status;

#endif