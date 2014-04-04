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
 */

/*
create all functions:
 CREATE FUNCTION sum_int32_be RETURNS INTEGER SONAME "udf_arrays.so";
 CREATE FUNCTION sum_int32_le RETURNS INTEGER SONAME "udf_arrays.so";

drop all functions:
 DROP FUNCTION sum_int32_be;
 DROP FUNCTION sum_int32_le;
 */


#define DEFINE_ARRAY_FCT(name, return_type, array_type, convertion_fct, aggregation_fct) \
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
      snprintf(error, sizeof(error), "Bad length for an array of ##array_type##: %d is not a multiple of %lu", len, sizeof(array_type));\
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
  return_type result = 0;\
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

DEFINE_ARRAY_FCT(sum_int32_be, int64_t, int32_t, be32toh, sum_macro)
DEFINE_ARRAY_FCT(sum_int32_le, int64_t, int32_t, le32toh, sum_macro)
