#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types
#define buff_size 4
#define data_size 50

typedef struct _DecodeInfo  // Structure to hold information for decoding
{
    char *stego_bmp;  // File name of the stego BMP image
    FILE *fptr_stego; // File pointer for the stego BMP image

    char *output_file; // File name for the output secret file
    char output_fname[20]; // Buffer to hold the output file name
    FILE *fptr_out; // File pointer for the output secret file
    int extn_size; // Size of the secret file extension
    char extn[buff_size]; // Buffer to hold the secret file extension
    int extn_data_size; // Size of the secret file data
    char extn_data[data_size]; // Buffer to hold the secret file data
} DecodeInfo; // End of DecodeInfo structure definition


OperationType check_operation_type(char *argv[]); // Function to check the operation type based on command-line arguments


Status read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *encInfo); // Function to read and validate decoding arguments


Status do_decoding(DecodeInfo *decInfo); // Function to perform the decoding process


Status open_files_decode(DecodeInfo *decInfo); // Function to open necessary files for decoding


Status decode_magic_string(DecodeInfo *decInfo); // Function to decode the magic string from the image


Status decode_secret_file_size(DecodeInfo *decInfo); // Function to decode the size of the secret file


Status decode_secret_file_extn(long size, DecodeInfo *decInfo); // Function to decode the extension of the secret file


Status decode_secret_file_data_size(DecodeInfo *decInfo); // Function to decode the size of the secret file data


Status decode_secret_file_data(long size, DecodeInfo *decInfo); // Function to decode the actual secret file data


Status decode_byte_to_lsb(char *image_buffer); // Function to decode a byte from the least significant bits


Status decode_byte_to_size(char *image_buffer); // Function to decode size information from the image buffer
#endif // End of header guard
