#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "encode.h"
#include "types.h"
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


Status read_and_validate_encode_args(int argc,char *argv[], EncodeInfo *encInfo)  // Function to read and validate encoding arguments
{
    if(argc >= 4) // Check if there are enough command-line arguments
    {
        if(strstr(argv[2],".bmp") != NULL) // Check if the 3rd argument is a .bmp file
        {
            encInfo->src_image_fname = argv[2]; // Store the source image file name
            printf("Input file -> %s\n",encInfo->src_image_fname); // Print the input file name
            if(strchr(argv[3],'.') != NULL)  // Check if the 4th argument has a file extension
            {
                strcpy(encInfo->extn_secret_file,strchr(argv[3],'.')); // Store the secret file extension
                encInfo->secret_fname = argv[3]; // Store the secret file name
                printf("secret file extension -> %s\n",encInfo->extn_secret_file);  // Print the secret file extension
                printf("secret file name -> %s\n",encInfo->secret_fname); // Print the secret file name
                if(argc == 5) // If there is a 5th argument
                {
                    if(strstr(argv[4],".bmp") != NULL)   // Check if the 5th argument is a .bmp file
                    {
                        encInfo->stego_image_fname = argv[4]; // Store the stego image file name
                        printf("output file name -> %s\n",encInfo->stego_image_fname); // Print the output file name
                    }
                }
                else // If there is no 5th argument
                {
                    argv[4] = "output.bmp"; // Set default output file name
                    encInfo->stego_image_fname = argv[4]; // Store the default output file name
                    printf("output file name -> %s\n",encInfo->stego_image_fname); // Print the output file name
                }
            }
            else // If the 4th argument doesn't have an extension
            {
                return e_failure; // Return failure status
            }
        }
        else // If the 3rd argument is not a .bmp file
        {
            return e_failure; // Return failure status
        }
        return e_success; // Return success status
    }
    else // If there are not enough arguments
    {
        printf("Invalid number of arguments\n"); // Print error message
    }
}


Status do_encoding(EncodeInfo *encInfo) // Function to perform encoding
{
    printf("Started Encoding\n"); // Print start of encoding

    // Attempt to open the necessary files
    if(open_files(encInfo) == e_success)    
    {
        printf("File opened successfully\n"); // Success message for file opening
    }
    else
    {
        printf("Failed to open file\n"); // Error message for file opening failure
        return e_failure; // Return failure status
    }

    // Check if the image has enough capacity to encode the secret file
    if(check_capacity(encInfo) == e_success)
    {
        printf("Image Capacity check is successfull\n"); // Success message for capacity check
    }
    else
    {
        printf("Image Capacity check is failure\n"); // Error message for capacity check failure
        return e_failure; // Return failure status
    }

    // Copy the BMP header from source image to the stego image
    if(copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
    {
        printf("bmp header is copied successfully\n"); // Success message for header copy
    }
    else
    {
        printf("Copying bmp header is a failure\n"); // Error message for header copy failure
        return e_failure; // Return failure status
    }

    // Encode the magic string into the image
    if(encode_magic_string(MAGIC_STRING, encInfo) == e_success)
    {
        printf("Encoded magic string successfully\n"); // Success message for magic string encoding
    }
    else
    {
        printf("Encoding magic string is a failure\n"); // Error message for magic string encoding failure
        return e_failure; // Return failure status
    }

    // Encode the size of the secret file extension
    if (encode_secret_file_size(strlen(encInfo->extn_secret_file), encInfo) == e_success)
    {
        printf("Encoded secret file extension size successfully\n"); // Success message for extension size encoding
    }
    else
    {
        printf("Encoding secret file extension size is a failure\n"); // Error message for extension size encoding failure
        return e_failure; // Return failure status
    }

    // Encode the secret file extension itself
    if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
    {
        printf("Encoded secret file extension successfully\n"); // Success message for extension encoding
    }
    else
    {
        printf("Encoding secret file extension is a failure\n"); // Error message for extension encoding failure
        return e_failure; // Return failure status
    }

    // Encode the size of the secret file
    if(encode_secret_file_size(encInfo->size_secret_file,encInfo) == e_success)
    {
        printf("Encoded secret file size successfully\n"); // Success message for file size encoding
    }
    else
    {
        printf("Encoding secret file size is a failure\n"); // Error message for file size encoding failure
        return e_failure; // Return failure status
    }

    // Encode the actual data from the secret file into the image
    if(encode_secret_file_data(encInfo) == e_success)
    {
        printf("Encoded secret file data successfully\n"); // Success message for data encoding
    }
    else
    {
        printf("Encoding secret file data is a failure\n"); // Error message for data encoding failure
        return e_failure; // Return failure status
    }

    // Copy the remaining data from the source image to the stego image
    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("Copied the remaining bytes successfully\n"); // Success message for remaining data copy
    }
    else
    {
        printf("Copying remaining bytes is a failure\n"); // Error message for remaining data copy failure
        return e_failure; // Return failure status
    }

    print_size(encInfo); // Print the size of the encoded data

    fclose(encInfo->fptr_src_image); // Close the source image file
    fclose(encInfo->fptr_secret); // Close the secret file
    fclose(encInfo->fptr_stego_image); // Close the stego image file

    return e_success; // Return success status
}


Status open_files(EncodeInfo *encInfo) // Function to open necessary files
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r"); // Open the source image file
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen"); // Print error for file opening
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname); // Print specific error message

        return e_failure; // Return failure status
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r"); // Open the secret file
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen"); // Print error for file opening
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname); // Print specific error message

        return e_failure; // Return failure status
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w"); // Open the stego image file for writing
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen"); // Print error for file opening
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname); // Print specific error message

        return e_failure; // Return failure status
    }

    // No failure return e_success
    return e_success; // Return success status
}

// Check capacity of the image for encoding
Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image); // Get image capacity
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret); // Get secret file size
    if(encInfo->image_capacity > (54 + (2 + 4 + 4 + 4 + encInfo->size_secret_file) * 8)) // Check if there is enough capacity
    {
        return e_success; // Return success if enough capacity
    }
    else
    {
        return e_failure; // Return failure if not enough capacity
    }
}

uint get_file_size(FILE *fptr) // Function to get file size
{
    fseek(fptr , 0 , SEEK_END); // Move to the end of the file
    return ftell(fptr); // Return the file size
}

// Copy the header from the source image to the stego file
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char str[54]; // Buffer to hold the BMP header
    fseek(fptr_src_image , 0 , SEEK_SET); // Move to the start of the source image
    fread(str , 54 , 1 , fptr_src_image); // Read the BMP header
    fwrite(str , 54 , 1 , fptr_dest_image); // Write the BMP header to the stego image
    return e_success; // Return success status
}

// Encode magic string into the image
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    encode_data_to_image(magic_string, strlen(magic_string), encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo); // Encode the magic string
    return e_success; // Return success status
}

// Encode data into the image by manipulating the least significant bits
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image, EncodeInfo *encInfo)
{
    int i; // Loop counter
    for(i=0;i<size;i++) // Loop through each byte of data
    {
        fread(encInfo->image_data , 8, 1, fptr_src_image); // Read 8 bits from source image
        encode_byte_to_lsb(data[i] ,encInfo->image_data); // Encode the data byte into the least significant bits
        fwrite(encInfo->image_data , 8, 1, fptr_stego_image); // Write modified bits to the stego image
    }
    return e_success; // Return success status
}

// Encode a single byte into the least significant bits of the image buffer
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i=0;i<8;i++) // Loop through each bit of the byte
    {
        image_buffer[i]=((data & (1<<i))>>i) | (image_buffer[i] & 0xfe); // Set the LSB based on the data byte
    }
}

// Do encoding of the secret file size
Status encode_size(int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char str[32]; // Buffer to hold data
    fread(str , 32 , 1, fptr_src_image); // Read 32 bits from source image
    encode_size_to_lsb(str,size); // Encode the size into the buffer
    fwrite(str , 32 , 1, fptr_stego_image); // Write modified data to the stego image
    return e_success; // Return success status
}

// Encode size to least significant bits
Status encode_size_to_lsb(char *image_buffer, int size)
{
    for(int i=0;i<32;i++) // Loop through each bit of size
    {
        image_buffer[i]=((size & (1<<i))>>i) | (image_buffer[i] & 0xfe); // Set the LSB in the buffer
    }
}

// Encoding of secret file extension
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    encode_data_to_image(file_extn,strlen(file_extn),encInfo->fptr_src_image,encInfo->fptr_stego_image,encInfo); // Encode the file extension
    return e_success; // Return success status
}

// Encode secret file data size
Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)
{
    char str[32]; // Buffer to hold data
    fread(str , 32 , 1, encInfo->fptr_src_image); // Read 32 bits from source image
    encode_size_to_lsb(str,file_size); // Encode the file size into the buffer
    fwrite(str , 32 , 1,encInfo->fptr_stego_image); // Write modified data to the stego image
    return e_success; // Return success status
}

// Encode secret file data into the image
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char str[encInfo->size_secret_file]; // Buffer to hold secret file data
    fseek(encInfo->fptr_secret, 0, SEEK_SET); // Move to the start of the secret file
    fread(str, encInfo->size_secret_file, 1, encInfo->fptr_secret); // Read the secret file data
    encode_data_to_image(str,encInfo->size_secret_file,encInfo->fptr_src_image,encInfo->fptr_stego_image,encInfo); // Encode the secret file data
    return e_success; // Return success status
}

// Copy remaining bytes from the source to the stego image
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch; // Buffer to hold a single byte
    while(fread(&ch,1,1,fptr_src) > 0) // Read until end of the source file
    {
        fwrite(&ch,1,1,fptr_dest); // Write the byte to the stego image
    }
    return e_success; // Return success status
}

// Print sizes of the source and stego images
void print_size(EncodeInfo *encInfo)
{
    uint source_size = get_file_size(encInfo->fptr_src_image); // Get the size of the source image file
    uint stego_size = get_file_size(encInfo->fptr_stego_image); // Get the size of the stego image file
    printf("File size of source image: %u\n", source_size); // Print the size of the source image file
    printf("File size of encoded image: %u\n", stego_size); // Print the size of the encoded stego image file
}
