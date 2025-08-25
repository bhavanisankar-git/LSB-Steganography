#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decode.h"
#include "common.h"

/* read and validate */
Status read_and_validate_decode_args(char *argv[],DecodeInfo *decInfo)
{
    // save the stego image file name
    decInfo->stego_image_fname = argv[2];  

    if(argv[3] != NULL) // checking wheather the argument passed for the out file
    {
        char *dot = strrchr(argv[3],'.'); // find the last occurance of .(address)
        int len;
        
        if(dot!=NULL) //if we have extension
        {
            len = dot - argv[3]; // length till dot  
        }
        else     // if textension not found 
        {
            len = strlen(argv[3]);
        }
        decInfo->output_fname = (char *)malloc(len + 1);
        strncpy(decInfo->output_fname, argv[3], len);  // copying the file name without the extension till length
        decInfo->output_fname[len] = '\0'; // Null-terminate
    }
    else // if no output file name provided 
    {
        decInfo->output_fname = malloc(strlen("output_file") + 1);
        if (decInfo->output_fname == NULL)
        {
            return e_failure;
        }

        strcpy(decInfo->output_fname, "output_file");      // // just save the file name later we have to add the extension from the encoding data
    }  
    return e_success;
}

/* open files */
Status decode_open_files(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image  = fopen(decInfo->stego_image_fname,"r");
    if((decInfo->fptr_stego_image) == NULL)
    {
        printf("opening the stego image file failed\n");
        return e_failure;
    }

    // decInfo->fptr_output = fopen(decInfo->output_fname,"w");
    // if((decInfo->fptr_output) == NULL)
    // {
    //     printf("opening the output file failed\n");
    //     return e_failure;
    // }
    return e_success;
}

/* skip bmp header */
Status skip_bmp_header(FILE *fptr_stego_image)
{
    fseek(fptr_stego_image,54,SEEK_SET);  // moving the file pointer to 54 bytes as to skip the header file
    return e_success;
}

/* decode lsb to byte */
char decode_lsb_to_byte(char *image_buffer)
{
    char byte = 0;  // taking a character to store the decoded value
    for(int i=0;i<8;i++) // running the loop for char size(bits)
    {
        byte = byte << 1;  // making space in the lsb 
        byte = byte | (image_buffer[i] & 1); // appending the character in the lasb of the byte from image buffer 
    }
    return byte; // return decoded char
}

/* Decode a size into lsb of image data array */
int decode_lsb_to_int(char *image_buffer)
{
    int value = 0; // take buffer if int size
    for(int i=0;i<32;i++) 
    {
        value = value<<1;
        value |= (image_buffer[i]&1);
    }
    return value; // return integer value
}

/* Decode magic string size */
Status decode_magic_string_size(DecodeInfo *decInfo)
{
    // sizeof reads a int value of 32 bytes
    char buffer[32];
    fread(buffer,32,1,decInfo->fptr_stego_image);
    decInfo->magic_string_size = decode_lsb_to_int(buffer);
    printf("magic string size----->%d\n",decInfo->magic_string_size);
    return e_success;
}

/* Decode magic string */
Status decode_magic_string(DecodeInfo *decInfo)
{   
    char magic[5];
    printf("enter magic string\n");
    scanf(" %[^\n]",magic);
   for (int i = 0; i < decInfo->magic_string_size ; i++)
    {
        char buffer[8];
        fread(buffer, 1, 8, decInfo->fptr_stego_image);
        char decoded_char = decode_lsb_to_byte(buffer);
        if (decoded_char != magic[i])
        {
            printf("Magic string mismatch\n");
            return e_failure;
        }
    }
    return e_success;
}

/* decode the secret file extension size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char buffer[32]; 
    fread(buffer,1,32,decInfo->fptr_stego_image); // reading 32 bytes as the size is int
    decInfo->extn_size = decode_lsb_to_int(buffer); // storing the size of the extension
    return e_success;
}

/* Decode the secret file extension */
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char buffer[8]; 
    for(int i=0;i<decInfo->extn_size;i++) // running the loop for extn size times 
    {
        fread(buffer,1,8,decInfo->fptr_stego_image); // reading the 8 bytes to decode 1 byte or char of extn
        decInfo->extn_secret_file[i] = decode_lsb_to_byte(buffer); // decoding the 8 bytes and storing in the string
    }
    decInfo->extn_secret_file[decInfo->extn_size] = '\0';
    // Allocate new memory for output filename + extension
    int new_len = strlen(decInfo->output_fname) + strlen(decInfo->extn_secret_file) + 1;
    decInfo->output_fname = realloc(decInfo->output_fname, new_len);
    if (decInfo->output_fname == NULL)
    {
        printf("Memory reallocation failed\n");
        return e_failure;
    }
    // append extension
    strcat(decInfo->output_fname,decInfo->extn_secret_file);
    return e_success;
}

/* decode the secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer,1,32,decInfo->fptr_stego_image); // reading 32 bytes as it is a integer
    decInfo->size_secret_file = decode_lsb_to_int(buffer); // storing the size of the secret file size by decoding in the size
    return e_success;
}

/* decode the secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char buffer[8];
    for(int i=0;i<decInfo->size_secret_file;i++) // run loop for secret file size times
    {
        fread(buffer,sizeof(char),8,decInfo->fptr_stego_image); // reading 8 bytes to buffer from stego image
        
        char decode_byte = decode_lsb_to_byte(buffer); // decoding 8 bytes and store in buffer 
        fwrite(&decode_byte,sizeof(char),1,decInfo->fptr_output); // writing 1 byte in the output file
    }
    return e_success;
}

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo)
{
    if(decode_open_files(decInfo)==e_failure)
    return e_failure;
    printf("Opened files successfully.\n");

    if(skip_bmp_header(decInfo->fptr_stego_image) == e_failure)
    return e_failure;
    printf("Skipped bmp header successfully.\n");

    if(decode_magic_string_size(decInfo) == e_failure)
    return e_failure;
    printf("Magic string size decoded successfully.\n");

    if(decode_magic_string(decInfo) == e_failure)
    return e_failure;
    printf("Magic string decoded successfully.\n");

    if(decode_secret_file_extn_size(decInfo) == e_failure)
    return e_failure;
    printf("Secret file extension size decoded successfully.\n");

    if(decode_secret_file_extn(decInfo) == e_failure)
    return e_failure;
    printf("Secret file extension decoded successfully.\n");
    
    // opening the output file after appending the extension

    decInfo->fptr_output = fopen(decInfo->output_fname,"w");
    if((decInfo->fptr_output) == NULL)
    {
        printf("opening the output file failed\n");
        return e_failure;
    }
    printf("Output file opened successfully.\n");

    if(decode_secret_file_size(decInfo) == e_failure)
    return e_failure;
    printf("Secret file size decoded successfully.\n");

    if(decode_secret_file_data(decInfo) == e_failure)
    return e_failure;
    printf("Secret file data decoded successfully.\n");

    // printf("decoding success\n");
    return e_success;
}



