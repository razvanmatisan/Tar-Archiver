CC=gcc
CFLAGS=-Wall -Wextra -std=c99

build: archiver

archiver: archiver.c
	$(CC) $(CFLAGS) archiver.c -o archiver

pack:
	zip -FSr 314CA_MatisanRazvan_Tema3.zip README Makefile *.c

clean:
	rm -f archiver
