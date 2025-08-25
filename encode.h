#ifndef ENCODE_H
#define ENCODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8) 
#define MAX_FILE_SUFFIX 5   // the max extension size will be 4 bytes

typedef struct _EncodeInfo
{
    /* Source Image info */
    char *src_image_fname; // to store the name of the image(file)
    FILE *fptr_src_image;  // file pointer for storing the fopen address
    uint image_capacity;   // to store the size of the image
    uint bits_per_pixel;
    char image_data[MAX_IMAGE_BUF_SIZE];  // 8 bytes of data in array

    /* Secret File Info */
    char *secret_fname;  // to store the name of the secret file
    FILE *fptr_secret;  // to store the file pointer address
    char extn_secret_file[MAX_FILE_SUFFIX];  //to store the extension
    char secret_data[MAX_SECRET_BUF_SIZE]; // to store the secret data
    long size_secret_file; // to store the size secret file 

    /* Stego Image Info */
    char *stego_image_fname;  // to store the image name 
    FILE *fptr_stego_image;  // to store the file pointer of the image

} EncodeInfo;


/* Encoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(int argc,char *argv[], EncodeInfo *encInfo);                

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo);

/* Get File pointers for i/p and o/p files */
Status open_files(EncodeInfo *encInfo);

/* check capacity */
Status check_capacity(EncodeInfo *encInfo);

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image);

/* Get file size */
uint get_file_size(FILE *fptr);

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

/* stire magic string size */
Status encode_magic_string_size(char *magic_string,EncodeInfo *encInfo);

/* Store Magic String */
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo);

/* Encode secret file extension szie */
Status encode_secret_file_extn_size(int , EncodeInfo *encoInfo);

/* Encode secret file extenstion */
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo);

/* Encode secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo);

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo);

/* Encode function, which does the real encoding */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image);

/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(char data, char *image_buffer);

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);

/*Encode a size(int) to lsb */
Status encode_size_to_lsb(int data, char *image_buffer);


#endif
