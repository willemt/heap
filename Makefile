GCOV_OUTPUT = *.gcda *.gcno *.gcov 
GCOV_CCFLAGS = -fprofile-arcs -ftest-coverage
CC     = gcc
CCFLAGS = -g -O2 -Wall -Werror -W -fno-omit-frame-pointer -fno-common -fsigned-char $(GCOV_CCFLAGS)


all: tests

main.c:
	sh tests/make-tests.sh tests/test_*.c > main.c

tests: main.c heap.o tests/test_heap.c tests/CuTest.c main.c
	$(CC) $(CCFLAGS) -I. -Itests -o $@ $^
	./tests
	gcov main.c tests/test_heap.c heap.c

heap.o: heap.c
	$(CC) $(CCFLAGS) -c -o $@ $^

clean:
	rm -f main.c heap.o $(GCOV_OUTPUT)
