#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"

OperationType check_operation_type(char *argv[])
{
    if(argv[1] == NULL)
        return e_unsupported;

    if(strcmp(argv[1], "-e") == 0)
        return e_encode;
    
    if(strcmp(argv[1], "-d") == 0)
        return e_decode;
    
    return e_unsupported;
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(argv[2] == NULL)
    {
        printf("bmp file is not passed\n");
        return e_failure;
    }
    //chect argv[2] is a bmp file
    if(strstr(argv[2], ".bmp") == NULL)
    {
        printf("Invalid bmp file\n");
        return e_failure;
    }
    //storing argv[2]`bmp file name to structure
    encInfo->src_image_fname = argv[2];

    if(argv[3] == NULL)
    {
        printf("Secret file is not available\n");
        return e_failure;
    }
    if(strchr(argv[3], '.') == NULL)
    {
        printf("Invalid secret file.\n");
        return e_failure;
    }
    //store argv[3] to structure
    encInfo->secret_fname = argv[3];
    if(argv[4] == NULL)
    encInfo->stego_image_fname = "stego.bmp";
    else
    {
        if(strstr(argv[4], ".bmp") == NULL)
        {
            printf("Invalid output file name\n");
            return e_failure;
        }

        encInfo->stego_image_fname = argv[4];
    }

    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo) == e_failure)
    {
        printf("Error: Opening files failed..\n");
        return e_failure;
    }

    if(check_capacity(encInfo) == e_failure)
    {
        printf("Error: Insufficien image capacity.\n");
        return e_failure;
    }

    if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        printf("Failed to copy BMP header\n");
        return e_failure;
    }

    if(encode_magic_string(MAGIC_STRING,encInfo) == e_failure)
    {
        return e_failure;
    }

    char buffer[32];
    int extn_size = strlen(encInfo->extn_secret_file);

    fread(buffer, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(extn_size, buffer);
    fwrite(buffer, 32, 1, encInfo->fptr_stego_image);

    if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_failure)
    {
        return e_failure;
    }

    if(encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure)
    {
        return e_failure;
    }

    if(encode_secret_file_data(encInfo) == e_failure)
    {
        return e_failure;
    }

    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }


    return e_success;
}

Status open_files(EncodeInfo *encInfo)
{
    //open source image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");

    if(encInfo->fptr_src_image == NULL)
    {
        printf("Unable to open source image file.\n");
        return e_failure;
    }

    //open secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");

    if(encInfo->fptr_secret == NULL)
    {
        printf("Unable to open secret file.\n");
        fclose(encInfo->fptr_src_image);
        return e_failure;
    }

    //open stego image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");

    if(encInfo->fptr_stego_image == NULL)
    {
        printf("Unable to open stego image file.\n");
        fclose(encInfo->fptr_src_image);
        fclose(encInfo->fptr_secret);
        return e_failure;
    }

    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    strcpy(encInfo->extn_secret_file, strchr(encInfo->secret_fname, '.'));

    int extn_size = strlen(encInfo->extn_secret_file);


    fseek(encInfo->fptr_secret, 0, SEEK_END);
    encInfo->size_secret_file = ftell(encInfo->fptr_secret);
    fseek(encInfo->fptr_secret, 0, SEEK_SET);

    int total_bytes_needed = 54 + 16 + 32 + (encInfo->size_secret_file*8) + 32 + (extn_size*8);

    int bmp_file_size = get_image_size_for_bmp(encInfo->fptr_src_image);

    if(bmp_file_size >= total_bytes_needed)
        return e_success;

    else
        return e_failure;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char header[54];

    fread(header, 54, 1, fptr_src_image);
    fwrite(header, 54, 1, fptr_stego_image);

    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    int n = 7;

    for(int i = 0; i < 8; i++)
    {
        char mask = 1<< n;
        char bit = data & mask;

        bit = bit >> n;

        image_buffer[i] = image_buffer[i] & 0xFE;
        image_buffer[i] = image_buffer[i] | bit;

        n--;

    }

    return e_success;
}

Status encode_size_to_lsb(long data, char *image_buffer)
{
    int n = 31;

    for(int i = 0; i < 32; i++)
    {
        int mask = 1 << n;
        int bit = data & mask;

        bit = bit >> n;

        image_buffer[i] = image_buffer[i] & 0xFE;
        image_buffer[i] = image_buffer[i] | bit;

        n--;
    }

    return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    char buffer[8];

    for(int i = 0; i < 2; i++)
    {
        fread(buffer, 8, 1, encInfo->fptr_src_image);

        encode_byte_to_lsb(magic_string[i], buffer);

        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);
    }

    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char buffer[32];

    fread(buffer, 32, 1, encInfo->fptr_src_image);

    encode_size_to_lsb(file_size, buffer);

    fwrite(buffer, 32, 1, encInfo->fptr_stego_image);

    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    char buffer[8];

    for(int i = 0;file_extn[i] != '\0'; i++)
    {
        fread(buffer, 8, 1, encInfo->fptr_src_image);

        encode_byte_to_lsb(file_extn[i], buffer);

        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);
    }

    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char ch;
    char buffer[8];

    while(fread(&ch, 1, 1, encInfo->fptr_secret) == 1)
    {
        fread(buffer, 8, 1, encInfo->fptr_src_image);

        encode_byte_to_lsb(ch, buffer);

        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);

    }
    
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;

    while(fread(&ch, 1, 1, fptr_src) == 1)
    {
        fwrite(&ch, 1, 1, fptr_dest);
    }

    return e_success;
}

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;

    fseek(fptr_image, 18, SEEK_SET);

    fread(&width, sizeof(width), 1, fptr_image);
    fread(&height, sizeof(height), 1, fptr_image);

    fseek(fptr_image, 0, SEEK_SET);   // <-- IMPORTANT

    return width * height * 3;
}