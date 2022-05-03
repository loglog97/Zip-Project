all: wzip.o
	gcc -o wzip wzip.o -Wall -Werror -pthread

wzip.o: wzip.c
	gcc -c wzip.c
clean:
	rm -f wzip submission.zip *.o

zip:
	zip submission.zip *.c *.h makefile real exepct
