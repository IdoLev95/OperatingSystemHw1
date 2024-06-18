all: os.c pt.c
	gcc -O3 -Wall -std=c11 os.c pt.c -o os
debug: os.c pt.c
	gcc -ggdb -Wall -std=c11 os.c pt.c -o os

