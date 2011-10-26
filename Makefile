CC     = gcc


all: tests

main.c:
	sh make-tests.sh > main.c

tests: main.c heap.o test_heap.c CuTest.c main.c
	$(CC) -o $@ $^
	./tests

heap.o: heap.c
	$(CC) -c -o $@ $^

clean:
	rm -f main.c heap.o tests
