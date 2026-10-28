#include <stdio.h>
#include "encode.h"
#include "types.h"
#include<string.h>
#include"common.h"

int main(int argc,char* argv[])
{   
    if (argc < 2){
        printf("\033[1;31m✘ Insufficient Arguments Count\033[0m\n");
        return 1;
    }
    

    printf("\033[1;97m");
    printf("       * * * * * * * * * * * * *\n");
    printf("\033[0m");

    printf("\033[1;41;97m");
    printf("       LSB IMAGE STEGANOGRAPHY       ");
    printf("\033[0m\n");

    printf("\033[1;97m");
    printf("       * * * * * * * * * * * * *\n");
    printf("\033[0m");



    EncodeInfo encode;
    int ret=check_operation_type(argv);
    if(ret==e_encode){
        //encoding
        printf("\033[1;36m\n========================================\n");
        printf("            ENCODING PROCESS\n");
        printf("========================================\033[0m\n");
        printf("\033[1;32m\n✔ READ & VALIDATE SUCCESSFULL\033[0m\n");
            
        if((read_and_validate_encode_args(argv,&encode))==e_success){
            
            printf(GREEN "✔ Arguments validated successfully\n" RESET);

            FILE *fp = fopen(encode.secret_fname, "rb");

            if (fp != NULL){
                fseek(fp, 0, SEEK_END);
                encode.size_secret_file = ftell(fp);
                fclose(fp);
            }

            printf("\n" CYAN);
            printf("+------------------------------------------------+\n");
            printf("|                  IMAGE DETAILS                 |\n");
            printf("+------------------------+-----------------------+\n");

            printf(RESET "| %-22s | %-21s |\n", "Source Image", encode.src_image_fname);
            printf("| %-22s | %-21s |\n", "Secret File", encode.secret_fname);
            printf("| %-22s | %-20ld  |\n", "Secret File Size(Byte)", encode.size_secret_file);
            printf("| %-22s | %-21s |\n", "Output Image", encode.stego_image_fname);

            printf(CYAN "+------------------------+-----------------------+\n" RESET);  
            if(do_encoding(&encode)==e_success){
                printf("\n" CYAN);
                printf("+------------------------------------------------+\n");
                printf(RESET GREEN "       ENCODING COMPLETED SUCCESSFULLY\n" RESET);
                printf(CYAN "+------------------------------------------------+\n" RESET);
            }
            else{
                printf(RED "\n✖ Encoding failed\n" RESET);
            }
        }
        else{
            printf("\033[1;31m✘ READ AND VALIDATE FAILED\033[0m\n");
        }
    }
    else if(ret==e_decode){
        //decoding
    }
    else{
          printf("\033[1;31m✘ INVALID OPERATION\033[0m\n");
    }
    
    return 0;
}

    // ./a.out -e beautiful.bmp secret.txt [stego.bmp]
    // Check the argv[2] extn is .bmp or not
    OperationType check_operation_type (char*argv[]){
        if(strcmp(argv[1],"-e")==0){
            return e_encode;
        }
        else if(strcmp(argv[1],"-d")==0){
            return e_decode;
        }
        
        return e_unsupported;
        
        
    }

