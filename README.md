## Purpose
Some data does not really fit in the relationnal model. Like big arrays of measures. Storing those as records in tables tend to use a lot of space and using aggregation functions on the resulting data set is not all that fast.

There also is the possibility of storing raw data in blobs, but then SQL is not usable for anything meaningful and we need to retrieve our data to process it in application code.

Hence the idea of using User Defined Functions to get a few useable operations on said blobs. I started writing those to see how much improvement they would bring for operations like a "SELECT SUM(value) [...]".

## Usage
Like any MySQL UDF: After the customary "make", copy "libudf_arrays.so" into the "lib/plugin" folder of MySQL.

The files "create.sql" and "drop.sql" respectively contain all the "CREATE FUNCTION" and "DROP FUNCTION" statements.

Functions are named "operation_type_endianness". Available operations are curently sum, min and max. Types are int32, int64, u_int32, u_int64, float and double. Endianness is either "be" or "le".

Example:
```SQL
SOURCE /spare/projects/UDF/create.sql;

SELECT min_int32_be(0x0000000100000002);
-- 1

SELECT max_int32_be(0x0000000100000002);
-- 2

SELECT sum_int32_be(0x0000000100000002);
-- 3
```
## License
That would be a BSD 2-Clause License.