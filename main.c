/*
 * Project Title: LSB-Based Steganography for BMP Images
 * 
 * Description:
 * This project implements a simple steganography technique using the Least Significant Bit (LSB) method
 * to hide and extract secret data (such as a file) inside a BMP image. The encoder embeds the contents
 * of a secret file into the pixel data of a BMP image without visibly altering the image. The decoder 
 * reverses this process to extract the hidden file.
 *
 * Features:
 * - Encodes a secret file into a 24-bit BMP image using LSB technique.
 * - Decodes and reconstructs the hidden file from the stego-image.
 * - Supports decoding of:
 *     - Magic string size
 *     - Magic string
 *     - Secret file extension size
 *     - Secret file extension
 *     - Secret file size
 *     - Secret file data
 * - Handles file operations and error checking.
 *
 * Files:
 * - encoder.c: Contains the encoding logic.
 * - decoder.c: Contains the decoding logic.
 * - main.c   : Entry point for invoking encoder or decoder.
 * - stego.bmp: Output BMP image with embedded data.
 *
 * Usage:
 * Encoding:
 *   ./encode <source BMP> <secret file> <output BMP>
 * 
 * Decoding:
 *   ./decode <stego BMP> <output folder>
 *
 * Author: K.Bhavani Sankar
 * 
 */

#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include<string.h>

/* Check operation type */
OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1],"-e")==0)
        // printf("selected the encoding\n");
        return e_encode; // returns 0
    else if(strcmp(argv[1],"-d")==0)
        // printf("selected the decoding\n");
        return e_decode; // returns 1;
    else
        // printf("Invalid input\n");
        return e_unsupported; //returns 2
    
}

int main(int argc,char *argv[])
{
    switch(check_operation_type(argv))
    {
        case e_encode :
    {
        if(argc<4)
        {
        printf("Encoding: ./a.out -e <.bmp file> <.txt file> (output file)");
        return e_failure;
        }
        printf("selected encoding\n");
        EncodeInfo encode;
        if(read_and_validate_encode_args(argc,argv,&encode) == e_success)
        {
            /* perform the encoding */
            if(do_encoding(&encode)==e_failure)
            {
                printf("Encoding failed\n");
                return e_failure;
            }
            fclose(encode.fptr_src_image);
            fclose(encode.fptr_secret);
            fclose(encode.fptr_stego_image);
          
            return e_success;
        }
        else
        {
            printf("read and validate failed\n");                          
            return 0;
        }
        break;
    }
        
        case e_decode : 
   {
        if(argc<3)
        {
            printf("Decoding: /lsb_steg -d <.bmp file> (output file)");
            return e_failure;
        }
        printf("Selected decoding\n");
        DecodeInfo decode;
        if(read_and_validate_decode_args(argv,&decode) == e_success)
        {
            /* perform decoding */
            if(do_decoding(&decode)==e_failure)
            {
                printf("Decoding failed\n");
                return e_failure;
            }
            fclose(decode.fptr_stego_image);
            fclose(decode.fptr_output);
            
            printf("Decoding success\n");
            return e_success;
        }
        else
        {
            printf("read and validate failed\n");
            return e_failure;
        }
        break;
    }
        default :
        printf("Invalid input\n");
        printf("Encoding: ./a.out -e <.bmp file> <.txt file> (output file)\n");
        printf("Decoding: /lsb_steg -d <.bmp file> (output file)");
        return e_failure;
    }
}



