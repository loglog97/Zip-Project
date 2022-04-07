all:
	gcc -o wzip wzip.c -Wall -Werror

clean:
	rm -f wzip submission.zip

zip:
	zip submission.zip *.c *.h