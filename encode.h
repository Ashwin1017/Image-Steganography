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
#define MAX_FILE_SUFFIX 4

typedef struct _EncodeInfo
{
    //beautiful.bmp file
    /* Source Image info */
    char *src_image_fname; //file name
    FILE *fptr_src_image; //source file.bmp
    uint image_capacity; //.bmp size
    uint bits_per_pixel;
    char image_data[MAX_IMAGE_BUF_SIZE];

    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;//access the secret data in secret.txt file
    char extn_secret_file[MAX_FILE_SUFFIX]; //store the extension .txt or .c or .sh
    char secret_data[MAX_SECRET_BUF_SIZE]; //name
    long size_secret_file; //secret file size

    /* Stego Image Info */
    //output image Info
    char *stego_image_fname; //output.bmp file name
    FILE *fptr_stego_image; //to access the output file

} EncodeInfo;


/* Encoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(int argc,char *argv[], EncodeInfo *encInfo); //check the extensions

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo);

/* Get File pointers for i/p and o/p files */
Status open_files(EncodeInfo *encInfo); //check all files r empty or not

/* check capacity */
Status check_capacity(EncodeInfo *encInfo);//my file is greater than all data or not

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image);//input .bmp file size

/* Get file size */
uint get_file_size(FILE *fptr); //.txt file size

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

/* Store Magic String */
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo);

/* Encode secret file extenstion */
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo);

/* Encode secret file size */
Status encode_secret_file_size(int file_size, EncodeInfo *encInfo);

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo);

/* Encode function, which does the real encoding */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image,EncodeInfo *encInfo);

/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(char data, char *image_buffer);

/* Encode the file extension size */
Status encode_size(int size, FILE *fptr_src_image, FILE *fptr_stego_image);

Status encode_size_to_lsb(char *buffer, int size);

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);


void print_size(EncodeInfo *encInfo);

#endif


