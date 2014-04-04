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

#include "udf_arrays.c"

#include <stdio.h>
#include <string.h>
#include "CUnit/Basic.h"

// input data is named as "sample_TYPE_ENDIANNESS_GROUP"
// group being A, B or C

// Sample A is { 0, 1, 2 }
char sample_int32_be_A[] = {0x00,  0x00,  0x00,  0x00,
                            0x00,  0x00,  0x00,  0x01,
                            0x00,  0x00,  0x00,  0x02};
char sample_int32_le_A[] = {0x00,  0x00,  0x00,  0x00,
                            0x01,  0x00,  0x00,  0x00,
                            0x02,  0x00,  0x00,  0x00};

char sample_int64_be_A[] = {0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
                            0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x01,
                            0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x02};
char sample_int64_le_A[] = {0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, 
                            0x01,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, 
                            0x02,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00};

// Sample B is { 3, -4, -1 }, or { 3, 4, 1 } for unsigned data
char sample_int32_be_B[] = {0x00,  0x00,  0x00,  0x03,
                            0xFF,  0xFF,  0xFF,  0xFC, 
                            0xFF,  0xFF,  0xFF,  0xFF};
char sample_int32_le_B[] = {0x03,  0x00,  0x00,  0x00,
                            0xFC,  0xFF,  0xFF,  0xFF, 
                            0xFF,  0xFF,  0xFF,  0xFF};

char sample_int64_be_B[] = {0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x03, 
                            0xFF,  0xFF,  0xFF,  0xFF,  0xFF,  0xFF,  0xFF,  0xFC, 
                            0xFF,  0xFF,  0xFF,  0xFF,  0xFF,  0xFF,  0xFF,  0xFF};
char sample_int64_le_B[] = {0x03,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, 
                            0xFC,  0xFF,  0xFF,  0xFF,  0xFF,  0xFF,  0xFF,  0xFF, 
                            0xFF,  0xFF,  0xFF,  0xFF,  0xFF,  0xFF,  0xFF,  0xFF};

// Sample C is {min, max} value (ex: -2147483648 and 2147483647)
char sample_int32_be_C[] = {0x80,  0x00,  0x00,  0x00, 
                            0x7F,  0xFF,  0xFF,  0xFF};
char sample_int32_le_C[] = {0x00,  0x00,  0x00,  0x80, 
                            0xFF,  0xFF,  0xFF,  0x7F};

char sample_int64_be_C[] = {0x80,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, 
                            0x7F,  0xFF,  0xFF,  0xFF,  0xFF,  0xFF,  0xFF,  0xFF};
char sample_int64_le_C[] = {0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x80,
                            0xFF,  0xFF,  0xFF,  0xFF,  0xFF,  0xFF,  0xFF,  0x7F};


// Macros to avoid too much copy paste.
// tests will be named test_FUNCTION_GROUP
// FUNCTION being the name of the function being tested
// GROUP being the group of input values to use (A, B, C or NULL)

// Define a test checking that "function" returns "expected" of type "result_type" when applied to "input"
#define DEFINE_TEST_WITH_EXPECT(function, group, result_type, input, expected) \
void test_ ## function ## _ ## group(void) {\
  UDF_INIT initid;\
  UDF_ARGS args;\
  char error[1];\
  char isNull[1];\
  \
  char* args2[1] = { input };\
  args.args=args2;\
  ulong lengths[1] = {sizeof( input )};\
  args.lengths = lengths;\
  \
  result_type result = function(&initid, &args, isNull, error);\
  \
  CU_ASSERT_EQUAL(*isNull, 0);\
  CU_ASSERT_EQUAL(result, expected);\
}

// Define a test checking that "function" returns NULL for a NULL input
#define DEFINE_TEST_NULL_ARG(function) \
void test_ ## function ## _NULL(void) {\
  UDF_INIT initid;\
  UDF_ARGS args;\
  char error[1];\
  char isNull[1];\
  \
  char* args2[1] = { NULL };\
  args.args=args2;\
  ulong lengths[1] = { 0 };\
  args.lengths = lengths;\
  \
  function(&initid, &args, isNull, error);\
  \
  CU_ASSERT_EQUAL(*isNull, 1);\
}

// Define all tests (including NULL input and both endianness) for an operation (op: sum, max...) and a value "type" (int32, int64, etc).
// "result_type" is the return type of the UDF. resA, resB and resC are the expected results for each group of test input values.
#define DEFINE_TESTS(op, type, result_type, resA, resB, resC) \
DEFINE_TEST_WITH_EXPECT(op ## _ ## type ## _be, A, result_type, sample_ ## type ## _be_A,  resA)\
DEFINE_TEST_WITH_EXPECT(op ## _ ## type ## _be, B, result_type, sample_ ## type ## _be_B,  resB)\
DEFINE_TEST_WITH_EXPECT(op ## _ ## type ## _be, C, result_type, sample_ ## type ## _be_C,  resC)\
\
DEFINE_TEST_WITH_EXPECT(op ## _ ## type ## _le, A, result_type, sample_ ## type ## _le_A,  resA)\
DEFINE_TEST_WITH_EXPECT(op ## _ ## type ## _le, B, result_type, sample_ ## type ## _le_B,  resB)\
DEFINE_TEST_WITH_EXPECT(op ## _ ## type ## _le, C, result_type, sample_ ## type ## _le_C,  resC)\
\
DEFINE_TEST_NULL_ARG(op ## _ ## type ## _be)\
DEFINE_TEST_NULL_ARG(op ## _ ## type ## _le)

DEFINE_TESTS(sum, int32, int64_t, 3, -2, -1)
DEFINE_TESTS(sum, int64, int64_t, 3, -2, -1)


#define STR(a) #a
#define DECLARE_TESTS(fct, group) \
  { STR(fct ## _le), test_ ## fct ## _be_ ## group },\
  { STR(fct ## _be), test_ ## fct ## _le_ ## group },


CU_TestInfo tests_input_A[] = {
  DECLARE_TESTS(sum_int32, A)
  DECLARE_TESTS(sum_int64, A)
  CU_TEST_INFO_NULL,
};

CU_TestInfo tests_input_B[] = {
  DECLARE_TESTS(sum_int32, B)
  DECLARE_TESTS(sum_int64, B)
  CU_TEST_INFO_NULL,
};

CU_TestInfo tests_input_C[] = {
  DECLARE_TESTS(sum_int32, C)
  DECLARE_TESTS(sum_int64, C)
  CU_TEST_INFO_NULL,
};

CU_TestInfo tests_input_NULL[] = {
  DECLARE_TESTS(sum_int32, NULL)
  DECLARE_TESTS(sum_int64, NULL)
  CU_TEST_INFO_NULL,
};

CU_SuiteInfo suites[] = {
  { "Test on input { 0,  1,  2 }",  NULL, NULL, tests_input_A },
  { "Test on input { 3, -4, -1 }",  NULL, NULL, tests_input_B },
  { "Test on input { MIN, MAX }",   NULL, NULL, tests_input_C },
  { "Test with NULL input",         NULL, NULL, tests_input_NULL },
  CU_SUITE_INFO_NULL,
};

int main()
{
   /* initialize the CUnit test registry */
   if (CU_initialize_registry() != CUE_SUCCESS)
      return CU_get_error();

   /* add the tests to the suite */
   if (CU_register_suites(suites) != CUE_SUCCESS) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* Run all tests using the CUnit Basic interface */
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   CU_cleanup_registry();
   return CU_get_error();
}
