#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include <stdint.h>
#include<string.h>
#include "common.h"
#include "types.h"

typedef unsigned int uint;

/* Structure for Decoding Info */
typedef struct _DecodeInfo
{
    /* Source Image info */
    char *src_image_fname;
    FILE *fptr_src_image;

    /* Decoded file info */
    char dest_image_fname[256];
    FILE *dest_image_fpointer;

    /* Extension & file sizes */
    uint extn_size;
    uint size_src_file;

    char extn[20];

} DecodeInfo;


/* Function Prototypes */

/* Read & validate arguments */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Open required files */
Status open_decoding_files(DecodeInfo *decInfo);

/* Decode magic string */
Status decode_magic_string(DecodeInfo *decInfo);

/* Decode 1 byte from 8 bytes (LSB) */
char decode_byte_from_lsb(char data, char *image_buffer);

/* Decode extension size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode the extension */
Status decode_secret_file_extn(int size, DecodeInfo *decInfo);

/* Decode secret file data size */
Status decode_secret_file_data_size(DecodeInfo *decInfo);

/* Decode actual data */
Status decode_secret_file_data(int size, DecodeInfo *decInfo);

/* Final decoding operation */
Status do_decoding(DecodeInfo *decInfo);

/* Decode 32-bit size from LSB */
uint decode_size_from_lsb(char *buffer);

#endif