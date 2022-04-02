CC=       	gcc
CFLAGS= 	-g -gdwarf-2 -std=gnu99 -Wall -lmavalloc -pedantic -ansi
CLFLAGS= 	-g -gdwarf-2 -std=gnu99 -Wall -pedantic -ansi
LDFLAGS=
LIBRARIES=      lib/libmavalloc.a

all:   unit_test 

unit_test: main.o libmavalloc.a
	gcc -o unit_test main.o -L. -lmavalloc -g

main.o: main.c
	gcc  -c  main.c -g

mavalloc.o: mavalloc.c
	gcc  -c  mavalloc.c -g

libmavalloc.a: mavalloc.o
	ar rcs libmavalloc.a mavalloc.o

clean:
	rm -f *.o *.a unit_test

.PHONY: all clean
