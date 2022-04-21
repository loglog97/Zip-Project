all:
	gcc -o wzip wzip.c -Wall -Werror -pthread

clean:
	rm -f wzip submission.zip

zip:
	zip submission.zip *.c *.h MakeFile