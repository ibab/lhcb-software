#include <cstdio>
#include "RTL/rtl.h"

extern "C" int rtl_testffx(int,char **) {
  static unsigned int mask[] = {
    0x00000001, 0x00000002, 0x00000004, 0x00000008,
    0x00000010, 0x00000020, 0x00000040, 0x00000080,
    0x00000100, 0x00000200, 0x00000400, 0x00000800, 
    0x00001000, 0x00002000, 0x00004000, 0x00008000, 
    0x00010000, 0x00020000, 0x00040000, 0x00080000, 
    0x00100000, 0x00200000, 0x00400000, 0x00800000, 
    0x01000000, 0x02000000, 0x04000000, 0x08000000, 
    0x10000000, 0x20000000, 0x40000000, 0x80000000
  };
  for (size_t i=0; i<sizeof(mask)/sizeof(mask[0]); ++i)  {
    for (int start=0; start < 32; ++start)  {
      int val = mask[i];
      int len = 32-start;
      int pos = 0;
      int s = start;
      int sc = lib_rtl_ffc (&s,&len,&val,&pos);
      printf("FFC: %2d  %2d  sc=%d %08X  %2d\t\t\t\t", i, s, sc, val, pos);
      val = ~val;
      s = start;
      sc = lib_rtl_ffs (&s,&len,&val,&pos);
      printf("FFX: %2d  %2d  sc=%d %08X  %2d\n", i, s, sc, val, pos);
    }
  }
  return 0;
}
