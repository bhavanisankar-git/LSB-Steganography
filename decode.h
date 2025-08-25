#include<stdio.h>
#ifndef DECODE_H
#define DECODE_H

#include "types.h"  // contains user defined types

typedef struct _decodeInfo
{
    /* stego image info */
    char *stego_image_fname;       // Name of the stego image file
    FILE *fptr_stego_image;        // File pointer to the stego image
    char image_data[8];            // Buffer to hold 8 bytes of image data

    int magic_string_size;
    char *output_fname;            // Name of the output (decoded) file
    FILE *fptr_output;             // File pointer to the output file
    int extn_size;                 // Size of the file extension 
    char extn_secret_file[4];      // Extension of the secret file 
    long size_secret_file;         // Size of the decoded secret file
}DecodeInfo;

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[],DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* get file pointers for i/p and o/p files */
Status decode_open_files(DecodeInfo *decInfo);

/* skip bmp header */
Status skip_bmp_header(FILE *fptr_stego_image);

/* Decode magic string size */
Status decode_magic_string_size(DecodeInfo *decInfo);

/* Decode magic string */
Status decode_magic_string(DecodeInfo *decInfo);

/* decode the secret file extension size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode the secret file extension */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* decode the secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* decode the secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo);

/* decode byte into LSB of image data array */
char decode_lsb_to_byte(char *image_buffer);

/* Decode a size into lsb of image data array */
int decode_lsb_to_int(char *image_buffer);

#endif