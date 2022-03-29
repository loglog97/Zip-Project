#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void throwError(){
    char error_message[30] = "An error has occurred\n";
    fprintf(stderr, error_message, strlen(error_message));
}
void compress(char *input){

}

int main(int argc, char *argv[]){
    int fileCount = 1;
    char *inputLine = malloc(255*sizeof(char));
    size_t buffer = 255;

    while(fileCount < argc){
        FILE *fp = fopen(argv[fileCount], "r");
        if(fp != NULL){
            while(getline(&inputLine, &buffer, fp) != EOF){
                compress(inputLine);
            }
        }else{
            throwError();
        }
        fileCount++;
    }
}