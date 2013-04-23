#include <cstdio>
#include <iostream>
#include <iomanip>
#include "RTL/rtl.h"
#include "RTL/bits.h"

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
      unsigned int val[2] = {mask[i], 0};
      int len = 32-start;
      int pos = 0;
      int s = start;
      int sc = lib_rtl_ffc (&s,&len,val,&pos);
      printf("FFC: %2ld  %2d  sc=%d %08X  %2d\t\t\t\t", long(i), s, sc, val[0], pos);
      val[0] = ~val[0];
      s = start;
      sc = lib_rtl_ffs (&s,&len,val,&pos);
      printf("FFX: %2ld  %2d  sc=%d %08X  %2d\n", long(i), s, sc, val[0], pos);
    }
  }
  return 0;
}

extern "C" int rtl_testbits(int,char **) {
  int pos = 0, size = 15;
  char txt[32*sizeof(int)];
  int bf_len = 8*sizeof(txt);
  ::memset(txt,0,sizeof(txt));
  ::printf("Filed size: %ld bits\n",long(sizeof(txt)*8));
  BF_set(txt,20,96);
  ::printf("BF_set(txt,20,%d);\n",96);
  BF_print(txt,sizeof(txt));
  BF_count(txt,sizeof(txt),&pos,&size);
  ::printf("BFCount: pos:%d size:%d\n",pos,size);
  BF_count(txt+8,bf_len-8*8,&pos,&size);
  ::printf("BFCount: start:%d pos:%d size:%d  %d\n",8*8,pos,size, size+8*8+pos);
  BF_count(txt+16,bf_len-16*8,&pos,&size);
  ::printf("BFCount: start:%d pos:%d size:%d  %d\n",16*8,pos,size, size+16*8+pos);
  BF_count(txt+32,bf_len-32*8,&pos,&size);
  ::printf("BFCount: start:%d pos:%d size:%d  %d\n",32*8,pos,size,size+32*8+pos);

  BF_free(txt,21,94);
  printf("BF_free(txt,21,%d);\n",3*12);
  BF_print(txt,sizeof(txt));

  BF_alloc(txt,bf_len,256,&pos);
  printf("BF_alloc(txt,...,256,%d);\n",pos);
  BF_print(txt,sizeof(txt));

  BF_alloc(txt,bf_len,11,&pos);
  printf("BF_alloc(txt,bf_len,11,%d);\n",pos);
  BF_print(txt,sizeof(txt));
  BF_free(txt,pos,11);
  printf("BF_free(txt,%d,11);\n",pos);
  BF_print(txt,sizeof(txt));

  BF_alloc(txt,bf_len,11+32,&pos);
  printf("BF_alloc(txt,...,11+32,%d);\n",pos);
  BF_print(txt,sizeof(txt));

  BF_alloc(txt,bf_len,32,&pos);
  printf("BF_alloc(txt,...,32,%d);\n",pos);
  BF_print(txt,sizeof(txt));
  return 0;
}

extern "C" int rtl_testbits2(int,char **) {
#define VALSIZE 1000
  unsigned long long randvals[VALSIZE];
  int i, j;
  int pos, size;

  for(i=0;i<VALSIZE;i++) {
    // randvals[i] = (((unsigned long long )rand())  << 32)| rand();
    randvals[i] = 0xe000000000000001LL;
    //cout << randvals[i] << "  ";
  }

  for(i=0;i<100000;i++) {
    for(j=0;j<VALSIZE;j++) {
      if(!BF_count((const char*)&randvals[j], sizeof(long long) * 8, &pos, &size)) {
        std::cout << "Problem!!" << std::endl;
        exit(-1);
      } else if(i==0) {
        std::vector<std::string> words;
        Bits::dumpWords((const char*)&randvals[j], sizeof(long long) * 8,words);
        std::cout << std::setw(16) << randvals[j]
          << " val," << std::setw(2) << pos << ":" << std::setw(2) << size << " "
          << words[0] << "." << words[1] << std::endl;
      }
    }
  }
  return 0;
}
