#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if (argv[2] == NULL)
    {
        printf("stego image file not passed\n");
        return e_failure;
    }

    if (strstr(argv[2], ".bmp") == NULL)
    {
        printf("Invalid stego image file.\n");
        return e_failure;
    }

    decInfo->stego_image_fname = argv[2];
    
    if(argv[3] != NULL)
    {
        strcpy(decInfo->output_fname, argv[3]);
    }
    else
    {
        strcpy(decInfo->output_fname, "output");
    }

    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    char magic_string[3];
    int extn_size;
    char file_extn[MAX_FILE_SUFFIX];
    long file_size;
    
    printf("1.Opening files.\n");
    if (open_files_decode(decInfo) == e_failure)
    {
        printf("Unable to open files\n");
        return e_failure;
    }


    printf("2.Skipping BMP Header.\n");
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);

    printf("3.Decoding Magic string.\n");
    if (decode_magic_string(magic_string, decInfo) == e_failure)
        return e_failure;

    printf("Magic String = %s\n", magic_string);

    if (strcmp(magic_string, MAGIC_STRING) != 0)
    {
        printf("Magic string mismatch\n");
        return e_failure;
    }

    printf("4.Decoding extension size.\n");
    if (decode_secret_file_extn_size(&extn_size, decInfo) == e_failure)
        return e_failure;

    printf("Extension size = %d\n", extn_size);

    if(extn_size <= 0 || extn_size >= MAX_FILE_SUFFIX)
    {
        printf("Invalid extension size.\n");
        return e_failure;
    }

    printf("5.Decoding extension.\n");
    if (decode_secret_file_extn(file_extn, extn_size, decInfo) == e_failure)
        return e_failure;

    printf("Extension = %s\n", file_extn);

    strcat(decInfo->output_fname, file_extn);

    printf("Outfile = %s\n", decInfo->output_fname);

    decInfo->fptr_output = fopen(decInfo->output_fname, "wb");

    if (decInfo->fptr_output == NULL)
    {
        printf("Unable to create output file\n");
        return e_failure;
    }

    printf("6.Decoding secret file size.\n");
    if (decode_secret_file_size(&file_size, decInfo) == e_failure)
        return e_failure;

    printf("Secret file size = %ld bytes\n", file_size);

    if(file_size <= 0)
    {
        printf("Invalid secret file size.\n");
    }

    decInfo->size_secret_file = file_size;

    printf("7.Decoding secret file data.\n");

    if (decode_secret_file_data(decInfo) == e_failure)
        return e_failure;

    printf("8.closing files.\n");
    fclose(decInfo->fptr_stego_image);
    fclose(decInfo->fptr_output);

    return e_success;
}

Status open_files_decode(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");

    if (decInfo->fptr_stego_image == NULL)
    {
        printf("Unable to open stego image file\n");
        return e_failure;
    }

    return e_success;
}

Status decode_magic_string(char *magic_string, DecodeInfo *decInfo)
{
    char buffer[8];

    for (int i = 0; i < 2; i++)
    {
        fread(buffer, 8, 1, decInfo->fptr_stego_image);

        if (decode_byte_from_lsb(&magic_string[i], buffer) == e_failure)
            return e_failure;
    }

    magic_string[2] = '\0';

    return e_success;
}

Status decode_secret_file_extn_size(int *extn_size, DecodeInfo *decInfo)
{
    char buffer[32];
    long size;

    if(fread(buffer, 32, 1, decInfo->fptr_stego_image) != 1)
    {
        printf("Error reading extn size\n");
        return e_failure;
    }

    if (decode_size_from_lsb(&size, buffer) == e_failure)
        return e_failure;

    *extn_size = (int)size;

    return e_success;
}

Status decode_secret_file_extn(char *file_extn, int extn_size, DecodeInfo *decInfo)
{
    char buffer[8];

    for (int i = 0; i < extn_size; i++)
    {
        if(fread(buffer, 8, 1, decInfo->fptr_stego_image) != 1)
        {
            printf("Error reading extn\n");
            return e_failure;
        }

        if (decode_byte_from_lsb(&file_extn[i], buffer) == e_failure)
            return e_failure;
    }

    file_extn[extn_size] = '\0';

    return e_success;
}

Status decode_secret_file_size(long *file_size, DecodeInfo *decInfo)
{
    char buffer[32];

    if(fread(buffer, 32, 1, decInfo->fptr_stego_image) != 1)
    {
        printf("Error reading file size\n");
        return e_failure;

    }
    if (decode_size_from_lsb(file_size, buffer) == e_failure)
        return e_failure;

    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char buffer[8];
    char ch;

    for (long i = 0; i < decInfo->size_secret_file; i++)
    {
        if(fread(buffer, 8, 1, decInfo->fptr_stego_image) != 1)
        {
            printf("Error reading secret data at byte %ld\n", i);
            return e_failure;
        }

        if (decode_byte_from_lsb(&ch, buffer) == e_failure)
            return e_failure;

        fwrite(&ch, 1, 1, decInfo->fptr_output);
    }

    return e_success;
}

Status decode_byte_from_lsb(char *data, char *image_buffer)
{
    *data = 0;

    for (int i = 0; i < 8; i++)
    {
        *data = (*data << 1) | (image_buffer[i] & 1);
    }

    return e_success;
}

Status decode_size_from_lsb(long *data, char *image_buffer)
{
    *data = 0;

    for (int i = 0; i < 32; i++)
    {
        *data = (*data << 1) | (image_buffer[i] & 1);
    }

    return e_success;
}