#include "RTL/rtl.h"
int sse_bit_test0();
int sse_bit_test3();
int avx_bit_test0();
int avx_bit_test3();

int main(int, char**) {
  const char* line="-----------------------------------------------------";
#ifndef RUN_TESTS
#define RUN_TESTS 0
#endif
#if RUN_TESTS>=1
  ::lib_rtl_output(LIB_RTL_INFO,"++ Executing test: %s   sse_bit_test0  -----------------\n",line);
  sse_bit_test0();
#endif
#if RUN_TESTS>=2
  ::lib_rtl_output(LIB_RTL_INFO,"++ Executing test: %s   avx_bit_test0  -----------------\n",line);
  avx_bit_test0();
#endif
#if RUN_TESTS>=3
  ::lib_rtl_output(LIB_RTL_INFO,"++ Executing test: %s   sse_bit_test3  -----------------\n",line);
  sse_bit_test3();
#endif
#if RUN_TESTS>=4
  ::lib_rtl_output(LIB_RTL_INFO,"++ Executing test: %s   avx_bit_test   -----------------\n",line);
  avx_bit_test3();
#endif
  return 1;
}
