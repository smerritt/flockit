all: libflockit.so test

libflockit.so: flockit.o
	gcc -shared -Wl,-soname,libflockit.so.1 -o libflockit.so flockit.o -ldl

flockit.o: flockit.c
	gcc -fPIC -Wall -c flockit.c

clean:
	rm -f *.o *.so test

test: test.c
	gcc -o test test.c
