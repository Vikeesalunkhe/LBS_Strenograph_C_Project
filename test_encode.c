#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>




int main(int argc, char *argv[])
{   
    EncodeInfo E1;
    // 1: validation cla
    // 2 : check validation type
    int ret = check_operation_type(argv);

    // 3 : -e or -d selected
    if (ret == e_encode)
    {
        printf("Selected Encoding\n");
        if (read_and_validate_encode_args(argv, &E1) == e_success)
        {
            printf("INFO : Read and Validate Encode arg is Success\n");
            if (do_encoding(&E1) == e_success)
            {
                printf("Encoding is Success\n");
            }
            else
            {
                printf("failed to Encoding\n");
            }
        }
        else 
        {
            printf("ERROR : Read and Validation Encode arg is Failed\n");
        }

    }
    else if (ret == e_decode)
    {
        printf("Selected Decoding\n");
    }
    else 
    {
        printf("Invalid Operation\n");
        printf("usage:\n for Encoding : ./a.out -e beautiful.bmp secret.txt [stego.bmp]");
    }
    return 0;
}


//check operation mode like encoding or decoding depend on user command line input
OperationType check_operation_type(char *argv[])
{
    if ( strcmp(argv[1], "-e") == 0)    //strcmp(str1, str2) : compare two string if same return 0 
        return e_encode;
    else if (strcmp(argv[1], "-d") == 0)
        return e_decode;
    else 
        return e_unsupported;
    
}





