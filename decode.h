#ifndef DECODE_H
#define DECODE_H

#include "types.h"

/*
 * Structure to store information required for
 * decoding hidden data from a stego image
 */
#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    /* Decoded File Info */
    char *output_fname;
    FILE *fptr_output;

    char extn_secret_file[MAX_FILE_SUFFIX];
    long size_secret_file;

    /* Buffers */
    char image_data[MAX_IMAGE_BUF_SIZE];

} DecodeInfo;


/* Decoding function prototypes */

/* Read and validate decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Open files for decoding */
Status open_decode_files(DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Check for magic string in stego image */
Status decode_magic_string(char *magic_string, DecodeInfo *decInfo);

/* Decode a single byte from 8 image bytes (LSB) */
char decode_byte_from_lsb(char *image_buffer);

/* Decode data of given size from stego image */
Status decode_data_from_image(char *data, int size, DecodeInfo *decInfo);

/* Decode a 32-bit size/int from stego image */
Status decode_size_from_lsb(int *size, DecodeInfo *decInfo);

/* Decode secret file extension size */
Status decode_secret_file_extn_size(int *extn_size, DecodeInfo *decInfo);

/* Decode secret file extension string */
Status decode_secret_file_extn(DecodeInfo *decInfo, int extn_size);

/* Decode secret file size */
Status decode_secret_file_size(long *file_size, DecodeInfo *decInfo);

/* Decode and write secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo);

#endif
