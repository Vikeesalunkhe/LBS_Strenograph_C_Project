#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

//validate input files
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    //beautifull.bmp
    if (strstr(argv[2], ".bmp") != NULL)      //strstr(substr, mainstr) : return 
    {
        encInfo->src_image_fname = argv[2];
    } 
    else
    {
        printf(".bmp  extention not found\n");
        return e_failure;
    }

    //secret.txt
    if (strstr(argv[3], ".txt") != NULL)
    {
        encInfo->secret_fname = argv[3];
    }
    else
    {
        printf(".txt Extention not found\n");
        return e_failure;
    }


    //.bmp default.bmp
    if (argv[4] != NULL)
    {
        //validate if it is .bmp file or not
        encInfo->stego_image_fname = argv[4];
    }
    else{
        printf("INFO : Set default.bmp for output img\n");
        encInfo->stego_image_fname = "default.bmp";
    }

    return e_success;

}



// //
// uint get_file_size(FILE *fptr)
// {
//     fseek(fptr, 0, SEEK_END);

//     return ftell(fptr);

// }


// // check file capacity 
// Status check_capacity(EncodeInfo *encInfo)
// {
//     encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
//     encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

//     if (encInfo->image_capacity > 16+32+32+encInfo->size_secret_file*8)
//     {
//         return e_success;
//     }
//     else{
//         return e_failure;
//     }
// }


// // Copy beautiful.bmp file header and past in default.bmp file 
// Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
// {
//     //read from beautiful.bmp
//     char str[54];
//     rewind(fptr_src_image);    //file ptr on 0 pos
//     fread(str, 54, 1, fptr_src_image);   //reading 54 bytes from 0 pos

//     //write to stego.bmp
//     fwrite(str, 54, 1, fptr_dest_image);   //write 54 byte on des file
//     return e_success;
// }

// Status encode_byte_to_lsb(char data, char *image_buffer)
// {
//     for (int i = 0; i < 8; i++)
//     {
//         image_buffer[i] = (image_buffer[i] & 0xFE) | ((data >> i) & 1);    //(image_buffer[i] & 0xFE)  : clear 0 pos bit every byte and set data bit pos wise
//     }
//     return e_success;
// }


// Status encode_data_to_image(char *data, int size, EncodeInfo *encInfo)   //"#*"
// {
//     for (int i = 0; i< size; i++)
//     {
//         fread(encInfo->image_data, 8, 1, encInfo->fptr_src_image);
//         encode_byte_to_lsb(data[i], encInfo->image_data);
//         fwrite(encInfo->image_data, 8, 1, encInfo->fptr_stego_image);
//     }

//     return e_success;
// }




// //encode magic string
// Status encode_magic_string(char *magic_string, EncodeInfo *encinfo)
// {
//     encode_data_to_image(magic_string, strlen(magic_string), encinfo);
//     return e_success;
// }

// Status encode_size_to_lsb(int size, EncodeInfo *encInfo)
// {
//     char str[32];
//     fread(str, 32, 1, encInfo->fptr_src_image);
//     for (int i = 0; i < 32; i++)
//     {
//         str[i] = (str[i] & 0xFE) ((size >> i) & i));
//     }
// }


// Status encode_secret_file_extn_size(int file_size, EncodeInfo *encInfo)
// {
//     encode_size_to_lsb(file_size, encInfo);
// }



// Status encode_secret_file_data(EncodeInfo *encInfo)
// {
//     char data[encInfo->size_secret_file];
//     rewind(encInfo->fptr_secret);
//     fgets(data, encInfo->size_secret_file, encInfo->fptr_secret);
//     encode_data_to_image(data, encInfo->size_secret_file, encInfo->fptr_secret);
//     return e_success;
// }

// Status copy_remaining_img_data(EncodeInfo *encInfo)
// {
//     int size = 54 + encInfo->image_data - ftell(encInfo->fptr_src_image);
//     char rem_data[size];
//     fread(rem_data, size, 1, encInfo->fptr_src_image);
//     fwrite(rem_data, size, 1, encInfo->fptr_stego_image);
//     return e_success;
// }



// Status do_encoding(EncodeInfo *encInfo)
// {
//     if (open_files(encInfo) == e_success)
//     {
//         printf("INFO : File are opened successfully\n");
//         if (check_capacity(encInfo))
//         {
//             printf("INFO : secret data can fit in given image file\n");
//             if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
//             {
//                 printf("INFO : Copied bmp header successfully\n");
//                 if (encode_magic_string(MAGIC_STRING, encInfo) == e_success)
//                 {
//                     printf("Encode Magic String Successfully\n");
//                     strcpy(encInfo->extn_secret_file, strstr(encInfo->secret_fname, '.'));
//                     printf("%s\n", encInfo->extn_secret_file);

//                     if (encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo) == e_success)
//                     {
//                         printf("Encoded secret file extn size successfully\n");
//                         if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
//                         {
//                             printf("Encode Secret file exte successfully\n");
//                             if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
//                             {
//                                 printf("Encode secret file size successfully\n");
//                                 if (encode_secret_file_data(encInfo) == e_success)
//                                 {
//                                     printf("Encode Secret file data successfullt\n");
//                                     if (copy_remaining_img_data(encInfo) == e_success)
//                                     {
//                                         printf("Copied remanning data successfully\n");
//                                     }
//                                     else
//                                     {
//                                         printf("Failed to copy Remanning data\n");
//                                         return e_failure;
//                                     }
//                                 }
//                                 else
//                                 {
//                                     printf("failed to secret file data\n");
//                                 }
//                             }
//                             else
//                             {
//                                 printf("failed to encode secret file size\n");
//                             }
//                         }
//                         else 
//                         {
//                             printf("failed to encode secret file exte\n");
//                         }

//                     }
//                     else 
//                     {
//                         printf("Failed to encode secret extn size\n");
//                         return e_failure;
//                     }
//                 }
//                 else
//                 {
//                     printf("Failed to Encode Magic String\n");
//                     return e_failure;
//                 }
//             }
//             else
//             {
//                 printf("ERROR : Failed bmp header successfully\n");
//                 return e_failure;
//             }
//         }

//     }
// }