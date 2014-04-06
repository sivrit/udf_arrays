/*

Copyright (c) 2014, Tristan BARTEMENT
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

/*
** TODO doc
*/

#ifdef STANDARD
/* STANDARD is defined, don't use any mysql functions */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef __WIN__
typedef unsigned __int64 ulonglong;	/* Microsofts 64 bit types */
typedef __int64 longlong;
#else
typedef unsigned long long ulonglong;
typedef long long longlong;
#endif /*__WIN__*/
#else
#include <my_global.h>
#include <my_sys.h>
#if defined(MYSQL_SERVER)
#include <m_string.h>		/* To get strmov() */
#else
/* when compiled as standalone */
#include <string.h>
#define strmov(a,b) stpcpy(a,b)
#define bzero(a,b) memset(a,0,b)
#endif
#endif



#include <mysql.h>
#include <endian.h>

/*
 * sum_int32_be - Compute the sum of an array of signed 32 bits big endian integers
 * sum_int32_le - Compute the sum of an array of signed 32 bits little endian integers
 * sum_int64_be - Compute the sum of an array of signed 64 bits big endian integers
 * sum_int64_le - Compute the sum of an array of signed 64 bits little endian integers
 * 
 * sum_u_int32_be - Compute the sum of an array of unsigned 32 bits big endian integers
 * sum_u_int32_le - Compute the sum of an array of unsigned 32 bits little endian integers
 * sum_u_int64_be - Compute the sum of an array of unsigned 64 bits big endian integers
 * sum_u_int64_le - Compute the sum of an array of unsigned 64 bits little endian integers
 * 
 * 
 * min_int32_be - Compute the minimum of an array of signed 32 bits big endian integers
 * min_int32_le - Compute the minimum of an array of signed 32 bits little endian integers
 * min_int64_be - Compute the minimum of an array of signed 64 bits big endian integers
 * min_int64_le - Compute the minimum of an array of signed 64 bits little endian integers
 * 
 * min_u_int32_be - Compute the minimum of an array of unsigned 32 bits big endian integers
 * min_u_int32_le - Compute the minimum of an array of unsigned 32 bits little endian integers
 * min_u_int64_be - Compute the minimum of an array of unsigned 64 bits big endian integers
 * min_u_int64_le - Compute the minimum of an array of unsigned 64 bits little endian integers
 * 
 * 
 * max_int32_be - Compute the maximum of an array of signed 32 bits big endian integers
 * max_int32_le - Compute the maximum of an array of signed 32 bits little endian integers
 * max_int64_be - Compute the maximum of an array of signed 64 bits big endian integers
 * max_int64_le - Compute the maximum of an array of signed 64 bits little endian integers
 * 
 * max_u_int32_be - Compute the maximum of an array of unsigned 32 bits big endian integers
 * max_u_int32_le - Compute the maximum of an array of unsigned 32 bits little endian integers
 * max_u_int64_be - Compute the maximum of an array of unsigned 64 bits big endian integers
 * max_u_int64_le - Compute the maximum of an array of unsigned 64 bits little endian integers
 * 
 */

/*

create all functions:
 CREATE FUNCTION sum_int32_be RETURNS INTEGER SONAME "udf_arrays.so";
 CREATE FUNCTION sum_int32_le RETURNS INTEGER SONAME "udf_arrays.so";
 CREATE FUNCTION sum_int64_be RETURNS INTEGER SONAME "udf_arrays.so";
 CREATE FUNCTION sum_int64_le RETURNS INTEGER SONAME "udf_arrays.so";
 CREATE FUNCTION sum_u_int32_be RETURNS INTEGER SONAME "udf_arrays.so";
 CREATE FUNCTION sum_u_int32_le RETURNS INTEGER SONAME "udf_arrays.so";
 CREATE FUNCTION sum_u_int64_be RETURNS INTEGER SONAME "udf_arrays.so";
 CREATE FUNCTION sum_u_int64_le RETURNS INTEGER SONAME "udf_arrays.so";
 CREATE FUNCTION sum_float_be RETURNS REAL SONAME "udf_arrays.so";
 CREATE FUNCTION sum_float_le RETURNS REAL SONAME "udf_arrays.so";
 CREATE FUNCTION sum_double_be RETURNS REAL SONAME "udf_arrays.so";
 CREATE FUNCTION sum_double_le RETURNS REAL SONAME "udf_arrays.so";

 CREATE FUNCTION min_int32_be RETURNS INTEGER SONAME "udf_arrays.so";
 CREATE FUNCTION min_int32_le RETURNS INTEGER SONAME "udf_arrays.so";
 CREATE FUNCTION min_int64_be RETURNS INTEGER SONAME "udf_arrays.so";
 CREATE FUNCTION min_int64_le RETURNS INTEGER SONAME "udf_arrays.so";
 CREATE FUNCTION min_u_int32_be RETURNS INTEGER SONAME "udf_arrays.so";
 CREATE FUNCTION min_u_int32_le RETURNS INTEGER SONAME "udf_arrays.so";
 CREATE FUNCTION min_u_int64_be RETURNS INTEGER SONAME "udf_arrays.so";
 CREATE FUNCTION min_u_int64_le RETURNS INTEGER SONAME "udf_arrays.so";
 CREATE FUNCTION min_float_be RETURNS REAL SONAME "udf_arrays.so";
 CREATE FUNCTION min_float_le RETURNS REAL SONAME "udf_arrays.so";
 CREATE FUNCTION min_double_be RETURNS REAL SONAME "udf_arrays.so";
 CREATE FUNCTION min_double_le RETURNS REAL SONAME "udf_arrays.so";
 
 
 CREATE FUNCTION max_int32_be RETURNS INTEGER SONAME "udf_arrays.so";
 CREATE FUNCTION max_int32_le RETURNS INTEGER SONAME "udf_arrays.so";
 CREATE FUNCTION max_int64_be RETURNS INTEGER SONAME "udf_arrays.so";
 CREATE FUNCTION max_int64_le RETURNS INTEGER SONAME "udf_arrays.so";
 CREATE FUNCTION max_u_int32_be RETURNS INTEGER SONAME "udf_arrays.so";
 CREATE FUNCTION max_u_int32_le RETURNS INTEGER SONAME "udf_arrays.so";
 CREATE FUNCTION max_u_int64_be RETURNS INTEGER SONAME "udf_arrays.so";
 CREATE FUNCTION max_u_int64_le RETURNS INTEGER SONAME "udf_arrays.so";
 CREATE FUNCTION max_float_be RETURNS REAL SONAME "udf_arrays.so";
 CREATE FUNCTION max_float_le RETURNS REAL SONAME "udf_arrays.so";
 CREATE FUNCTION max_double_be RETURNS REAL SONAME "udf_arrays.so";
 CREATE FUNCTION max_double_le RETURNS REAL SONAME "udf_arrays.so";
 
 
drop all functions:
 DROP FUNCTION sum_int32_be;
 DROP FUNCTION sum_int32_le;
 DROP FUNCTION sum_int64_be;
 DROP FUNCTION sum_int64_le;
 DROP FUNCTION sum_u_int32_be;
 DROP FUNCTION sum_u_int32_le;
 DROP FUNCTION sum_u_int64_be;
 DROP FUNCTION sum_u_int64_le;
 DROP FUNCTION sum_float_be;
 DROP FUNCTION sum_float_le;
 DROP FUNCTION sum_double_be;
 DROP FUNCTION sum_double_le;
 

 DROP FUNCTION min_int32_be;
 DROP FUNCTION min_int32_le;
 DROP FUNCTION min_int64_be;
 DROP FUNCTION min_int64_le;
 DROP FUNCTION min_u_int32_be;
 DROP FUNCTION min_u_int32_le;
 DROP FUNCTION min_u_int64_be;
 DROP FUNCTION min_u_int64_le;
 DROP FUNCTION min_float_be;
 DROP FUNCTION min_float_le;
 DROP FUNCTION min_double_be;
 DROP FUNCTION min_double_le;
 

 DROP FUNCTION max_int32_be;
 DROP FUNCTION max_int32_le;
 DROP FUNCTION max_int64_be;
 DROP FUNCTION max_int64_le;
 DROP FUNCTION max_u_int32_be;
 DROP FUNCTION max_u_int32_le;
 DROP FUNCTION max_u_int64_be;
 DROP FUNCTION max_u_int64_le;
 DROP FUNCTION max_float_be;
 DROP FUNCTION max_float_le;
 DROP FUNCTION max_double_be;
 DROP FUNCTION max_double_le;
 
 */


#define DEFINE_ARRAY_FCT(name, return_type, array_type, convertion_fct, aggregation_fct, initial_value) \
my_bool name##_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {\
  if(args->arg_count != 1) {\
    strmov(message, "This function takes 1 argument");\
    return 1;\
  }\
  \
  if(args->args[0] != NULL) {\
    const u_int32_t len = args->lengths[0];\
    if(len % sizeof(array_type) != 0) {\
      char error[255];\
      snprintf(error, sizeof(error), "Bad length for an array of " #array_type ": %d is not a multiple of %lu", len, sizeof(array_type));\
      strmov(message, error);\
      return 1;\
    }\
  }\
  \
  if(args->arg_count) {\
    args->arg_type[0]= STRING_RESULT; /* Force argument to a String (a blob actually) */\
    args->maybe_null[0]=1;\
  }\
  \
  initid->maybe_null=1;\
    \
  return 0;\
}\
\
\
void name##_deinit(UDF_INIT *initid) {\
}\
\
return_type name(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,\
                  char *is_null, char *error) {\
  if(args->args[0] == NULL) {\
    *is_null=1;\
    return -1;\
  }\
  \
  const u_int32_t len = args->lengths[0];\
  const int intLen = len / sizeof(array_type);\
  \
  return_type result = initial_value;\
  \
  array_type* data = (array_type*)args->args[0]; \
   for(int i = 0; i<intLen; i+=1) {\
    array_type baseValue = data[i];\
    array_type realValue = convertion_fct(baseValue);\
    result = aggregation_fct(result, realValue);\
  }\
  \
  return result;\
}

#define sum_macro(a, b) ((a)+(b))
#define min_macro(a, b) ((a)>(b)?(b):(a))
#define max_macro(a, b) ((a)>(b)?(a):(b))


// be32toh, etc. fails for floats and doubles.
// So we define our own functions for this.
#define IDENTITY(a) (a)
float swap_float(float f) {
   float a;
   unsigned char *dst = (unsigned char *)&a;
   unsigned char *src = (unsigned char *)&f;

   dst[0] = src[3];
   dst[1] = src[2];
   dst[2] = src[1];
   dst[3] = src[0];

   return a;
}

double swap_double(double d) {
   double a;
   unsigned char *dst = (unsigned char *)&a;
   unsigned char *src = (unsigned char *)&d;
   
   dst[0] = src[7];
   dst[1] = src[6];
   dst[2] = src[5];
   dst[3] = src[4];
   dst[4] = src[3];
   dst[5] = src[2];
   dst[6] = src[1];
   dst[7] = src[0];

   return a;
}

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define float_letoh(a) IDENTITY(a)
#define float_betoh(a) swap_float(a)
#define double_letoh(a) IDENTITY(a)
#define double_betoh(a) swap_double(a)
#elif __BYTE_ORDER == __BIG_ENDIAN
#define float_betoh(a) IDENTITY(a)
#define float_letoh(a) swap_float(a)
#define double_betoh(a) IDENTITY(a)
#define double_letoh(a) swap_double(a)
#else
#error "Failed to determine endianness"
#endif

// sums
DEFINE_ARRAY_FCT(sum_int32_be, int64_t, int32_t, be32toh, sum_macro, 0)
DEFINE_ARRAY_FCT(sum_int32_le, int64_t, int32_t, le32toh, sum_macro, 0)

DEFINE_ARRAY_FCT(sum_int64_be, int64_t, int64_t, be64toh, sum_macro, 0)
DEFINE_ARRAY_FCT(sum_int64_le, int64_t, int64_t, le64toh, sum_macro, 0)

DEFINE_ARRAY_FCT(sum_u_int32_be, int64_t, u_int32_t, be32toh, sum_macro, 0)
DEFINE_ARRAY_FCT(sum_u_int32_le, int64_t, u_int32_t, le32toh, sum_macro, 0)

DEFINE_ARRAY_FCT(sum_u_int64_be, u_int64_t, u_int64_t, be64toh, sum_macro, 0)
DEFINE_ARRAY_FCT(sum_u_int64_le, u_int64_t, u_int64_t, le64toh, sum_macro, 0)

DEFINE_ARRAY_FCT(sum_float_be, double, float, float_betoh, sum_macro, 0)
DEFINE_ARRAY_FCT(sum_float_le, double, float, float_letoh, sum_macro, 0)

DEFINE_ARRAY_FCT(sum_double_be, double, double, double_betoh, sum_macro, 0)
DEFINE_ARRAY_FCT(sum_double_le, double, double, double_letoh, sum_macro, 0)

// mins
DEFINE_ARRAY_FCT(min_int32_be, int64_t, int32_t, be32toh, min_macro, INT_MAX)
DEFINE_ARRAY_FCT(min_int32_le, int64_t, int32_t, le32toh, min_macro, INT_MAX)

DEFINE_ARRAY_FCT(min_int64_be, int64_t, int64_t, be64toh, min_macro, LONG_LONG_MAX)
DEFINE_ARRAY_FCT(min_int64_le, int64_t, int64_t, le64toh, min_macro, LONG_LONG_MAX)

DEFINE_ARRAY_FCT(min_u_int32_be, int64_t, u_int32_t, be32toh, min_macro, UINT_MAX)
DEFINE_ARRAY_FCT(min_u_int32_le, int64_t, u_int32_t, le32toh, min_macro, UINT_MAX)

DEFINE_ARRAY_FCT(min_u_int64_be, u_int64_t, u_int64_t, be64toh, min_macro, ULONG_LONG_MAX)
DEFINE_ARRAY_FCT(min_u_int64_le, u_int64_t, u_int64_t, le64toh, min_macro, ULONG_LONG_MAX)

DEFINE_ARRAY_FCT(min_float_be, double, float, float_betoh, min_macro, INFINITY)
DEFINE_ARRAY_FCT(min_float_le, double, float, float_letoh, min_macro, INFINITY)

DEFINE_ARRAY_FCT(min_double_be, double, double, double_betoh, min_macro, INFINITY)
DEFINE_ARRAY_FCT(min_double_le, double, double, double_letoh, min_macro, INFINITY)

// maxes
DEFINE_ARRAY_FCT(max_int32_be, int64_t, int32_t, be32toh, max_macro, INT_MIN)
DEFINE_ARRAY_FCT(max_int32_le, int64_t, int32_t, le32toh, max_macro, INT_MIN)

DEFINE_ARRAY_FCT(max_int64_be, int64_t, int64_t, be64toh, max_macro, LONG_LONG_MIN)
DEFINE_ARRAY_FCT(max_int64_le, int64_t, int64_t, le64toh, max_macro, LONG_LONG_MIN)

DEFINE_ARRAY_FCT(max_u_int32_be, int64_t, u_int32_t, be32toh, max_macro, 0)
DEFINE_ARRAY_FCT(max_u_int32_le, int64_t, u_int32_t, le32toh, max_macro, 0)

DEFINE_ARRAY_FCT(max_u_int64_be, u_int64_t, u_int64_t, be64toh, max_macro, 0)
DEFINE_ARRAY_FCT(max_u_int64_le, u_int64_t, u_int64_t, le64toh, max_macro, 0)

DEFINE_ARRAY_FCT(max_float_be, double, float, float_betoh, max_macro, -INFINITY)
DEFINE_ARRAY_FCT(max_float_le, double, float, float_letoh, max_macro, -INFINITY)

DEFINE_ARRAY_FCT(max_double_be, double, double, double_betoh, max_macro, -INFINITY)
DEFINE_ARRAY_FCT(max_double_le, double, double, double_letoh, max_macro, -INFINITY)
