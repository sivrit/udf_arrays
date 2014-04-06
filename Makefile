
CC = gcc
CFLAGS = -Wall -std=c99 -fPIC
INCLUDES = -I/usr/include/mysql/
TEST_LINK = -lcunit

all: test libudf_arrays.so

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@ 

libudf_arrays.so: udf_arrays.o
	$(CC) $(CFLAGS) -shared -o libudf_arrays.so udf_arrays.o

clean:
	rm -f *.o
	rm -f libudf_arrays.so
	rm -f test_udf_arrays

test: udf_arrays.o test_udf_arrays
	./test_udf_arrays

test_udf_arrays: test_udf_arrays.o udf_arrays.o
	$(CC) $(CFLAGS) -o test_udf_arrays test_udf_arrays.o $(TEST_LINK)
