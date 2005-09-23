#include "RTL/rtl.h"
#include "RTL/bits.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define Max(a,b) (a>b)?a:b;
#define Min(a,b) (a>b)?b:a;

int mask_and (const unsigned int* mask1,const unsigned int* mask2,unsigned int* mask3,int mask_size)   {
  int	    result=0;
  for (int i=0; i<mask_size; i++)  {
    mask3[i] = mask1[i] & mask2[i];
    result	|= mask3[i];
  }
  return result;
}

int mask_or (const unsigned int* mask1, const unsigned int* mask2,unsigned int* mask3,const int mask_size)   {
  int result = 0;
  for (int i=0;i<mask_size;i++)  {
    mask3[i] = mask1[i] | mask2[i];
    result	|= mask3[i];
  }
  return result;
}

int mask_and_ro (const unsigned int* mask1,const unsigned int* mask2,const int mask_size)   {
  int	    result=0;
  for (int i=0;i<mask_size;i++)  {
    result	|= (mask1[i] & mask2[i]);
  }
  return result;
}

int mask_or_ro (const unsigned int* mask1,const unsigned int* mask2, const int mask_size) {
  int result = 0;
  for (int i=0;i<mask_size;i++)  {
    result	|= (mask1[i] | mask2[i]);
  }
  return result;
}

int mask_summ(const unsigned int* mask, int mask_size)   {
  int result = 0;
  for (int i=0;i<mask_size;i++)  {
    result	|= mask[i] ;
  }
  return result;
}

int BF_alloc(char *base, int bf_size, int size_wanted, int* pos_found)   {
  bool set = false;
  for(int i=0, j=0, pos=0, len=0, nb=(bf_size/8); i < nb; ++i, j=0)  {
    for(int nbit=(i<nb) ? 8 : bf_size%8; j< nbit; ++j )  {
      if ( 0 == (base[i] & (1<<j)) )  {
        ++len;
        if ( set && len >= size_wanted )  {
          *pos_found = pos;
          return BF_set(base, pos, len);
        }
        else if ( !set )  {
          len = 1;
          set = true;
          pos = (i*8) + j;
        }
        continue;
      }
      set = false;
    }
  }
  *pos_found = bf_size;
  return 0;
}

int BF_count(const char* base,int bf_size,int* pos,int* size) {
  bool set = 0;  
  int max_pos = bf_size, max_len = 0, len = 0, start_pos = 0;
  for(int i=0, j=0; i < bf_size/8; ++i, j=0)  {
    if ( base[i] )  {
      for( ;j<8; ++j)  {
        if ( 0 == (base[i] & (1<<j)) )  {
          if ( !set )  {
            set = true;
            start_pos = (i*8) + j;
            len = 0;
          }
          ++len;
          continue;
        }
        if ( len > max_len )  {
          max_pos = start_pos;
          max_len = len;
        }
        len = 0;
        start_pos = (i*8) + j;
        set = false;
      }
    }
    else if ( set )  {
      len += 8;
      continue;
    }
    start_pos = 8*i;
    len = 8;
    set = true;
  }
  if ( len > max_len )  {
    max_pos = start_pos;
    max_len = len;
  }
  *pos    = max_pos;
  *size   = max_len;
  return 1;
}

int BF_set(char* base, int pos, int len)   {
  base += pos/8;
  int bit = pos%8;
  if ( bit > 0 )  {
    int j=(len-bit>=8) ? 8 : len+bit;
    for(int k=bit; k<j; ++k)
      *base |= (1<<k);
    len -= j-bit;
    ++base;
  }
  if ( len/8 )  {
    int nb = len/8;
    len   -= nb*8;
    ::memset(base,0xFF,nb);
    base += nb;
  }
  for(int m=0; m<len;++m)
    *base |= (1<<m);
  return 1;
}

int BF_free(char* base,int pos, int len) {
  base += pos/8;
  int bit = pos%8;
  if ( bit > 0 )  {
    int j=(len-bit>=8) ? 8 : len+bit;
    for(int k=bit; k<j; ++k)
      *base &= ~(1<<k);
    len -= j-bit;
    ++base;
  }
  if ( len/8 )  {
    int nb = len/8;
    len -=  nb*8;
    ::memset(base,0x0,nb);
    base += nb;
  }
  for(int m=0; m<len;++m)
    *base &= ~(1<<m);
  return 1;
}

static inline int generic_ffs(int x)  {
	int r = 1;
	if (!x)
    return 0;
	if (!(x & 0xffff)) {
		x >>= 16;
		r += 16;
	}
	if (!(x & 0xff)) {
		x >>= 8;
		r += 8;
	}
	if (!(x & 0xf)) {
		x >>= 4;
		r += 4;
	}
	if (!(x & 3)) {
		x >>= 2;
		r += 2;
	}
	if (!(x & 1)) {
		x >>= 1;
		r += 1;
	}
	return r;
}

static inline int generic_ffc(int x)  {
	int r = 1;
	if (!x)
    return 0;
	if (!(x & 0xffff)) {
		x >>= 16;
		r += 16;
	}
	if (!(x & 0xff)) {
		x >>= 8;
		r += 8;
	}
	if (!(x & 0xf)) {
		x >>= 4;
		r += 4;
	}
	if (!(x & 3)) {
		x >>= 2;
		r += 2;
	}
	if (!(x & 1)) {
		x >>= 1;
		r += 1;
	}
	return r;
}
#ifdef _WIN32
#define ffs generic_ffs
#define ffc generic_ffc
#endif
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

int lib_rtl_ffc (int* start, int* len, const void* base, int* position)  {
  unsigned int i, j, v;
  char* b = ((char*)base) + ((*start)/8);
  for (i=*start, j=i+*len, v=*(unsigned int*)b; i<j; ++i)  {
    if ( (v & mask[i%32]) == 0 )  {
      *position = i;
      return 1;
    }
  }
  *position = i;
  return i>31 ? 0 : 1;
}

int lib_rtl_ffs (int* pos, int* size, const void* base, int* ret)  {
  int retval;
  int val= *(int*)base;
  if ((*pos)<=32) {
    if (*pos)
      val&=~(1<<((*pos)-1));
    retval=ffs(val);
    if (retval==0) {
      *ret=*size;
      return 0;
    }
    retval--;
  } else {
    printf("ffs retval for > 32 bit size not yet implemented");
    return 0;
  }
  if (retval>=(*size)) {
    *ret=*size;
    return 0;
  }
  *ret=retval;
  return 1;
#if 0
  char* b = ((char*)base) + ((*start)/8);
  for (unsigned int i=*start, j=i+*len, v=*(unsigned int*)b; i<j; ++i)  {
    if ( (v & mask[i%32]) != 0 )  {
      *position = i;
      return 1;
    }
  }
  *position = i;
  return i>31 ? 0 : 1;
#endif
}
