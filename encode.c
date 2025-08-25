#include <stdio.h>
#include "encode.h"
#include "types.h"
#include<string.h>
#include "common.h"
#include<stdlib.h>

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


Status read_and_validate_encode_args(int argc,char *argv[], EncodeInfo *encInfo)
{
    // checking for the argv[2] is having bmp extension 
    if(strstr(argv[2],".bmp")==NULL)
    {
        return e_failure;
    }
    encInfo->src_image_fname = argv[2];

    // checking weather the argv[3] having the extension or not and storing the name in the secret_fname variable

    char* extn = strrchr(argv[3],'.'); // checking from the last occurance of . and storing in the char pointer of extn
    if(extn == NULL && strcmp(extn,".txt") != 0 && strcmp(extn,".c") != 0 && strcmp(extn,".sh") != 0) // checking for the extension using strcmp returns int
    {
        return e_failure;
    }
    encInfo->secret_fname = argv[3];
    strcpy(encInfo->extn_secret_file,extn);   // storing the extension 
        
   // checking for the argv[4] is there or not if not there save default name or validate does it have bmp extension or not           
    if(argc<5)
    {
        encInfo->stego_image_fname = "stego.bmp" ;
    }
    else
    {
        // check the argument passed having extension as bmp or not
        if(strstr(argv[4],".bmp")==NULL) 
        {
            return e_failure;
        }
        else
        {
            encInfo->stego_image_fname = argv[4]; //copying the file name passed in the argv[4]
        }
    }
    return e_success;
    
}

/* do encoding */
Status do_encoding(EncodeInfo *encInfo)
{   
    // openiing the files
    if(open_files(encInfo) == e_failure)
    {
        printf("file open failure\n");
        return e_failure;    
    }
    printf("Opened files successfully.\n");

    // checking the image capacity
    if(check_capacity(encInfo) == e_failure)
    {
    printf("image capacity is less\n");
    return e_failure;
    }
    printf("Image has enough capacity.\n");

    // copying the bmp header 
    if (copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_failure)
    {
        printf("Failed to copy bmp header\n");
        return e_failure;
    }
    printf("BMP header copied successfully.\n");

    // encoding the magic string size
    if(encode_magic_string_size(MAGIC_STRING,encInfo) == e_failure)
    {
        printf("encoding the magic string size failed\n");
        return e_failure;
    }
    printf("Size of Magic string encoded successfully.\n");

    // encoding the magic string 
    if (encode_magic_string(MAGIC_STRING, encInfo) == e_failure)
    {
        printf("encoding the magic string failed\n");
        return e_failure;
    }
    printf("Magic string encoded successfully.\n");

    // encoding the secret file extension size
    if (encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo) == e_failure) 
    {
        printf("encoding the extension size failed\n");
        return e_failure;
    }
    printf("Extension size encoded successfully.\n");

    // encoding the secret file extention
    if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_failure)
    {
        printf("encoding the secret file extension failed\n");
        return e_failure;
    }
    printf("Secret file extension encoded successfully.\n");

    // encoding the secret file size 
    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure)
    {
        printf("encoding the secret file size failed\n");
        return e_failure;
    }
    printf("Secret file size encoded successfully.\n");
    
    // encoding the secret file data
    if (encode_secret_file_data(encInfo) == e_failure)
    {
        printf("encoding the secret file data\n");
        return e_failure;
    }
    printf("Secret file data encoded successfully.\n");

    // copying the ramaining data failed
    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        printf("copy remaining data failed\n");
        return e_failure;
    }
    printf("Remaining image data copied successfully.\n");

    // if no failures 
    printf("Encoding completed successfully.\n");
    return e_success;
}

/* checking the capacity */
Status check_capacity(EncodeInfo *encInfo)
{   
    uint required_bits = 0;
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);   //for checking the capacity or compare get the sizes of the image
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    printf("Image capacity = %u bits\n", encInfo->image_capacity);
    printf("Secret file size = %ld bytes\n", encInfo->size_secret_file);
    
    //header file
    required_bits += 54;
    // magic string length
    required_bits += strlen(MAGIC_STRING)*8;
    // extension size  
    required_bits += 32;
    // extension text
    required_bits += strlen(encInfo-> extn_secret_file)*8;
    // size of the secret file (4bytes * 8)
    required_bits += 32;
    // secret file data 
    required_bits += (encInfo->size_secret_file)*8;

    // checking for the condition
    if(encInfo -> image_capacity > required_bits)
        return e_success;
    else
        return e_failure;
}


/* get file size */
uint get_file_size(FILE *fptr)
{
    long file_size;

    //move the file pointer to the end of the file
    fseek(fptr,0,SEEK_END);
    // get the last position of the file pointer
    file_size=ftell(fptr);
    // rewind the file pointer to the starting 
    rewind(fptr);
    //return the size of the file
    return file_size;
}


/* copy the bmp header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char buff[54]; //creating the buffer of 54 bytes
    fseek(fptr_src_image,0,SEEK_SET);  // rewinding the file pointer
    fseek(fptr_dest_image,0,SEEK_SET); // rewinding the file pointer
    fread(buff,sizeof(buff),1,fptr_src_image); // reading the 54 bytes from the source image 
    fwrite(buff,sizeof(buff),1,fptr_dest_image); // copying 54 bytes to the stego image file
    return e_success;
}

/* encoding the magic string size */
Status encode_magic_string_size(char * magic_string,EncodeInfo *encInfo)
{
    char buffer[32];
    long size = strlen(magic_string);
    fread(buffer,sizeof(int),8,encInfo->fptr_src_image);
    if(encode_size_to_lsb(size,buffer) == e_failure)
    return e_failure;
    else
    fwrite(buffer,sizeof(int),8,encInfo->fptr_stego_image);
    return e_success;
}

/* Status encode_magic_string */

Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    if(encode_data_to_image(magic_string,strlen(magic_string),encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
        return e_success;
    else
        return e_failure;

}

/* encoding the data to image */
Status encode_data_to_image(char* data,int size,FILE *fptr_src_image,FILE *fptr_stego_image)
{
    char image_buffer[8]; // creating a buffer of string of 8 bytes
    for(int i=0;i<size;i++)  // loop will call the function of size types 
    {
        fread(image_buffer,sizeof(char),8,fptr_src_image);  //reading the 8 bytes from the src image and storing in the buffer

        if(encode_byte_to_lsb(data[i],image_buffer)==e_failure)  // calling the byte to lsb function to encode the data of 1 byte
            {
                return e_failure;
            }
            fwrite(image_buffer,sizeof(char),8,fptr_stego_image);   // writing the data to the output image after the encryption
    }
    return e_success;
}



/* encode byte to the lsb */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
            for(int i=0;i<8;i++) // running the loop for thr 8 times for the 1 byte of data
                {
                    char bit = (data>>(7-i))&1;  // getting bit by bit of the char of 1byte
                    image_buffer[i] = image_buffer[i] & ~1; // clearing the last bit of each byte  
                    image_buffer[i] = image_buffer[i] | bit; // adding the bit to the lsb of the each byte in the buffer
                }
                return e_success;
}

/* encoding the int(4bytes) data */
Status encode_size_to_lsb(int data, char *image_buffer)
{
    for(int i=0;i<32;i++) // running the loop for 32 times 
    {
        char bit = (data>>(31-i))&1;  // getting bit by bit of char of 1 byte
        image_buffer[i] = image_buffer[i]& ~1; // clearing the last bit of each byte  
        image_buffer[i] |= bit; // adding the bit to the lsb of the each byte in the buffer
    }
    return e_success;
}

/* encoding the secret file extension size */
Status encode_secret_file_extn_size(int data, EncodeInfo *encInfo)
{
    char src_buffer[32]; // creating a buffer of 32 bytes
    fread(src_buffer,32,1,encInfo->fptr_src_image); // read 32 bytes from src file
    if(encode_size_to_lsb(data,src_buffer) == e_failure)  // call int_lsb(int ,char*);
    {
        return e_failure;
    }
    fwrite(src_buffer,32,1,encInfo->fptr_stego_image); // write the 32 bytes encoding data to the stego.bmp 
    return e_success;
}

/* encoding the secret file extension */
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    // calling data_to_image(data,strlen(data),fptr_src,fptr_dest);
    if(encode_data_to_image(file_extn,strlen(file_extn),encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }
    return e_success;
}

/* encoding the secret file size data */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    //read 32 bytes data from the beautifull.bmp
    char image_buffer[32];
    fread(image_buffer,32,1,encInfo->fptr_src_image);
    ////call size_to_lsb(secret_file,image buffer)
    if(encode_size_to_lsb(file_size,image_buffer)==e_failure)
    {
        return e_failure;
    }
    //write 32 bytes of encoded data into stego.bmp
    fwrite(image_buffer,32,1,encInfo->fptr_stego_image);
    return e_success;
}

/* encoding the secret file data */
Status encode_secret_file_data(EncodeInfo *encInfo) 
{
    char* buffer = malloc(encInfo->size_secret_file); 
    if (buffer == NULL) 
    return e_failure;
   // read all the data from the secret file and store into one buffer
   fread(buffer,encInfo->size_secret_file,1,encInfo->fptr_secret);
    // data_to_image(buffer,(size)strlen(data buffer));
    if(encode_data_to_image(buffer,encInfo->size_secret_file,encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
    {
        free(buffer);
        return e_failure;
    }
    free(buffer);
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char buffer;
    int bytes;
    //read 1 byte of data from the source and
    while((bytes = fread(&buffer,1,1,fptr_src))==1) // until fread funtion returns 0;
    {
        //write the 1byte of data to dest
        if(fwrite(&buffer,1,1,fptr_dest) != 1)
        {
            return e_failure;
        }
    }
    
    // checking wheather is there any error while reading form the src
    if(ferror(fptr_src))
    {
        return e_failure;
    }
    return e_success;

}

