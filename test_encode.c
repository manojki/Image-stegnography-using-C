#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;

    int ret = check_operation_type(argv);
    
    if(ret == e_encode)
    {
        //encoding
        int ret = read_and_validate_encode_args(argv, &encInfo);
        if(ret == e_failure)
        {
            printf("Validation is not correct.\n");
            return 0;
        }
        ret = do_encoding(&encInfo);
        if(ret == e_failure)
        {
            printf("Encoding is failed.\n");
            return 0;
        }
        printf("Encoding is Successful.\n");
        return 0;
                
    }

    else if(ret == e_decode)
    {
        //decoding
        int ret = read_and_validate_decode_args(argv, &decInfo);
        if(ret == e_failure)
        {
            printf("Validation is not correct.\n");
            return 0;
        }
        ret = do_decoding(&decInfo);
        if(ret == e_failure)
        {
            printf("Decoding is failed.\n");
            return 0;
        }
        printf("Decoding is successful.\n");
        return 0;
    }

    else
    {
        //unsupported
        printf("Invalid operation type\n");
        return 0;
    }
}

