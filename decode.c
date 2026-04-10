#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "common.h"
#include "types.h"

/* Read & validate decoding arguments */


Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo){
        if(argv[2] != NULL){

                if(strcmp(strstr(argv[2],  "."), ".bmp") == 0){
                        decInfo -> src_image_fname = argv[2];
                }
                else{
                        printf("Please pass argument bmp file\n");
                }
        }
        else{
                printf("Please pass argument bmp file\n");
                return e_failure;
        }

        if(argv[3] == NULL)
	{

                strcpy(decInfo ->dest_image_fname, "output.txt");
        }
        else
	{
                strcpy(decInfo -> dest_image_fname, argv[3]);
        }
        return e_success;
}

/* Open required files */
Status open_decoding_files(DecodeInfo *decInfo)
{
    decInfo->fptr_src_image = fopen(decInfo->src_image_fname, "rb");
    if (!decInfo->fptr_src_image)
        return e_failure;

    decInfo->dest_image_fpointer = fopen(decInfo->dest_image_fname, "w");
    if (!decInfo->dest_image_fpointer)
        return e_failure;

    return e_success;
}

/* Decode one byte from 8 image bytes */
char decode_byte_from_lsb(char data, char *image_buffer)
{
    data = 0;
    for (int i = 0; i < 8; i++)
    {
        data = data << 1;
        data |= (image_buffer[i] & 1);
    }
    return data;
}

/* Decode 32-bit integer from 32 image bytes */
uint decode_size_from_lsb(char *buffer)
{
    uint size = 0;

    for (int i = 0; i < 32; i++)
    {
        size = size << 1;
        size |= (buffer[i] & 1);
    }
    return size;
}

/* Decode magic string */
Status decode_magic_string(DecodeInfo *decInfo)
{
    char magic[3] = {0};
    char buffer[8];

    const char *expected = "#*";  // whatever you encoded

    for (int i = 0; i < 2; i++)
    {
        fread(buffer, 1, 8, decInfo->fptr_src_image);
        magic[i] = decode_byte_from_lsb(0, buffer);
    }

    printf("magic string  is %s \n: ", magic);

    if (strcmp(magic,expected) != 0)
        return e_failure;

    return e_success;
}


/* Decode secret file extension size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char buffer[32];

    fread(buffer, 1, 32, decInfo->fptr_src_image);

    decInfo->extn_size = decode_size_from_lsb(buffer);

    if (decInfo->extn_size == 0 || decInfo->extn_size > 10)
        return e_failure;

    return e_success;
}


/* Decode extension characters */
Status decode_secret_file_extn(int size, DecodeInfo *decInfo)
{
    char buffer[8];

    for (int i = 0; i < size; i++)
    {
        fread(buffer, 1, 8, decInfo->fptr_src_image);
        decInfo->extn[i] = decode_byte_from_lsb(0, buffer);
    }

    decInfo->extn[size] = '\0';  // Null terminate

    return e_success;
}


/* Decode secret file size */
Status decode_secret_file_data_size(DecodeInfo *decInfo)
{
    char buffer[32];

    fread(buffer, 1, 32, decInfo->fptr_src_image);

    decInfo->size_src_file = decode_size_from_lsb(buffer);

    if (decInfo->size_src_file == 0)
        return e_failure;

    return e_success;
}

/* Decode actual file data */
Status decode_secret_file_data(int size, DecodeInfo *decInfo)
{
    char buffer[8];

    for (int i = 0; i < size; i++)
    {
        fread(buffer, 1, 8, decInfo->fptr_src_image);
        char ch = decode_byte_from_lsb(0, buffer);
        fputc(ch, decInfo->dest_image_fpointer);
    }

    return e_success;
}

/* Final decoding operation */
Status do_decoding(DecodeInfo *decInfo)
{
    //printf("Selected for decoding.\n");

    /* Step 1: Open files */
    printf("Opening required files... ");
    if (open_decoding_files(decInfo) == e_success)
        printf("DECODED\n");
    else
    {
        printf("Failed\n");
        return e_failure;
    }

    /* Step 2: Skip BMP header */
    fseek(decInfo->fptr_src_image, 54, SEEK_SET);

    /* Step 3: Decode magic string */
    printf("Decoding magic string... ");
    if (decode_magic_string(decInfo) == e_success)
        printf("DECODED \n");
    else
    {
        printf("Failed\n");
        return e_failure;
    }

    /* Step 4: Decode extension size */
    printf("Decoding secret file extension size... ");
    if (decode_secret_file_extn_size(decInfo) == e_success)
        printf("DECODED\n");
    else
    {
        printf("Failed\n");
        return e_failure;
    }

    /* Step 5: Decode extension */
    printf("Decoding secret file extension... ");
    if (decode_secret_file_extn(decInfo->extn_size, decInfo) == e_success)
    {
        printf("DECODED\n");

    }
    else
    {
        printf("Failed\n");
        return e_failure;
    }

    /* Step 6: Decode secret file size */
    //printf("Decoding secret file size... ");
    if (decode_secret_file_data_size(decInfo) == e_success)
        printf("DECODED\n");
    else
    {
        printf("Failed\n");
        return e_failure;
    }

    /* Step 7: Decode actual data */
    printf("Decoding secret file data... ");
    if (decode_secret_file_data(decInfo->size_src_file, decInfo) == e_success)
        printf("DECODED\n");
    else
    {
        printf("Failed\n");
        return e_failure;
    }

    printf("Decoding completed successfully\n");
    return e_success;
}