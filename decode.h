#ifndef ENCODE_H
#define ENCODE_H
#include"types.h"

#include<stdio.h>
#include"common.h"

typedef struct  _DecodeInfo{
    char *stego_img_fname;
    FILE* fptr_stego_img;

    char* decode_file_fname;
    FILE* fptr_decode_file;

    char secre_file_extn[5];
    long secret_file_size;
}DecodeInfo;

/*do decoding*/
Status Do_decoding(DecodeInfo*decInfo);

/*function for read and validate inputs*/
Status read_and_validate_decode_args(int argc,char*argv[],DecodeInfo *decode);

/*function for opening files*/
Status open_decode_file(DecodeInfo*decInfo);

/*function for operation type whether it is decoding or not*/
//Status check_operation_type(char*argv);

/*decode magic string from stego.bmp*/
Status Decode_magic_string(DecodeInfo*decInfo,char* magic_string);

/*to skip 54 byte bmp header*/
Status skip_bmp_header(DecodeInfo *decInfo);

/*to decode extension size of secret file so on decoding end we can store data */
Status decode_secret_file_exte_size(DecodeInfo*decInfo,int* exte_size);

/*decode seret file size*/
Status decode_secret_file_size(DecodeInfo*decInfo);

/*deocode secret file from secret.bmp*/
Status decode_secret_file_exte(DecodeInfo*decInfo ,int exte_size);

/*Decode secret file data from secret.bmp*/
Status decode_secret_file_data(DecodeInfo*decInfo);

//extension part of op file
Status open_output_file(DecodeInfo *decInfo);

/*close all open file */
Status close_decode_files(DecodeInfo*decInfo);
#endif
