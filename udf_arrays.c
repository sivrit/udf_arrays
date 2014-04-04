 /* BSD-licensed (TODO more text) */

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


/****************************************************************************
** sum_int32_be - Compute the sum of an array of signed 32 bits big endian integers
**
** CREATE FUNCTION sum_int32_be RETURNS INTEGER SONAME "udf_arrays.so";
** DROP FUNCTION sum_int32_be;
**
****************************************************************************/


/**
 * 
 */
int64_t sum_int32_be(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
                  char *is_null, char *error) {
  if(args->args[0] == NULL) {
    *is_null=1;
    return -1;
  }
  
  const longlong len = args->lengths[0];
  
  if(len % 4 != 0) {
    strmov(error,"Argument should be an array of ints");
    return -1;
  }
  
  longlong intLen = len / 4;
  
  
  longlong sum = 0;
  
 
  u_int32_t* data = (u_int32_t*)args->args[0]; 
   for(int i = 0; i<intLen; i+=1) {
    u_int32_t baseValue = data[i];
    u_int32_t realValue = be32toh(baseValue);
    sum += realValue;
  }
  
  return sum;
}

my_bool sum_int32_be_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  if (args->arg_count != 1)
  {
    strmov(message,"This function takes 1 argument");
    return 1;
  }
  if (args->arg_count) {
    args->arg_type[0]= STRING_RESULT; /* Force argument to a String (a blob actually) */
    args->maybe_null[0]=1;
  }
    initid->maybe_null=1;
    
  return 0;
}


