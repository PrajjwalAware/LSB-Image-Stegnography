#include <stdio.h>
#include "encode.h"
#include "types.h"
#include<string.h>
#include"common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte 
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

/***************************************************************************************************************************** */
// ./a.out -e beautiful.bmp secret.txt [stego.bmp]
// Check the argv[2] extn is .bmp or not

Status read_and_validate_encode_args(char*argv[],EncodeInfo *encInfo){
   /* if(argc!=4){
        printf("The Entered Arguments are not Enough\n");
        printf("Enter arguments like -> ./a.out -e beautiful.bmp secret.txt [stego.bmp]\n");
        printf("For enocing use -e for decodin -d\n");
        return e_failure;
    }
        */

    if(strstr(argv[2],".bmp")!=NULL){
        encInfo->src_image_fname=argv[2];
        if(strstr(argv[3],".txt")!=NULL){
            encInfo->secret_fname=argv[3];
            char *ptr = strrchr(argv[3], '.');
            strcpy(encInfo->extn_secret_file, ptr);
            if(argv[4]==NULL){
                encInfo->stego_image_fname="default.bmp";
            }
            else if(strstr(argv[4],".bmp")!=NULL){
                encInfo->stego_image_fname=argv[4];
            }
            else{
                return e_failure;
            }
        }
        else{
            return e_failure;
        } 
    }
    else{

        return e_failure;
    }

    return e_success;
    
}

Status do_encoding(EncodeInfo* encinfo){
    
    if(open_files(encinfo)==e_success){
        
        printf(GREEN "\n\n✔ Files opened successfully\n" RESET);
        
        if(check_capacity(encinfo)==e_success){

            printf(GREEN "✔ Image capacity verified successfully\n" RESET);
            
            if(copy_bmp_header(encinfo->fptr_src_image,encinfo->fptr_stego_image)==e_success){
                
                printf(GREEN "✔ BMP header copied successfully\n" RESET);
                
                if((encode_magic_string(MAGIC_STRING,encinfo))==e_success){

                    printf(GREEN "✔ Magic string encoded successfully\n" RESET);
                
                    if((encode_secret_file_extn_size(encinfo->extn_secret_file,encinfo))==e_success){
                       
                        if (encode_secret_file_extn(encinfo->extn_secret_file, encinfo) != e_success){
                            return e_failure;
                        }
                        printf(GREEN "✔ Secret file extension encoded successfully\n" RESET);
                        

                        if (encode_secret_file_size(encinfo->size_secret_file, encinfo) != e_success){
                            return e_failure;
                        }
                        printf(GREEN "✔ Secret file size encoded successfully\n" RESET);

                        if (encode_secret_file_data(encinfo) != e_success){
                            return e_failure;
                        }
                        printf(GREEN "✔ Secret file data encoded successfully\n" RESET);
                            
                        if (copy_remaining_img_data(encinfo->fptr_src_image,encinfo->fptr_stego_image) != e_success){
                            return e_failure;
                        }

                        printf(GREEN "✔ Remaining image data copied successfully\n" RESET);
                    }
                }
            }
            
            else{
                
                return e_failure;
            }
        }

        else{
            return e_failure;
        }
    }
    else{
        return e_failure;
    }
    return e_success;
}
//it checks the image capacity and actual bytes needed to encode data
Status check_capacity(EncodeInfo *encInfo){
    
    encInfo ->image_capacity=get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo ->size_secret_file=get_file_size(encInfo->fptr_secret);
    encInfo->exte_size = strlen(encInfo->extn_secret_file);
    
    if(encInfo->image_capacity>(16 + 32 + (encInfo->exte_size * 8) + 32 + (encInfo->size_secret_file * 8))){
        return e_success;
    }
    return e_failure;
}

//this calculates the size of file
uint get_file_size(FILE *fptr){
    
    //we have to set offest to 0
    fseek(fptr,0,SEEK_END);
    //calulate like end_pos of fptr - starting i.e. 0th
    return ftell(fptr);

}

//this copies the bmp header of src image to the destinantion image 
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image){
    
    char arr[54];
    fseek(fptr_src_image,0,SEEK_SET);
    //read data from src img to empty arr
    fread(arr,54,1,fptr_src_image);
    //write data to dest img from arr
    fwrite(arr,54,1,fptr_dest_image);
    return e_success;

}

//encode 1 byte data char type data
Status encode_byte_to_lsb(char data,char *image_buffer){
    for(int i =0 ; i<8;i++){
        if((data&(1<<i))!=0){
            image_buffer[i]=(image_buffer[i] & 0xFE)|1;
        }
        else{
            image_buffer[i]=(image_buffer[i] & 0xFE)|0;
        }

    }
    return e_success;
}

//Encode 4 byte data like extension and all
Status encode_int_to_lsb(int data,char *image_buffer){
    for(int i =0 ; i<32;i++){
        if((data&(1<<i))!=0){
            image_buffer[i]=(image_buffer[i] & 0xFE)|1;
        }
        else{
            image_buffer[i]=(image_buffer[i] & 0xFE)|0;
        }

    }
    return e_success;
}

//checks magic string encodeing done or not and returns success
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo){
    if(encode_data_to_image(magic_string,strlen(magic_string),encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success){
        return e_success;
    }
    else {
        return e_failure;
    }
}

//actual encoding of data to image
Status encode_data_to_image(const char *data , int size , FILE *fptr_src_image,FILE *fptr_stego_image){
    char buffer[8];
    for(int i=0;i<size;i++){
        fread(buffer,8,1,fptr_src_image);
        if(encode_byte_to_lsb(data[i],buffer) != e_success){
            return e_failure;
        }
        fwrite(buffer,8,1,fptr_stego_image);
    }
    return e_success;
}

Status encode_secret_file_extn_size(const char* file_extn,EncodeInfo *encInfo){
    encInfo->exte_size = strlen(file_extn);
    char buffer[32];
    fread(buffer,32,1,encInfo->fptr_src_image);
    if(encode_int_to_lsb(encInfo->exte_size,buffer)!=e_success){
        return e_failure;
    }
    
    fwrite(buffer,32,1,encInfo->fptr_stego_image);
    return e_success;
}


Status copy_remaining_img_data(FILE *fptr_src,FILE *fptr_dest){
    char ch;
    while(fread(&ch,1,1,fptr_src)==1){
        
       if(fwrite(&ch,1,1,fptr_dest)!=1){
        return e_failure;
       }
    }
    fclose(fptr_dest);
    fclose(fptr_src);
    return e_success;
}


Status encode_secret_file_size(long file_size, EncodeInfo *encInfo){
    
    
    char buffer[32];
   
    fread(buffer,32,1,encInfo->fptr_src_image);
    if(encode_int_to_lsb(file_size,buffer)!=e_success){
        return e_failure;
    }
    fwrite(buffer, 32, 1, encInfo->fptr_stego_image);
    return e_success;
}


Status encode_secret_file_data(EncodeInfo *encInfo){
    char buffer[encInfo->size_secret_file];
    rewind(encInfo->fptr_secret);
    fread(buffer,encInfo->size_secret_file,1,encInfo->fptr_secret);
    if(encode_data_to_image(buffer , encInfo->size_secret_file , encInfo->fptr_src_image,encInfo->fptr_stego_image)!=e_success){
        return e_failure;
    }
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo){
    if(encode_data_to_image(file_extn,strlen(file_extn),encInfo->fptr_src_image,encInfo->fptr_stego_image)!=e_success){
        return e_failure;
    }

    return e_success;
}
