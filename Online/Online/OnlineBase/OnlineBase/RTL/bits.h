#ifndef BUFFERMANAGER_BITS_H
#define BUFFERMANAGER_BITS_H
#ifdef __cplusplus
#include <cstdlib>
#include <cstring>
#include <climits>
extern "C" {
#else
#include <stdio.h>
#include <string.h>
#include <limits.h>
#endif
  inline int bit_set(unsigned int* mask,int pos)  {
    mask[pos/32] |= (1<<(pos%32));
    return 0;
  }

  inline int bit_clear(unsigned int* mask,int pos)  {
    mask[pos/32] &= ~(1<<(pos%32));
    return 0;
  }

  inline int bit_test (const unsigned int *mask,int pos)    {
    return (mask[pos/32] & (1<<(pos%32)));
  }
  int mask_and2   (const unsigned int* mask1,const unsigned int* mask2,unsigned int* mask3,int mask_size);
  int mask_and3   (const unsigned int* mask1,const unsigned int* mask2,const unsigned int* mask3,unsigned int* mask4,int mask_size);
  int mask_or2    (const unsigned int* mask1,const unsigned int* mask2,unsigned int* mask3,const int mask_size);
  int mask_or3    (const unsigned int* mask1,const unsigned int* mask2,const unsigned int* mask3,unsigned int* mask4,const int mask_size);
  int mask_and_ro2(const unsigned int* mask1,const unsigned int* mask2,const int mask_size);
  int mask_and_ro3(const unsigned int* mask1,const unsigned int* mask2,const unsigned int* mask3,const int mask_size);
  int mask_or_ro2 (const unsigned int* mask1,const unsigned int* mask2,const int mask_size);
  int mask_or_ro3 (const unsigned int* mask1,const unsigned int* mask2,const unsigned int* mask3, const int mask_size);
  int mask_or_ro4 (const unsigned int* mask1,const unsigned int* mask2,const unsigned int* mask3,const unsigned int* mask4,const int mask_size);
  int mask_summ   (const unsigned int* mask, int mask_size);
  int BF_alloc    (char *base, int bf_size,int size_wanted,int* pos_found);
  int BF_count    (const char* base,int bf_size,int* pos,int* size);
  int BF_set      (char* base, int start,int end);
  int BF_free     (char* base,int pos, int size);
  void BF_print   (const void* base, int bf_size, size_t ncols=4, bool prt_hdr=true);
#ifdef __cplusplus
}
#include <vector>
#include <string>
#include "RTL/Pack.h"
namespace Bits  {
  void dumpWords(const void* field, int len, std::vector<std::string>& words);
  template <int i> struct BitMask  {
    unsigned int m_mask[i];
    /// Default bitfield constructor
    BitMask() {
      ::memset(m_mask, 0, i * sizeof(m_mask[0]));
    }
    /// Initializing bitfield constructor
    explicit BitMask(int def) {
      for(int j=0; j<i;++j) m_mask[j]=def;
    }
    /// Number of digits for the bitfield
    static unsigned int digits() {
      return i*sizeof(unsigned int)*CHAR_BIT;
    }
    /// Number of 32 bit words in the mask
    int words()  const {
      return i;
    }
    /// Clear bitfield
    void clear()  {
      ::memset(m_mask, 0, i * sizeof(m_mask[0]));
    }
    int mask_or(const BitMask<i>& mask)  const   {
      return ::mask_or_ro2(m_mask, mask.m_mask, i);
    }
    int mask_or(const BitMask<i>& mask1, const BitMask<i>& mask2)  const   {
      return ::mask_or_ro3(m_mask, mask1.m_mask, mask2.m_mask, i);
    }
    int mask_or(const BitMask<i>& mask1, const BitMask<i>& mask2, const BitMask<i>& mask3)  const   {
      return ::mask_or_ro4(m_mask, mask1.m_mask, mask2.m_mask, mask3.m_mask, i);
    }
    int mask_summ()  const    {
      return ::mask_summ(m_mask,i);
    }
    int mask_and(const BitMask<i>& mask1, const BitMask<i>& mask2)  {
      return ::mask_and2(mask1.m_mask, mask2.m_mask, m_mask, i);
    }
    int mask_and(const BitMask<i>& mask1, const BitMask<i>& mask2, const BitMask<i>& mask3)  {
      return ::mask_and3(mask1.m_mask, mask2.m_mask, mask3.m_mask, m_mask, i);
    }
    void set(int which)  {
      ::bit_set(m_mask,which);
    }
    void clear(int which)  {
      ::bit_clear(m_mask,which);
    }
    int test(int which) const  {
      return ::bit_test(m_mask,which);
    }
    BitMask& operator=(const unsigned int val[i]) {
      for(int j=0; j<i;++j) m_mask[j]=val[j];
      return *this;
    }
    BitMask& operator=(const BitMask<i>& val) {
      for(int j=0; j<i;++j) m_mask[j]=val.m_mask[j];
      return *this;
    }
    bool operator==(const unsigned int val[i]) {
      return ::memcmp(val,m_mask,i*sizeof(m_mask[0])) == 0;
    }
    bool operator==(const BitMask<i>& val) {
      return ::memcmp(val.m_mask,m_mask,i*sizeof(m_mask[0])) == 0;
    }
    bool operator!=(const unsigned int val[i]) {
      return ::memcmp(val,m_mask,i*sizeof(m_mask[0])) != 0;
    }
    bool operator!=(const BitMask<i>& val) {
      return ::memcmp(val.m_mask,m_mask,i*sizeof(m_mask[0])) != 0;
    }
    const unsigned int* bits()  const  {
      return m_mask;
    }
    unsigned int word(int which)  const  {
      return m_mask[which];
    }
    void setWord(int which, int value)  {
      m_mask[which] = value;
    }
  };
}
#include "RTL/Unpack.h"
#endif

#endif // BUFFERMANAGER_BITS_H
