#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>

/**
 * @brief wzip project code - compresses a file using RLE encoding
 * @author Logan Cole
 * Stage 1: Compeleted 4/7/2022
 * Stage 2: Not finished
 */


/**
 * @brief Writes the output to stdout
 * the count should be a 4 byte int
 * the letter should be a 1 byte char
 * for a total of 5 byte output for each count and letter
 * @param count the count of the letter
 * @param letter the letter to be printed
 */
void writeOut(int count, char *letter){
    fwrite(&count, 4, 1, stdout);
    fwrite(letter, 1, 1, stdout);
}

void* handleLargeFile(void* param){
    return NULL;
}

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
            writeOut(count, letterToChk);   //to check to the current letter and reset count
            strcpy(letterToChk, nextLetter);
            count = 1;
        }
    }
    return count;
}

int main(int argc, char *argv[]){
    struct stat finfo;
    char letterToChk[2];
    int count = 0;

    if(argc < 2){ //checks if its given any files
        printf("wzip: file1 [file2 ...]\n");
        exit(1);
    }
    for(int i = 1; i < argc; i++){
        FILE *fp = fopen(argv[i], "r");
        if(fp != NULL){
            fstat(fileno(fp), &finfo);
            if(finfo.st_size > 4096){
                pthread_t threads[3];
                for(int i = 0; i < 3; i++){
                    pthread_create(&threads[i], NULL, handleLargeFile, NULL);
                }
            }else{
                fread(&letterToChk, 1, 1, fp); //grabs the first char
                count += compress(fp, letterToChk);
            }
            //printf("%lld", finfo.st_size);
        }else{
            printf("Invalid file...\n");
            exit(1);
        }
    }
    writeOut(count, letterToChk);
}