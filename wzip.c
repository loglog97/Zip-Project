#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void writeFile(int count, char *letter){
    //printf("%d:%s\n", count, letter);
    fwrite(&count, 4, 1, stdout);
    fwrite(letter, 1, 1, stdout);
}

int compress(FILE *fp, char letterToChk[]){
    //char letterToChk[2]; //holds the current letter
    char nextLetter[2]; //used for following letter for comparing
    int count = 1;
    while(fread(&nextLetter, 1, 1, fp)){
        if(strcmp(&letterToChk[0], &nextLetter[0]) == 0){
            count++;
        }else{
            writeFile(count, letterToChk);
            strcpy(letterToChk, nextLetter);
            count = 1;
        }
        //printf("letterChK:%s\nnextLetter: %s\n", letterToChk, nextLetter);
    }
    return count;
    //writeFile(count, letterToChk);
    //printf("%c", letterToChk[0]);
}

int main(int argc, char *argv[]){
    char letterToChk[2];
    int count = 0;
    for(int i = 1; i < argc; i++){
        FILE *fp = fopen(argv[i], "r");
        if(fp != NULL){
            fread(&letterToChk, 1, 1, fp);
            count += compress(fp, letterToChk);
        }
    }
    writeFile(count, letterToChk);
}