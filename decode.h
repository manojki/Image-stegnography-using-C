#ifndef DECODE_H
#define DECODE_H
#include <stdio.h>
#include "types.h"

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 20

typedef struct _DecodeInfo
{
    char *stego_image_fname;
    FILE *fptr_stego_image;

    char output_fname[50];
    FILE *fptr_output;

    char extn_secret_file[MAX_FILE_SUFFIX];
    long size_secret_file;

    char image_data[MAX_IMAGE_BUF_SIZE];
    char secret_data[MAX_SECRET_BUF_SIZE];
}DecodeInfo;

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

Status do_decoding(DecodeInfo *decInfo);

Status open_files_decode(DecodeInfo *decInfo);

Status decode_magic_string(char *magic_string, DecodeInfo *decInfo);

Status decode_secret_file_extn_size(int *extn_size, DecodeInfo *decInfo);

Status decode_secret_file_extn(char *file_extn, int extn_size, DecodeInfo *decInfo);

Status decode_secret_file_size(long *file_size, DecodeInfo *decInfo);

Status decode_secret_file_data(DecodeInfo *decInfo);

Status decode_byte_from_lsb(char *data, char *image_buffer);

Status decode_size_from_lsb(long *data, char *image_buffer);

#endif








