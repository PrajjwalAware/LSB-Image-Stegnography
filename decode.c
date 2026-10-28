#include<stdio.h>
#include<string.h>
#include"common.h"
#include"types.h"
#include"decode.h"

//DecodeInfo decode;

Status Do_decoding(DecodeInfo*decInfo){
    
    if(open_decode_file(decInfo)==e_success){
        
        printf("\033[1;32m✔ FILE OPEN SUCCESSFULL FOR DECODING\033[0m\n");
        
        if(skip_bmp_header(decInfo)==e_success){
        
            printf("\033[1;32m✔ POINTING TO ENCODED DATA AFTER BMP HEADER\033[0m\n");
        
        }

        char magic_string[strlen(MAGIC_STRING)+1];
        if(Decode_magic_string(decInfo,magic_string) == e_failure){
            printf("\033[1;31m✘ Magic string decoding failed\033[0m\n");
            return e_failure;
        }
        else { 
            printf("\033[1;32m✔ Magic string decoded successfully\033[0m\n"); 
        }


        int exte_size;
        if(decode_secret_file_exte_size(decInfo,&exte_size)==e_success){
        
            printf("\033[1;32m✔ Secret file extension size decoded successfully\033[0m\n");
        
        }


        if(decode_secret_file_exte(decInfo,exte_size)==e_success){
        
            printf("\033[1;32m✔ Secret file extension decoded successfully\033[0m\n");
        
        }
        //printf("ext size=%d ext=%s\n",exte_size,decInfo->secre_file_extn);


        if(decode_secret_file_size(decInfo)==e_success){
        
            printf("\033[1;32m✔ Secret file size decoded successfully\033[0m\n");
        
        }


        if(decode_secret_file_data(decInfo)==e_success){
        
            printf("\033[1;32m✔ Secret file data decoded successfully\033[0m\n");
        }


        if(close_decode_files(decInfo)==e_success){
        
            printf("\033[1;32m✔ Decode files closed successfully\033[0m\n");
        }

    }
    else{
        
        return e_failure;
    
    }
    
    return e_success;

}
/*Function for file opening*/
Status open_decode_file(DecodeInfo*decInfo){
    decInfo->fptr_stego_img=fopen(decInfo->stego_img_fname,"rb");
    if(decInfo->fptr_stego_img==NULL){
       perror("fopen");
       fprintf(stderr, "\033[1;31m✘ Error: Unable to open file %s\033[0m\n", decInfo->stego_img_fname);
       return e_failure;
    }
    decInfo->fptr_decode_file=fopen(decInfo->decode_file_fname,"wb");
    if(decInfo->fptr_decode_file==NULL){
        perror("fopen");
        fprintf(stderr, "\033[1;31m✘ Error: Unable to create output file %s\033[0m\n", decInfo->decode_file_fname);
        fclose(decInfo->fptr_stego_img);
        return e_failure;
    }
    return e_success;
}

/*for read and validte the function inputs*/

Status read_and_validate_decode_args(int argc,char*argv[],DecodeInfo *decode){
    if(argc<3 || argc>4){
        return e_failure;
    }

    if(strstr(argv[2],".bmp")!=NULL){
        decode->stego_img_fname=argv[2];
        if(argc==4){
        if(strstr(argv[3],".txt")!=NULL){
                decode->decode_file_fname=argv[3];
                char *ptr= strrchr(decode->decode_file_fname,'.');
                int len=ptr-decode->decode_file_fname;
                char fname[100];
                strncpy(fname,argv[3],len);
                fname[len]='\0';
                strcat(fname,decode->secre_file_extn);
         
            }
        }
        if(argc==3){
            decode->decode_file_fname="Decode.txt";
        }
        else if(argc==4){ 
                if(strstr(argv[3],".txt")!=NULL){
                decode->decode_file_fname=argv[3];
            }
            else{
                return e_failure;
            }
        }

    }
    else{
        return e_failure;
    }
    return e_success;
    

}

/*skip bmp header of 54 bytes */

Status skip_bmp_header(DecodeInfo *decInfo){

    if(fseek(decInfo->fptr_stego_img,54,SEEK_SET)!=0){

        return e_failure;   
    
    }

    return e_success;

}


Status Decode_magic_string(DecodeInfo*decInfo,char* magic_string){

    unsigned char buff[8];
    int bit[8];
    int len =strlen(MAGIC_STRING);
    for(int i=0;i<len;i++){
        if(fread(buff,1,8,decInfo->fptr_stego_img)!=8){

            return e_failure;
    
        }

        for(int j=0;j<8;j++){
    
            bit[j] = buff[j] & 1;
    
        }
    //binary to decimal
        int dec_val=0;
        for(int j = 7;j>=0;j--){

            dec_val= dec_val* 2 +bit[j];
    
        }
        magic_string[i]=(char)dec_val;
    }
    magic_string[strlen(MAGIC_STRING)]='\0';
    
    if(strcmp(magic_string,MAGIC_STRING)==0){
       // printf("\033[1;32m✔ Magic string decoded successfully\033[0m\n");
        return e_success;
    }
    else{
        return e_failure;
    }

    return e_success;
    

}


/*decode Secret file extension size*/

Status decode_secret_file_exte_size(DecodeInfo*decInfo,int* exte_size){
    unsigned char buff[32];
    //fseek(decInfo->fptr_stego_img,70,SEEK_SET);
    if(fread(buff,1,32,decInfo->fptr_stego_img)!=32){
        return e_failure;
    }
    int bit[32];
    for(int i=0;i<32;i++){
        bit[i]=buff[i] & 1;
    }
    int dec=0;
    for(int i=31;i>=0;i--){
        dec=dec*2+bit[i];
    }
    *exte_size=dec;
    
    return e_success;
    
}

/*decode secret file size*/

Status decode_secret_file_size(DecodeInfo*decInfo){
    unsigned char buff[32];
   // fseek(decInfo->fptr_stego_img, 0, SEEK_CUR);
  
    if(fread(buff,1,32,decInfo->fptr_stego_img)!=32){
        return e_failure;
    }
    int bit[32];
    int dec=0;
    for(int i=0;i<32;i++){
        bit[i]=buff[i] & 1;
    }
    
    for(int i=31;i>=0;i--){
        dec=dec*2 + bit[i];
    }
   
    decInfo->secret_file_size=dec;
    return e_success;

}

Status decode_secret_file_exte(DecodeInfo*decInfo ,int exte_size){
    
    unsigned char buff[8];
    int bit[8];
    char ext[5];
    int i;
    for(i=0;i<exte_size;i++){
        
        fread(buff,1,8,decInfo->fptr_stego_img);

        for(int j=0;j<8;j++){
            bit[j]=buff[j]&1;
        }
        int dec=0;
        for(int j=7;j>=0;j--){
            dec=dec*2+bit[j];
        }
        ext[i]=dec;
        
        
    }
    ext[i]='\0';
    strcpy(decInfo->secre_file_extn, ext);
    return e_success;
}

/*decode secret data from secret.bmp */
Status decode_secret_file_data(DecodeInfo*decInfo){
    char buff[8];
    int bit[8];
    for(int i=0;i<decInfo->secret_file_size;i++){
        
        unsigned char dec=0;
        if(fread(buff,1,8,decInfo->fptr_stego_img)!=8){
            return e_failure;
        }
        for(int j=0;j<8;j++){
            bit[j]=buff[j] & 1;
        }

        for(int j=7;j>=0;j--){
            dec=dec*2 + bit[j];
        }
        fwrite(&dec,1,1,decInfo->fptr_decode_file);

    }
    return e_success;
}

/*close all files*/
Status close_decode_files(DecodeInfo*decInfo){
    fclose(decInfo->fptr_decode_file);
    fclose(decInfo->fptr_stego_img);
    return e_success;
}
