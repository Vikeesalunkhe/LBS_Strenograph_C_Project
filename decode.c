#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "decode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/*
 * Read and validate decode arguments from argv
 * argv[2] = stego image (.bmp)
 * argv[3] = output file name (optional, defaults to "output.txt")
 */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // validate stego img .bmp
    if (argv[2] != NULL && strstr(argv[2], ".bmp") != NULL)
    {
        decInfo->stego_image_fname = argv[2];
    }
    else
    {
        printf("ERROR : Invalid or missing stego image. Expected a .bmp file.\n");
        return e_failure;
    }

    // output file name is optional
    if (argv[3] != NULL)
    {
        decInfo->output_fname = argv[3];
    }
    else
    {
        printf("INFO : No output filename provided. Using default: output.txt\n");
        decInfo->output_fname = "output.txt";
    }

    return e_success;
}

// open stego img for reading
Status open_decode_files(DecodeInfo *decInfo)
{
   
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open stego image: %s\n", decInfo->stego_image_fname);
        return e_failure;
    }

    return e_success;
}


char decode_byte_from_lsb(char *image_buffer)
{
    char data = 0;
    for (int i = 0; i < 8; i++)
    {
        data = data | ((image_buffer[i] & 1) << i);
    }
    return data;
}


Status decode_data_from_image(char *data, int size, DecodeInfo *decInfo)
{
    for (int i = 0; i < size; i++)
    {
        fread(decInfo->image_data, 8, 1, decInfo->fptr_stego_image);
        data[i] = decode_byte_from_lsb(decInfo->image_data);
    }
    return e_success;
}


Status decode_size_from_lsb(int *size, DecodeInfo *decInfo)
{
    char str[32];
    *size = 0;

    fread(str, 32, 1, decInfo->fptr_stego_image);
    for (int i = 0; i < 32; i++)
    {
        *size = *size | ((str[i] & 1) << i);
    }
    return e_success;
}


//validate the magic string
Status decode_magic_string(char *magic_string, DecodeInfo *decInfo)
{
    int len = strlen(magic_string);
    char decoded[len + 1];

    decode_data_from_image(decoded, len, decInfo);
    decoded[len] = '\0';

    if (strcmp(decoded, magic_string) == 0)
    {
        return e_success;
    }
    else
    {
        printf("ERROR : Magic string mismatch. Got: '%s', Expected: '%s'\n", decoded, magic_string);
        printf("ERROR : This image does not appear to contain hidden data.\n");
        return e_failure;
    }
}


// decode size in byte of the secret file ext string
Status decode_secret_file_extn_size(int *extn_size, DecodeInfo *decInfo)
{
    return decode_size_from_lsb(extn_size, decInfo);
}


//decode secret file ext string eg. .txt
Status decode_secret_file_extn(DecodeInfo *decInfo, int extn_size)
{
    decode_data_from_image(decInfo->extn_secret_file, extn_size, decInfo);
    decInfo->extn_secret_file[extn_size] = '\0';
    return e_success;
}


//decode 32 bit secret file size
Status decode_secret_file_size(long *file_size, DecodeInfo *decInfo)
{
    int size = 0;
    decode_size_from_lsb(&size, decInfo);
    *file_size = (long)size;
    return e_success;
}


Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char data[decInfo->size_secret_file + 1];

    decode_data_from_image(data, decInfo->size_secret_file, decInfo);
    data[decInfo->size_secret_file] = '\0';

    /* Write decoded data to output file */
    fwrite(data, decInfo->size_secret_file, 1, decInfo->fptr_output);
    return e_success;
}


Status do_decoding(DecodeInfo *decInfo)
{
    if (open_decode_files(decInfo) == e_success)
    {
        printf("INFO : Stego image opened successfully\n");

        /* Step 1: Skip the 54-byte BMP header (not encoded, just copied) */
        fseek(decInfo->fptr_stego_image, 54, SEEK_SET);
        printf("INFO : Skipped BMP header (54 bytes)\n");

        /* Step 2: Validate magic string */
        if (decode_magic_string(MAGIC_STRING, decInfo) == e_success)
        {
            printf("INFO : Magic string validated successfully\n");

            /* Step 3: Decode extension size */
            int extn_size = 0;
            if (decode_secret_file_extn_size(&extn_size, decInfo) == e_success)
            {
                printf("INFO : Decoded extension size = %d\n", extn_size);

                /* Step 4: Decode extension string */
                if (decode_secret_file_extn(decInfo, extn_size) == e_success)
                {
                    printf("INFO : Decoded file extension = %s\n", decInfo->extn_secret_file);

                    /* Step 5: Decode secret file size */
                    if (decode_secret_file_size(&decInfo->size_secret_file, decInfo) == e_success)
                    {
                        printf("INFO : Decoded secret file size = %ld bytes\n", decInfo->size_secret_file);

                        /* Open output file now that we know the extension */
                        decInfo->fptr_output = fopen(decInfo->output_fname, "w");
                        if (decInfo->fptr_output == NULL)
                        {
                            perror("fopen");
                            fprintf(stderr, "ERROR: Unable to open output file: %s\n", decInfo->output_fname);
                            return e_failure;
                        }

                        /* Step 6: Decode and write secret file data */
                        if (decode_secret_file_data(decInfo) == e_success)
                        {
                            printf("INFO : Secret data decoded and written to '%s' successfully\n", decInfo->output_fname);
                            fclose(decInfo->fptr_output);
                        }
                        else
                        {
                            printf("ERROR : Failed to decode secret file data\n");
                            fclose(decInfo->fptr_output);
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("ERROR : Failed to decode secret file size\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("ERROR : Failed to decode file extension\n");
                    return e_failure;
                }
            }
            else
            {
                printf("ERROR : Failed to decode extension size\n");
                return e_failure;
            }
        }
        else
        {
            printf("ERROR : Magic string validation failed\n");
            return e_failure;
        }

        fclose(decInfo->fptr_stego_image);
    }
    else
    {
        printf("ERROR : Failed to open stego image\n");
        return e_failure;
    }

    return e_success;
}
