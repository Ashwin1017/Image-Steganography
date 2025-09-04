#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "decode.h"
#include "types.h"
#include "common.h"


Status read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo) // Function to read and validate decoding arguments
{
    if (argc >= 3) // Check if enough arguments are provided
    {
        if (strstr(argv[2], ".bmp") != NULL) // Check if the second argument is a .bmp file
        {
            decInfo->stego_bmp = argv[2]; // Store the .bmp filename in the structure
            printf("Decoded file -> %s\n", decInfo->stego_bmp); // Print the decoded file name
            if (argc == 4) // Check if there is a fourth argument
            {
                if (strchr(argv[3], '.') != NULL) // Check if the third argument has a period
                {
                    decInfo->output_file = argv[3]; // Store the output file name
                    decInfo->output_file = strcpy(decInfo->output_file, strtok(argv[3], ".")); // Remove extension from output file name
                    printf("secret file name -> %s\n", decInfo->output_file); // Print the secret file name
                }
                else // If no period in third argument
                {
                    decInfo->output_file = argv[3]; // Store the output file name
                    strcpy(decInfo->output_file, argv[3]); // Copy the output file name
                    printf("Secret file name -> %s\n", decInfo->output_file); // Print the secret file name
                }
            }
            else // If no fourth argument is provided
            {
                argv[3] = "secret_output"; // Set default output file name
                decInfo->output_file = argv[3]; // Store the default output file name
                printf("Secret file name -> %s\n", decInfo->output_file); // Print the secret file name
            }
        }
        else // If the second argument is not a .bmp file
        {
            return e_failure; // Return failure status
        }
    }
    else // If not enough arguments are provided
    {
        printf("Invalid number of arguments\n"); // Print error message for invalid arguments
    }

    return e_success; // Return success status
}


// check open file
Status open_files_decode(DecodeInfo *decInfo) // Function to open the stego file for decoding
{
    decInfo->fptr_stego = fopen(decInfo->stego_bmp, "r"); // Open the stego BMP file for reading
    // Do Error handling
    if (decInfo->fptr_stego == NULL) // Check if the file was opened successfully
    {
        perror("fopen"); // Print error message if file opening fails
        fprintf(stderr, "ERROR: Unable to open stego file %s\n", decInfo->stego_bmp); // Print error details

        return e_failure; // Return failure status
    }
    return e_success; // Return success status
}

Status decode_byte_to_lsb(char *image_buffer) // Function to decode a byte from the least significant bits
{
    char bit=0; // Variable to store the decoded bit
    for(int i=0;i<8;i++) // Loop through each bit
    {
        bit = ((image_buffer[i] & 1)<<i) | bit; // Extract the least significant bit and combine
    }
    return bit; // Return the decoded byte
}

//decode magic string
Status decode_magic_string(DecodeInfo *decInfo) // Function to decode the magic string
{
    char data[8]; // Buffer to hold data read from the stego image
    char magic[3]; // Buffer to hold the decoded magic string
    fseek(decInfo->fptr_stego,54,SEEK_SET); // Seek to the start of the encoded data
    for(int i=0;i<2;i++) // Read the magic string
    {
        fread(data, 8, 1,decInfo->fptr_stego); // Read 8 bits from the stego image
        magic[i]=decode_byte_to_lsb(data); // Decode the byte to LSB
    }
    magic[2]='\0'; // Null-terminate the magic string
    if(strcmp(magic,"#*") == 0) // Check if the decoded magic string matches
    {
        printf("Decode Magic string is successfull\n"); // Print success message
    }
    else // If magic string does not match
    {
        printf("Magic string didn't match\n"); // Print failure message
        return e_failure; // Return failure status
    }
    return e_success; // Return success status
}

//do byte to size operation by taking 32 bits
Status decode_byte_to_size(char *image_buffer) // Function to decode size information from 32 bits
{
    char bit=0; // Variable to store the decoded size
    for(int i=0;i<32;i++) // Loop through each bit
    {
        bit=((image_buffer[i] & 1) << i) | bit; // Extract the least significant bit and combine
    }
    return bit; // Return the decoded size
}

//decode secret file size
Status decode_secret_file_size(DecodeInfo *decInfo) // Function to decode the size of the secret file
{
    char data[32]; // Buffer to hold data read from the stego image
    fread(data,32,1,decInfo->fptr_stego); // Read 32 bits from the stego image
    decInfo->extn_size = decode_byte_to_size(data); // Decode the size

    return e_success; // Return success status
}

//decode secret file extension
Status decode_secret_file_extn(long size, DecodeInfo *decInfo) // Function to decode the extension of the secret file
{
    char data[8]; // Buffer to hold data read from the stego image
    for(int i=0;i<size;i++) // Loop through the size of the extension
    {
        fread(data, 8, 1,decInfo->fptr_stego); // Read 8 bits from the stego image
        decInfo->extn[i]=decode_byte_to_lsb(data); // Decode and store the extension byte
    }
    decInfo->extn[size]='\0'; // Null-terminate the extension
    char output_str[25]; // Buffer to hold the full output file name
    strcpy(output_str,decInfo->output_file); // Copy the base output file name
    strcat(output_str,decInfo->extn); // Append the extension to the output file name
    decInfo->fptr_out = fopen(output_str, "w"); // Open the output file for writing
    // Do Error handling
    if (decInfo->fptr_out == NULL) // Check if the file was opened successfully
    {
        perror("fopen"); // Print error message if file opening fails
        fprintf(stderr, "ERROR: Unable to open output file %s\n", decInfo->output_file); // Print error details

        return e_failure; // Return failure status
    }
    // No failure return e_success*/
    return e_success; // Return success status
}

//decode secret file data size 
Status decode_secret_file_data_size(DecodeInfo *decInfo) // Function to decode the size of the secret file data
{
    char data[32]; // Buffer to hold data read from the stego image
    fread(data,32, 1,decInfo->fptr_stego); // Read 32 bits from the stego image
    decInfo->extn_data_size = decode_byte_to_size(data); // Decode and store the data size

    return e_success; // Return success status
}

//decode secret file data
Status decode_secret_file_data(long size, DecodeInfo *decInfo) // Function to decode the actual secret file data
{
    char data[8]; // Buffer to hold data read from the stego image
    for(int i=0;i<size;i++) // Loop through the size of the data
    {
        fread(data,8,1,decInfo->fptr_stego); // Read 8 bits from the stego image
        decInfo-> extn_data[i]=decode_byte_to_lsb(data); // Decode and store the data byte
    }
    decInfo->extn_data[size]='\0'; // Null-terminate the data
    fwrite(decInfo->extn_data,decInfo->extn_data_size,1,decInfo->fptr_out); // Write the decoded data to the output file
    fclose(decInfo->fptr_out); // Close the output file
    return e_success; // Return success status
}

//start do decoding 
Status do_decoding(DecodeInfo *decInfo) // Function to execute the decoding process
{
    printf("Decoding started\n"); // Print start message
    if(open_files_decode(decInfo) == e_success) // Attempt to open files for decoding
    {
        printf("Open files is successfull\n"); // Print success message

        if(decode_magic_string(decInfo)==e_success) // Attempt to decode the magic string
        {
            printf("Magic string is matched\n"); // Print success message

            if(decode_secret_file_size(decInfo) == e_success) // Attempt to decode the secret file size
            {
                printf("Decode secret extension size is successfull\n"); // Print success message

                if(decode_secret_file_extn(decInfo->extn_size , decInfo) == e_success) // Attempt to decode the secret file extension
                {
                    printf("Decode secret file extension is successfull\n"); // Print success message

                    if(decode_secret_file_data_size(decInfo) == e_success) // Attempt to decode the secret file data size
                    {
                        printf("Decode secret file data size is successfull\n"); // Print success message

                        if(decode_secret_file_data(decInfo->extn_data_size,decInfo) == e_success) // Attempt to decode the secret file data
                        {
                            printf("Decode secret file data is successfull\n"); // Print success message
                        }
                        else // If decoding secret file data fails
                        {
                            printf("Decode secret file data is Failed\n"); // Print failure message
                            return e_failure; // Return failure status
                        }
                    }
                    else // If decoding secret file data size fails
                    {
                        printf("Decode secret file data size is Failed\n"); // Print failure message
                        return e_failure; // Return failure status
                    }

                }
                else // If decoding secret file extension fails
                {
                    printf("Decode secret file extension is Failed\n"); // Print failure message
                    return e_failure; // Return failure status
                }
            }
            else // If decoding secret file size fails
            {
                printf("Decode secret extention size is Failed\n"); // Print failure message
                return e_failure; // Return failure status
            }

        }
        else // If decoding magic string fails
        {
            printf("Decoding of  magic string is failed\n"); // Print failure message
            return e_failure; // Return failure status
        }
    }
    else // If opening files fails
    {
        printf("Opening files is failed\n"); // Print failure message
        return e_failure; // Return failure status
    }
    return e_success; // Return success status
}
