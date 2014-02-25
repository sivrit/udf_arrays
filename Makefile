udf_arrays.so: udf_arrays.c
	gcc -std=c99 -shared -o udf_arrays.so udf_arrays.c -I/usr/include/mysql/ -fPIC
