/*
Title: Image Steganography: Encoding and Decoding Secret Data within Image Files

Date: 	16-08-2024
Author: S. Ashwin

Input:	For option 'e'
		a) Source image file name
		b) Secret text file name
		c) Output image file name
		For option 'd'
		a) Stego image file name
		b) Output text file name

Output:	Data in secret file copied to Output text file
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include "decode.h"


int main(int argc,char *argv[]) // Main function with command-line arguments
{
    EncodeInfo encInfo; // Declare an EncodeInfo structure
    DecodeInfo decInfo; // Declare a DecodeInfo structure
    int ret = check_operation_type(argv); // Check operation type (encode or decode)
    if(ret == e_encode) // If encoding is selected
    {
        printf("Selected encoding\n"); // Print encoding selection
        if(argc >= 4) // Ensure sufficient arguments are provided
        {
            if(read_and_validate_encode_args(argc,argv,&encInfo) == e_success) // Validate encoding arguments
            {
                if(do_encoding(&encInfo) == e_success) // Perform encoding
                    printf("Encoded successfully\n"); // Print success message
            }
            else
            {
                printf("Encoded failure\n"); // Print failure message for encoding
            }
        }
    }
    else if(ret == e_decode) // If decoding is selected
    {
        printf("Selected decoding\n"); // Print decoding selection
        if(argc >= 3) // Ensure sufficient arguments are provided
        {
            if(read_and_validate_decode_args(argc,argv,&decInfo) == e_success) // Validate decoding arguments
            {
                printf("Read and validation is successfull\n"); // Print validation success
                if(do_decoding(&decInfo) == e_success) // Perform decoding
                {
                    printf("Decoding successful\n"); // Print success message for decoding
                }
                else
                {
                    printf("Decoding failure\n"); // Print failure message for decoding
                }
            }
            else
            {
                printf("Read and validation is failed\n"); // Print validation failure message
            }
        }
    }
    else // If an unsupported operation is selected
    {
        printf("selected unsupported\n"); // Print unsupported operation message
    }
}

OperationType check_operation_type(char *argv[]) { // Function to check operation type based on command-line argument
    if (strcmp(argv[1], "-e") == 0) // Check for encoding flag
        return e_encode; // Return encoding operation type
    else if (strcmp(argv[1], "-d") == 0) // Check for decoding flag
        return e_decode; // Return decoding operation type
    else
        return e_unsupported; // Return unsupported operation type
}
