#include<stdio.h>
#include<string.h>
#include"common.h"
#include"types.h"
#include"decode.h"

int main(int argc,char*argv[]){

    DecodeInfo decode;
    if(argc<2){
        printf("\033[1;31m✘ Insufficient Arguments Count\033[0m\n");
        return 1;
    }

    if(strcmp(argv[1],"-d")==0){
        if(read_and_validate_decode_args(argc,argv,&decode)==e_success){

            printf("\033[1;36m\n========================================\n");
            printf("           DECODING PROCESS\n");
            printf("========================================\033[0m\n");
            printf("\033[1;32m\n✔ READ & VALIDATE SUCCESSFULL\033[0m\n");
            
            if(Do_decoding(&decode)==e_success){
            
                printf("\n" CYAN);
                printf("+------------------------------------------------+\n");
                printf(RESET GREEN "       DECODING COMPLETED SUCCESSFULLY\n" RESET);
                printf(CYAN "+------------------------------------------------+\n" RESET);
            }
            
            else{
            
                printf("\033[1;31m✘ DECODING FAILED\033[0m\n");
            
            }
        }
        
        else{
        
            printf("\033[1;31m✘ READ AND VALIDATE FAILED\033[0m\n");
        }
    }
    
    else{
    
        printf("\033[1;31m✘ INVALID OPERATION\033[0m\n");
    }
    
    return 0;

}