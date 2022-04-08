#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief wzip project code - compresses a file using RLE encoding
 * @author Logan Cole
 * Stage 1: Compeleted 4/7/2022
 * Stage 2: Not finished
 */


/**
 * @brief counts the letters and compresses the file
 * 
 * @param fp a pointer to the file to compress
 * @param letterToChk the letter to start counting
 * @return the count of the letter
 */
int compress(FILE *fp, char letterToChk[]){
    char nextLetter[2]; //used for following letter for comparing
    int count = 1;
    while(fread(&nextLetter, 1, 1, fp)){ //runs while it can grab a letter
        if(strcmp(&letterToChk[0], &nextLetter[0]) == 0){ //if they match, increase counter
            count++;
        }else{ //if they dont, write letter and the count, set the letter
            fwrite(&count, 4, 1, stdout);
            fwrite(letterToChk, 1, 1, stdout);           //to check to the current letter and reset count
            strcpy(letterToChk, nextLetter);
            count = 1;
        }
    }
    return count;
}

int main(int argc, char *argv[]){
    char letterToChk[2];
    int count = 0;

    if(argc < 2){ //checks if its given any files
        printf("wzip: file1 [file2 ...]\n");
        exit(1);
    }
    for(int i = 1; i < argc; i++){
        FILE *fp = fopen(argv[i], "r");
        if(fp != NULL){
            fread(&letterToChk, 1, 1, fp); //grabs the first char
            count += compress(fp, letterToChk);
        }else{
            printf("Invalid file...\n");
            exit(1);
        }
    }
    fwrite(&count, 4, 1, stdout);
    fwrite(letterToChk, 1, 1, stdout);
}