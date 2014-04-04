
CC = gcc
CFLAGS = -Wall -std=c99 -fPIC -g
INCLUDES = -I/usr/include/mysql/
TEST_LINK = -lcunit

all: test udf_arrays.so

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@ 

udf_arrays.so: udf_arrays.o
	$(CC) $(CFLAGS) -shared -o udf_arrays.so udf_arrays.o

clean:
	rm *.o
	rm udf_arrays.so
	rm test_udf_arrays

test: udf_arrays.o test_udf_arrays
	./test_udf_arrays

test_udf_arrays: test_udf_arrays.o
	$(CC) $(CFLAGS) -o test_udf_arrays test_udf_arrays.o $(TEST_LINK)
