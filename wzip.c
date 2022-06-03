#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <math.h>

/**
 * @brief wzip project code - compresses a file using RLE encoding
 * if a file is > 4096 bytes, we use three threads to compress the file
 * @author Logan Cole
 * Stage 1: Compeleted 4/7/2022
 * Stage 2: Completed 05/3/2022
 */
char *fileData;

struct arg_t{
    int count;
    char letter;
    struct arg_t *next;
};

struct Queue{
    int prevOffset; //start
    int offset; //end
    struct arg_t *front, *back;
};

/**
 * @brief Creates the node to enqueue
 * 
 * @param count - count to add
 * @param letter - letter to add
 * @return struct arg_t* - returns the node to the enqueue function
 */
struct arg_t* addNode(int count, char letter){
    struct arg_t* tmp = (struct arg_t*)malloc(sizeof(struct arg_t));
    tmp->count = count;
    tmp->letter = letter;
    return tmp;
}

/**
 * @brief Initalizes the Queue with the offsets
 * 
 * @param offset - when to stop reading
 * @param prevOffset - where to start reading
 * @return struct Queue* - the initalized queue
 */
struct Queue* createQueue(int offset, int prevOffset){
    struct Queue* q = (struct Queue*)malloc(sizeof(struct Queue));
    q->front = q->back = NULL;
    q->prevOffset = prevOffset;
    q->offset = offset;
    return q;
}

/**
 * @brief Enqueues an item
 * 
 * @param q - the queue
 * @param count - the count of that letter
 * @param letter - the letter it just counted
 */
void enQueue(struct Queue* q, int count, char letter){
    struct arg_t* temp = addNode(count, letter);
    if (q->back == NULL) {
        q->front = q->back = temp;
        return;
    }
    q->back->next = temp;
    q->back = temp;
}

/**
 * @brief Dequeues an item
 * 
 * @param q - the queue
 */
void deQueue(struct Queue* q){
    if (q->front == NULL){
        return;
    }
    struct arg_t* temp = q->front;
    q->front = q->front->next;
    if (q->front == NULL){
        q->back = NULL;
    }
    free(temp);
}

/**
 * @brief Computes the Length of the Queue
 * 
 * @param q - the queue
 * @return int - the length
 */
int queueLength(struct Queue* q){
    struct arg_t* tmp = q->front;
    int count = 0;
    while(tmp != NULL){
        tmp = tmp->next;
        count++;
    }
    free(tmp);
    return count;
}

/**
 * @brief Writes the output to stdout
 * @param count the count of the letter
 * @param letter the letter to be printed
 */
void writeOut(int count, char *letter){
    fwrite(&count, 4, 1, stdout);
    fwrite(letter, 1, 1, stdout);
}

void* handleLargeFile(void* param){
    struct Queue* q = (struct Queue*) param;
    int count = 1;
    char nextLetter;
    int i = 0;
    for(i = q->prevOffset; i < q->offset; i++){
        nextLetter = fileData[i+1];
        if(fileData[i] == nextLetter){
            count++;
        }else{
            enQueue(q, count, fileData[i]);
            count = 1;
        }
    }
    if(fileData[i] != '\0'){
        enQueue(q, count, fileData[i]);
    }
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
    int flag = 0;
    int lastCount = 0;
    char lastLetter = '\0';

    if(argc < 2){ //checks if its given any files
        printf("wzip: file1 [file2 ...]\n");
        exit(1);
    }
    for(int i = 1; i < argc; i++){
        FILE *fp = fopen(argv[i], "r");
        if(fp != NULL){
            fstat(fileno(fp), &finfo);
            if(finfo.st_size > 4096){
                flag = 1;
                pthread_t threads[3];
                char *src = mmap(0, finfo.st_size, PROT_READ, MAP_PRIVATE, fileno(fp), 0); // src is the starting char
                fileData = src; //map src to global variable
                int fileSplit = (int) strlen(src) / 3;
                struct Queue* q1 = createQueue(fileSplit, 0);
                struct Queue* q2 = createQueue(fileSplit*2, fileSplit+1);
                struct Queue* q3 = createQueue(strlen(src), fileSplit*2+1);
                pthread_create(&threads[0], NULL, handleLargeFile, q1);
                pthread_create(&threads[1], NULL, handleLargeFile, q2);
                pthread_create(&threads[2], NULL, handleLargeFile, q3);
                pthread_join(threads[0], NULL);
                pthread_join(threads[1], NULL);
                pthread_join(threads[2], NULL);
                //print first queue
                if(lastLetter != '\0'){
                    if(lastLetter == q1->front->letter){
                        writeOut(lastCount + q1->front->count, &q1->front->letter);
                        deQueue(q1);
                    }else{
                        writeOut(lastCount, &lastLetter);
                        lastCount = 0;
                        lastLetter = '\0';
                    }
                }
                int q1Len = queueLength(q1);
                for(int j = 0; j < q1Len - 1; j++){  //dint print the last node
                    writeOut(q1->front->count, &q1->front->letter);
                    deQueue(q1); //remove that node
                }
                if(q1->front->letter == q2->front->letter){ //if the end of first is same as first in the second
                    writeOut(q1->front->count + q2->front->count, &q1->front->letter);
                    deQueue(q1);
                    deQueue(q2);
                }else{
                    writeOut(q1->front->count, &q1->front->letter);
                    deQueue(q1);
                }
                //print middle bit
                int q2Len = queueLength(q2);
                for(int j = 0; j < q2Len - 1; j++){
                    writeOut(q2->front->count, &q2->front->letter);
                    deQueue(q2);
                }
                
                if(q2->front->letter == q3->front->letter){ //if the end of first is same as first in the second
                    writeOut(q2->front->count + q3->front->count, &q3->front->letter);
                    deQueue(q2);
                    deQueue(q3);
                }else{
                    writeOut(q2->front->count, &q2->front->letter);
                    deQueue(q2);
                }
                //print last bit
                int q3Len = queueLength(q3);
                for(int j = 0; j < q3Len - 1; j++){
                    writeOut(q3->front->count, &q3->front->letter);
                    deQueue(q3);
                }
                if(i+1 != argc){
                    lastCount = q3->front->count;
                    lastLetter = q3->front->letter;
                    deQueue(q3);
                }else{
                    writeOut(q3->front->count, &q3->front->letter);
                    deQueue(q3);
                }
                free(q1);
                free(q2);
                free(q3);
            }else{
                fread(&letterToChk, 1, 1, fp);
                count += compress(fp, letterToChk);
            }
        }else{
            printf("Invalid file...\n");
            exit(1);
        }
    }
    if(flag == 0){ //only runs if its not handling a large file
        writeOut(count, letterToChk);
    }
}
