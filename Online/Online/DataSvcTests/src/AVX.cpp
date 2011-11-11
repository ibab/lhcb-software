//#include "RTL/rtl.h"
#include "AVX.h"
#include "GaudiKernel/PointerHelpers.h"
using namespace Framework;
using namespace std;

#ifdef _WIN32
#include "mmintrin.h"
// warning C4035: 'Framework::AVXMemory::first_set_int32' : no return value
#pragma warning ( disable : 4035 )
#else
#include <smmintrin.h>
#include <tmmintrin.h>
#include <cstring>
#endif
#include <iostream>
#include <iomanip>
#include <climits>
#include <ctime>

#define _USE_SSE2 1
//#define _USE_MMX

enum { SSE2_ALIGNMENT=0xf };
#define SSE2_WORD(x) (x>>7)
#define SSE2_BIT(x)  (x&0x7F)

#define MMX_WORD     (x>>6)

#define W_32  (CHAR_BIT*sizeof(int))
#define MASK128_ASSIGN(x,y) x=y
//#define MASK128_ASSIGN(x,y) _mm_stream_si128(&x,y)

#define MASK_IS_SET(x) (_mm_movemask_epi8(_mm_cmpeq_epi8(x,_mm_setzero_si128())) != 0xffff)
#define MASK128_AND(i)    (first[i]=_mm_and_si128(first[i],mask.first[i]))
#define MASK128_OR(i)     (first[i]=_mm_or_si128(first[i],mask.first[i]))
#ifdef _WIN32
#if 0
static inline bool MASK128_MATCH(__m128i a, __m128i b)  {
  return 0xffff == _mm_movemask_epi8(_mm_cmpeq_epi16(_mm_and_si128(a,b),b));
}
#endif
#define MASK128_MATCH(a,b)         (0xffff == _mm_movemask_epi8(_mm_cmpeq_epi16(_mm_and_si128(a,b),b)))
#define MASK128_EXTRACT_UINT(x,N)  x.m128i_u32[N]
#else
// Note: SSE4 Only!!
#define MASK128_MATCH(a,b)           (_mm_testc_si128(a,b) == 1)
#define MASK128_EXTRACT_UINT(x,N)  (_mm_extract_epi32(x,N))
//#define MASK128_EXTRACT_UINT(x,N)    (((int*)&x)[N])
#endif

namespace {

  static size_t s_POS[32] = {
    0,   1, 28,  2, 29, 14, 24, 3,
    30, 22, 20, 15, 25, 17,  4, 8,
    31, 27, 13, 23, 21, 19, 16, 7,
    26, 12, 18,  6, 11,  5, 10, 9
  }; 
  struct _Init {
    __m128i Zero2ThreeShift5;// = _mm_set_epi32(0<<5,1<<5,2<<5,3<<5);
    __m128i search;// = _mm_set_epi32(0x077CB531,0x077CB531,0x077CB531,0x077CB531);
    __m128i one;//    = _mm_cmpeq_epi8(_mm_setzero_si128(),_mm_setzero_si128());
    __m128i zero;
    __m128i bit128[AVXMemory::SSE2_BITS+4];
    __m128i not_bit128[AVXMemory::SSE2_BITS+4];
    _Init()  {
      // This is a bit tricky: If we use only one loop, the optimizer does bad things
      for(size_t i=0; i<AVXMemory::SSE2_BITS+4; ++i)   {
    #ifdef _USE_SSE2
        MASK128_ASSIGN(bit128[i],_mm_setzero_si128());
        MASK128_ASSIGN(not_bit128[i],_mm_cmpeq_epi8(_mm_setzero_si128(),_mm_setzero_si128()));
    #elif defined(_USE_MMX)
        bit128[i].m128i_u64[0] = bit128[i].m128i_u64[1] = 0;
    #else
        unsigned int* ui = (unsigned int*)&(bit128[i]);
        unsigned int* vi = (unsigned int*)&(not_bit128[i]);
        ui[0] = ui[1] = ui[2] = ui[3] = 0;
    #endif
      }
      for(size_t i=0; i<AVXMemory::SSE2_BITS; ++i)   {
        unsigned int* ui = (unsigned int*)&(bit128[i]);
        unsigned int* vi = (unsigned int*)&(not_bit128[i]);
        ui[i/W_32] |=  (1<<(i%W_32));
        vi[i/W_32] &= ~(1<<(i%W_32));
      }
      MASK128_ASSIGN(Zero2ThreeShift5, _mm_set_epi32(0<<5,1<<5,2<<5,3<<5));
      MASK128_ASSIGN(search, _mm_set_epi32(0x077CB531,0x077CB531,0x077CB531,0x077CB531));
      MASK128_ASSIGN(one,    _mm_cmpeq_epi8(_mm_setzero_si128(),_mm_setzero_si128()));
      MASK128_ASSIGN(zero,   _mm_setzero_si128());
    }
  };
  static _Init s;
}

static inline string string_i128(const __m128i v) {
  const unsigned int* q = (const unsigned int*)&v;
  char text[128];
  ::snprintf(text,sizeof(text),"%8X %8X %8X %8X",q[0],q[1],q[2],q[3]);
  return text;
}

static inline void dump_i128(const string& txt, const __m128i v) {
  cout << setw(32) << left << txt << ": " << hex << string_i128(v) << endl;
}

const AVXMemory::Registers AVXMemory::SSE_bits() {
  return Registers(s.bit128,SSE2_BITS*SSE2_BITS);
}

const AVXMemory::Registers AVXMemory::SSE_not_bits() {
  return Registers(s.bit128,SSE2_BITS*SSE2_BITS);
}

void AVXMemory::zero()   {
  size_t wrd = second/SSE2_BITS;
  switch(wrd)  {
    default:
      for(size_t i=5; i<wrd;++i) 
        MASK128_ASSIGN(first[i],_mm_setzero_si128());
    case 6:
      MASK128_ASSIGN(first[5],_mm_setzero_si128());
    case 5:
      MASK128_ASSIGN(first[4],_mm_setzero_si128());
    case 4:
      MASK128_ASSIGN(first[3],_mm_setzero_si128());
    case 3:
      MASK128_ASSIGN(first[2],_mm_setzero_si128());
    case 2:
      MASK128_ASSIGN(first[1],_mm_setzero_si128());
    case 1:
    case 0:
      MASK128_ASSIGN(first[0],_mm_setzero_si128());
      break;
  }
}

bool AVXMemory::empty()   const   {
  size_t wrd = second/SSE2_BITS;
  switch(wrd)  {
    default:
      for(size_t i=5; i<wrd;++i)
        if ( MASK_IS_SET(first[i]) ) return false;
    case 6:  if ( MASK_IS_SET(first[5]) ) return false;
    case 5:  if ( MASK_IS_SET(first[4]) ) return false;
    case 4:  if ( MASK_IS_SET(first[3]) ) return false;
    case 3:  if ( MASK_IS_SET(first[2]) ) return false;
    case 2:  if ( MASK_IS_SET(first[1]) ) return false;
    case 1:
    case 0:  if ( MASK_IS_SET(first[0]) ) return false;
  }
  return true;
}

/// Copy mask content
void AVXMemory::assign(const Registers& bits)   {
  size_t wrd = bits.second<second ? bits.second/SSE2_BITS : second/SSE2_BITS;
  switch(wrd)  {
    default:
      for(size_t i=5; i<wrd;++i) MASK128_ASSIGN(first[i], bits.first[i]);
    case 6: MASK128_ASSIGN(first[5], bits.first[5]);
    case 5: MASK128_ASSIGN(first[4], bits.first[4]);
    case 4: MASK128_ASSIGN(first[3], bits.first[3]);
    case 3: MASK128_ASSIGN(first[2], bits.first[2]);
    case 2: MASK128_ASSIGN(first[1], bits.first[1]);
    case 1:
    case 0: MASK128_ASSIGN(first[0], bits.first[0]);
  }
}

bool AVXMemory::match(const AVXMemory& bits)  const    {
  size_t wrd = bits.second<second ? bits.second/SSE2_BITS : second/SSE2_BITS;
  switch(wrd)  {
    default:
      for(size_t i=5; i<wrd;++i) if ( !MASK128_MATCH(first[i], bits.first[i]) ) return false;
    case 6: return MASK128_MATCH(first[0], bits.first[0]) && MASK128_MATCH(first[1], bits.first[1]) && MASK128_MATCH(first[2], bits.first[2]) && MASK128_MATCH(first[3], bits.first[3]) && MASK128_MATCH(first[4], bits.first[4]) && MASK128_MATCH(first[5], bits.first[5]);
    case 5: return MASK128_MATCH(first[0], bits.first[0]) && MASK128_MATCH(first[1], bits.first[1]) && MASK128_MATCH(first[2], bits.first[2]) && MASK128_MATCH(first[3], bits.first[3]) && MASK128_MATCH(first[4], bits.first[4]);
    case 4: return MASK128_MATCH(first[0], bits.first[0]) && MASK128_MATCH(first[1], bits.first[1]) && MASK128_MATCH(first[2], bits.first[2]) && MASK128_MATCH(first[3], bits.first[3]);
    case 3: return MASK128_MATCH(first[0], bits.first[0]) && MASK128_MATCH(first[1], bits.first[1]) && MASK128_MATCH(first[2], bits.first[2]);
    case 2: return MASK128_MATCH(first[0], bits.first[0]) && MASK128_MATCH(first[1], bits.first[1]);
    case 1:
    case 0: return MASK128_MATCH(first[0], bits.first[0]);
  }
}

static inline unsigned int msb(register unsigned int x)    {
  x |= (x >> 1);
  x |= (x >> 2);
  x |= (x >> 4);
  x |= (x >> 8);
  x |= (x >> 16);
  return s_POS[((x & ~(x >> 1)) * 0x077CB531UL) >> 27];
}

#ifdef _WIN32
static inline int __myffs(int x)  {
   __asm
   {
      mov eax, x    ; Get first argument
      bsf eax, eax  ;
   }
   /* Return with result in EAX */
}
#endif


// ------------------------------------------------------------------------------
// __x86_64__: I measured: 
//
// ~540 msec for ~45 mio calls using the assembler 
// ~630 msec -- " -- using SSE2
// ~730 msec -- " -- using the integer log2 shifting
//
// On WIN32, the SSE solution is by far slowest. 
// Assembler and log2 are about the same.
//
// ------------------------------------------------------------------------------

/// Search the first free bit
size_t AVXMemory::first_set_int32()  const    {
#if defined(__x86_64__) && defined(__linux__)
  unsigned long long x, *p = (unsigned long long*)first;
  for(size_t i=0, wrd=second>>6; i<wrd; ++i )  {
    if ( p[i] )  {
      // unsigned long long int z = p[i];
      x = p[i];
      __asm__("bsfq %1, %0;\n\t"
	      : "=r" (x)
	      : "r" (x)
	      : 
	      );
      //printf ("wrd:%4d   -- value: %16llX bit: %16llX %8d -- ffsll: %8d\n",i,z,x,x,ffsll(z));
      return (i<<6) + x;
    }
  }
#else

#if UINT_MAX==ULONG_MAX
  unsigned int *p = (unsigned int*)first;
  for(size_t i=0, wrd=second>>5; i<wrd; ++i )  {
    if ( p[i] )  {
#if defined(_WIN32)
      unsigned int x = p[i];
      __asm
      {
        mov eax, x    ; Get first argument
        bsf eax, eax  ; Get first bit set
        mov ebx, i    ; calculate i<<5 in ebx
        shl ebx, 5h   ;
        add eax, ebx  ; add to result and return
      }
      return;
      //return (i<<5) + __myffs(p[i]);
#else
      unsigned int x = p[i];
      // Note: this is only 20 % slower than the above assembly code!
      x |= (x >> 1);
      x |= (x >> 2);
      x |= (x >> 4);
      x |= (x >> 8);
      x |= (x >> 16);
      return (i<<5) + s_POS[((x & ~(x >> 1)) * 0x077CB531UL) >> 27];
#endif
    }
  }
#else
  unsigned int *p = (unsigned int*)first;
  for(unsigned int i=0, wrd=second>>5; i<wrd; ++i )  {
    if ( p[i] )  {
      unsigned int x = p[i];
      x |= (x >> 1);
      x |= (x >> 2);
      x |= (x >> 4);
      x |= (x >> 8);
      x |= (x >> 16);
      return (i<<5) + s_POS[((x & ~(x >> 1)) * 0x077CB531) >> 27];
    }
  }
#endif
#endif
  return NO_BIT_SET;
}

/// Search the first free bit using SSE instructions
size_t AVXMemory::first_set_SSE()  const    {
#if defined(_USE_SSE2) || defined(_WIN32)
  register __m128i x, y;//, z;
  for(size_t i=0, wrd=second>>7; i<wrd; ++i )    {
    MASK128_ASSIGN(x,first[i]); 
    if ( MASK_IS_SET(x) )  {
      MASK128_ASSIGN(y,x);
      MASK128_ASSIGN(x,_mm_or_si128(x,_mm_srli_epi32(x,1)));
      MASK128_ASSIGN(x,_mm_or_si128(x,_mm_srli_epi32(x,2)));
      MASK128_ASSIGN(x,_mm_or_si128(x,_mm_srli_epi32(x,4)));
      MASK128_ASSIGN(x,_mm_or_si128(x,_mm_srli_epi32(x,8)));
      MASK128_ASSIGN(x,_mm_or_si128(x,_mm_srli_epi32(x,16)));
      MASK128_ASSIGN(x,_mm_and_si128(x,_mm_andnot_si128(_mm_srli_epi32(x,1),s.one)));
      MASK128_ASSIGN(x,_mm_srli_epi32(_mm_mullo_epi32(x,s.search),27));

#define RETURN_BIT(N)  return i*SSE2_BITS + 32*N + s_POS[(0x077CB531*MASK128_EXTRACT_UINT(x,N))>>27]
#undef  RETURN_BIT
#define RETURN_BIT(N)  return (i<<7) + (N<<5) + s_POS[(0x077CB531*MASK128_EXTRACT_UINT(x,N))>>27]

#undef  RETURN_BIT
#define RETURN_BIT(N)  \
      ::lib_rtl_output(LIB_RTL_INFO,"first_set(128) : i=%d r=%8d %8d  %08X e=%08X bit:%8d\n",N, \
		       MASK128_EXTRACT_UINT(x,N), s_POS[MASK128_EXTRACT_UINT(x,N)], \
		       s_POS[MASK128_EXTRACT_UINT(x,N)],MASK128_EXTRACT_UINT(first[i],N), \
		       (i<<7) + (N<<5) + s_POS[MASK128_EXTRACT_UINT(x,N)]); \
      return (i<<7) + (N<<5) + s_POS[MASK128_EXTRACT_UINT(x,N)]

#undef  RETURN_BIT
#define RETURN_BIT(N)  return (i<<7) + (N<<5) + s_POS[MASK128_EXTRACT_UINT(x,N)]

#undef  RETURN_BIT
      register __m128i z;
      MASK128_ASSIGN(z,_mm_add_epi32(_mm_slli_epi32(_mm_set1_epi32(i),7),s.Zero2ThreeShift5));
#define RETURN_BIT(N)  return MASK128_EXTRACT_UINT(z,N) + s_POS[MASK128_EXTRACT_UINT(x,N)]

      if      ( MASK128_EXTRACT_UINT(y,0) ) { RETURN_BIT(0); }
      else if ( MASK128_EXTRACT_UINT(y,1) ) { RETURN_BIT(1); }
      else if ( MASK128_EXTRACT_UINT(y,2) ) { RETURN_BIT(2); }
      else if ( MASK128_EXTRACT_UINT(y,3) ) { RETURN_BIT(3); }
    }
  }
#else
  unsigned long long int *p = (unsigned long long int*)first;
  size_t wrd = (second/SSE2_BITS)*2;
  for ( size_t i=0; i<wrd; ++i )  {
    int bit = ::ffsll(p[i]);
    if ( bit > 0 ) return 64*i + bit - 1;
  }
#endif
  return NO_BIT_SET;
}

/// Apply OR mask to existing bits. If other mask has less words, only apply the mask to existing words
void AVXMemory::mask_or(const Registers& mask)    {
  size_t wrd = mask.second<second ? mask.second/SSE2_BITS : second/SSE2_BITS;
  switch(wrd)  {
    default:
      for(size_t i=5; i<wrd;++i) MASK128_OR(i);
    case 6:   MASK128_OR(5);
    case 5:   MASK128_OR(4);
    case 4:   MASK128_OR(3);
    case 3:   MASK128_OR(2);
    case 2:   MASK128_OR(1);
    case 1:
    case 0:   MASK128_OR(0);
  }
}

/// Apply AND mask to existing bits. If other mask has less words, only apply the mask to existing words
void AVXMemory::mask_and(const Registers& mask)    {
  size_t wrd = mask.second<second ? mask.second/SSE2_BITS : second/SSE2_BITS;
  switch(wrd)  {
    default:
      for(size_t i=5; i<wrd;++i) MASK128_AND(i);
    case 6:   MASK128_AND(5);
    case 5:   MASK128_AND(4);
    case 4:   MASK128_AND(3);
    case 3:   MASK128_AND(2);
    case 2:   MASK128_AND(1);
    case 1:
    case 0:   MASK128_AND(0);
  }
}

/// Clear a bit in the mask
void AVXMemory::clear_bit(register size_t bit)   {
#ifdef _USE_SSE2
  MASK128_ASSIGN(first[SSE2_WORD(bit)],_mm_and_si128(first[SSE2_WORD(bit)],s.not_bit128[SSE2_BIT(bit)]));
#elif defined(_USE_MMX)
  __m64* p = ((__m64*)first)+MMX_WORD(bit);
  __m64* b = (__m64*)(&s.not_bit128[MMX_BIT(bit)]);
  p[0] = _mm_and_si64(p[0],b[0]);
#else
  unsigned int* ui = (unsigned int*)first;
  ui[bit/W_32] &= ~(1<<(bit%W_32));
#endif
}

/// Set a bit in the mask
void AVXMemory::set_bit(register size_t bit)  {
#ifdef _USE_SSE2
  MASK128_ASSIGN(first[SSE2_WORD(bit)],_mm_or_si128(first[SSE2_WORD(bit)],s.bit128[SSE2_BIT(bit)]));
#elif defined(_USE_MMX)
  __m64* p = ((__m64*)first)+MMX_WORD(bit);
  __m64* b = (__m64*)(&s.not_bit128[MMX_BIT(bit)]);
  p[0] = _mm_or_si64(p[0],b[0]);
#else
  unsigned int* ui = (unsigned int*)first;
  ui[bit/W_32] |= (1<<(bit%W_32));
#endif
}

bool AVXMemory::test_bit(register size_t bit)  const   {
#ifdef _USE_SSE2
  return MASK128_MATCH(first[SSE2_WORD(bit)],s.bit128[SSE2_BIT(bit)]);
#else
  unsigned int* ui = (unsigned int*)first;
  return 0 != (ui[bit/W_32]&(1<<(bit%W_32)));
#endif
}

/// Retrieve the items set in the mask
vector<size_t> AVXMemory::bits() const   {
  vector<size_t> result;
  for(size_t i=0; i<second; ++i)   {
    if ( test_bit(i) ) result.push_back(i);
  }
  return result;
}

/// Check the supplied mask and return all bits, which are not set
vector<size_t> AVXMemory::missing(const AVXMemory& m) const   {
  vector<size_t> result;
  for(size_t i=0; i<m.second; ++i)   {
    if ( m.test_bit(i) )   {
      if ( i>m.second || test_bit(i) )  {
        result.push_back(i);
      }
    }
  }
  return result;
}

/// Copy constructor
AVXHeap::AVXHeap(const AVXMemory& copy) : AVXMemory(), _buffer(0)  {
  size_t wrd = copy.second/SSE2_BITS;
  size_t siz = wrd*sizeof(__m128i)+SSE2_ALIGNMENT;
  _buffer = new unsigned char[siz];
  first   = (__m128i*)(long(_buffer+SSE2_ALIGNMENT)&~SSE2_ALIGNMENT);
  second  = copy.second; 
  for(size_t i=0; i<wrd; ++i)   {
#ifdef _USE_SSE2
    MASK128_ASSIGN(first[i],copy.first[i]);
#elif defined(_USE_MMX)
    first[i].m128i_u64[0] = copy.first[i].m128i_u64[0];
    first[i].m128i_u64[1] = copy.first[i].m128i_u64[1];
#else 
    first[i].m128i_u64[0] = copy.first[i].m128i_u64[0];
    first[i].m128i_u64[1] = copy.first[i].m128i_u64[1];
#endif
  }
}

void AVXHeap::dump(const std::string& txt) const {
  char text[128];
  for(size_t i=0, n=second/SSE2_BITS; i<n; ++i) {
    ::sprintf(text," %dth.word:",int(i));
    dump_i128(txt+text,first[i]);
  }
}

string AVXHeap::toString() const {
  string s = "";
  char text[128];
  for(size_t i=0, n=second/SSE2_BITS; i<n; ++i) {
    ::sprintf(text," %dth.word:",int(i));
    s += text;
    s += string_i128(first[i]);
  }
  return s;
}

void AVXHeap::_deallocate()  {
  first = 0;
  second = 0;
  deleteArray(_buffer);
}

void AVXHeap::_extend(size_t bits)  {
  if ( (second<<7) < bits )  {
    size_t old =  second/SSE2_BITS;
    size_t wrd = (bits+SSE2_BITS)/SSE2_BITS;
    size_t siz = (wrd+1)*sizeof(__m128i)+SSE2_ALIGNMENT;
    unsigned char* tmp = new unsigned char[siz];
    __m128i* mm_tmp = (__m128i*)(long(tmp+SSE2_ALIGNMENT)&~SSE2_ALIGNMENT);
    for(size_t i=0; i<old; ++i)  {
#ifdef _USE_SSE2
      MASK128_ASSIGN(mm_tmp[i],first[i]);
#else
      mm_tmp[i].m128i_u64[0] = first[i].m128i_u64[0];
      mm_tmp[i].m128i_u64[1] = first[i].m128i_u64[1];
#endif
    }
    for(size_t i=old; i<wrd; ++i)
      MASK128_ASSIGN(mm_tmp[i],_mm_setzero_si128());
    deleteArray(_buffer);
    second  = wrd*SSE2_BITS;
    _buffer = tmp;
    first = mm_tmp;
  }
}

namespace Framework {
  template<> AVXMask<1>::AVXMask()   {
    first = (__m128i*)(long(_buffer+ALIGNMENT)&~ALIGNMENT);
    MASK128_ASSIGN(first[0],_mm_setzero_si128());
    second = 1*SSE2_BITS;
  }

  template<> AVXMask<2>::AVXMask()   {
    first = (__m128i*)(long(_buffer+ALIGNMENT)&~ALIGNMENT);
    MASK128_ASSIGN(first[0],_mm_setzero_si128());
    MASK128_ASSIGN(first[1],_mm_setzero_si128());
    second = 2*SSE2_BITS;
  }

  template<> AVXMask<3>::AVXMask()   {
    first = (__m128i*)(long(_buffer+ALIGNMENT)&~ALIGNMENT);
    MASK128_ASSIGN(first[0],_mm_setzero_si128());
    MASK128_ASSIGN(first[1],_mm_setzero_si128());
    MASK128_ASSIGN(first[2],_mm_setzero_si128());
    second = 3*SSE2_BITS;
  }

  template<> AVXMask<4>::AVXMask()   {
    first = (__m128i*)(long(_buffer+ALIGNMENT)&~ALIGNMENT);
    MASK128_ASSIGN(first[0],_mm_setzero_si128());
    MASK128_ASSIGN(first[1],_mm_setzero_si128());
    MASK128_ASSIGN(first[2],_mm_setzero_si128());
    MASK128_ASSIGN(first[3],_mm_setzero_si128());
    second = 4*SSE2_BITS;
  }

  template<> AVXMask<1>::AVXMask(const AVXMemory& copy)   {
    first = (__m128i*)(long(_buffer+ALIGNMENT)&~ALIGNMENT);
    MASK128_ASSIGN(first[0],copy.first[0]);
    second = 1*SSE2_BITS;
  }

  template<> AVXMask<2>::AVXMask(const AVXMemory& copy)   {
    first = (__m128i*)(long(_buffer+ALIGNMENT)&~ALIGNMENT);
    MASK128_ASSIGN(first[0],copy.first[0]);
    MASK128_ASSIGN(first[1],copy.second>SSE2_BITS ? copy.first[1] : _mm_setzero_si128());
    second = 2*SSE2_BITS;
  }

  template<> AVXMask<3>::AVXMask(const AVXMemory& copy)   {
    first = (__m128i*)(long(_buffer+ALIGNMENT)&~ALIGNMENT);
    MASK128_ASSIGN(first[0],copy.first[0]);
    MASK128_ASSIGN(first[1],copy.second>1*SSE2_BITS ? copy.first[1] : _mm_setzero_si128());
    MASK128_ASSIGN(first[2],copy.second>2*SSE2_BITS ? copy.first[2] : _mm_setzero_si128());
    second = 3*SSE2_BITS;
  }

  template<> AVXMask<4>::AVXMask(const AVXMemory& copy)   {
    first = (__m128i*)(long(_buffer+ALIGNMENT)&~ALIGNMENT);
    MASK128_ASSIGN(first[0],copy.first[0]);
    MASK128_ASSIGN(first[1],copy.second>1*SSE2_BITS ? copy.first[1] : _mm_setzero_si128());
    MASK128_ASSIGN(first[2],copy.second>2*SSE2_BITS ? copy.first[2] : _mm_setzero_si128());
    MASK128_ASSIGN(first[3],copy.second>3*SSE2_BITS ? copy.first[3] : _mm_setzero_si128());
    second = 4*SSE2_BITS;
  }

  template<> AVXMask<1>::AVXMask(unsigned int* mask)  {
    first = (__m128i*)(long(_buffer+ALIGNMENT)&~ALIGNMENT);
    MASK128_ASSIGN(first[0],_mm_set_epi32(mask[0],mask[1],mask[2],mask[3]));
    second = 1*SSE2_BITS;
  }

  template<> AVXMask<2>::AVXMask(unsigned int* mask)   {
    first = (__m128i*)(long(_buffer+ALIGNMENT)&~ALIGNMENT);
    MASK128_ASSIGN(first[0],_mm_set_epi32(mask[0],mask[1],mask[2],mask[3]));
    MASK128_ASSIGN(first[1],_mm_set_epi32(mask[0],mask[1],mask[2],mask[3]));
    second = 2*SSE2_BITS;
  }

  template<> AVXMask<3>::AVXMask(unsigned int* mask)   {
    first = (__m128i*)(long(_buffer+ALIGNMENT)&~ALIGNMENT);
    MASK128_ASSIGN(first[0],_mm_set_epi32(mask[0],mask[1],mask[2],mask[3]));
    MASK128_ASSIGN(first[1],_mm_set_epi32(mask[0],mask[1],mask[2],mask[3]));
    MASK128_ASSIGN(first[2],_mm_set_epi32(mask[0],mask[1],mask[2],mask[3]));
    second = 3*SSE2_BITS;
  }

  template<> AVXMask<4>::AVXMask(unsigned int* mask)   {
    first = (__m128i*)(long(_buffer+ALIGNMENT)&~ALIGNMENT);
    MASK128_ASSIGN(first[0],_mm_set_epi32(mask[0],mask[1],mask[2],mask[3]));
    MASK128_ASSIGN(first[1],_mm_set_epi32(mask[0],mask[1],mask[2],mask[3]));
    MASK128_ASSIGN(first[2],_mm_set_epi32(mask[0],mask[1],mask[2],mask[3]));
    MASK128_ASSIGN(first[3],_mm_set_epi32(mask[0],mask[1],mask[2],mask[3]));
    second = 4*SSE2_BITS;
  }

  /// Clear a bit in the mask
  template<> void AVXMask<1>::clear_bit(register size_t bit)   {
#ifdef _USE_SSE2
    MASK128_ASSIGN(first[0],_mm_and_si128(first[0],s.not_bit128[SSE2_BIT(bit)]));
#endif
  }

  /// Clear a bit in the mask
  template<> void AVXMask<2>::clear_bit(register size_t bit)   {
#ifdef _USE_SSE2
    MASK128_ASSIGN(first[SSE2_WORD(bit)],_mm_and_si128(first[SSE2_WORD(bit)],s.not_bit128[SSE2_BIT(bit)]));
#endif
  }

  /// Clear a bit in the mask
  template<> void AVXMask<3>::clear_bit(register size_t bit)   {
#ifdef _USE_SSE2
    MASK128_ASSIGN(first[SSE2_WORD(bit)],_mm_and_si128(first[SSE2_WORD(bit)],s.not_bit128[SSE2_BIT(bit)]));
#endif
  }

  /// Clear a bit in the mask
  template<> void AVXMask<4>::clear_bit(register size_t bit)   {
#ifdef _USE_SSE2
    MASK128_ASSIGN(first[SSE2_WORD(bit)],_mm_and_si128(first[SSE2_WORD(bit)],s.not_bit128[SSE2_BIT(bit)]));
#endif
  }

  /// Set a bit in the mask
  template<> void AVXMask<1>::set_bit(register size_t bit)  {
#ifdef _USE_SSE2
    MASK128_ASSIGN(first[0],_mm_or_si128(first[0],s.bit128[SSE2_BIT(bit)]));
#else
    unsigned int* ui = (unsigned int*)first;
    ui[bit/W_32] |= (1<<(bit%W_32));
#endif
  }

  /// Set a bit in the mask
  template<> void AVXMask<2>::set_bit(register size_t bit)  {
#ifdef _USE_SSE2
    MASK128_ASSIGN(first[SSE2_WORD(bit)],_mm_or_si128(first[SSE2_WORD(bit)],s.bit128[SSE2_BIT(bit)]));
#else
    unsigned int* ui = (unsigned int*)first;
    ui[bit/W_32] |= (1<<(bit%W_32));
#endif
  }

  /// Set a bit in the mask
  template<> void AVXMask<3>::set_bit(register size_t bit)  {
#ifdef _USE_SSE2
    MASK128_ASSIGN(first[SSE2_WORD(bit)],_mm_or_si128(first[SSE2_WORD(bit)],s.bit128[SSE2_BIT(bit)]));
#else
    unsigned int* ui = (unsigned int*)first;
    ui[bit/W_32] |= (1<<(bit%W_32));
#endif
  }

  /// Set a bit in the mask
  template<> void AVXMask<4>::set_bit(register size_t bit)  {
#ifdef _USE_SSE2
    MASK128_ASSIGN(first[SSE2_WORD(bit)],_mm_or_si128(first[SSE2_WORD(bit)],s.bit128[SSE2_BIT(bit)]));
#else
    unsigned int* ui = (unsigned int*)first;
    ui[bit/W_32] |= (1<<(bit%W_32));
#endif
  }

  template<> bool AVXMask<1>::test_bit(register size_t bit)  const   {
#ifdef _USE_SSE2
    return MASK128_MATCH(first[0],s.bit128[SSE2_BIT(bit)]);
#else
    unsigned int* ui = (unsigned int*)first;
    return 0 != (ui[bit/W_32]&(1<<(bit%W_32)));
#endif
  }

  template<> bool AVXMask<2>::test_bit(register size_t bit)  const   {
#ifdef _USE_SSE2
    return MASK128_MATCH(first[SSE2_WORD(bit)],s.bit128[SSE2_BIT(bit)]);
#else
    unsigned int* ui = (unsigned int*)first;
    return 0 != (ui[bit/W_32]&(1<<(bit%W_32)));
#endif
  }

  template<> bool AVXMask<3>::test_bit(register size_t bit)  const   {
#ifdef _USE_SSE2
    return MASK128_MATCH(first[SSE2_WORD(bit)],s.bit128[SSE2_BIT(bit)]);
#else
    unsigned int* ui = (unsigned int*)first;
    return 0 != (ui[bit/W_32]&(1<<(bit%W_32)));
#endif
  }

  template<> bool AVXMask<4>::test_bit(register size_t bit)  const   {
#ifdef _USE_SSE2
    return MASK128_MATCH(first[SSE2_WORD(bit)],s.bit128[SSE2_BIT(bit)]);
#else
    unsigned int* ui = (unsigned int*)first;
    return 0 != (ui[bit/W_32]&(1<<(bit%W_32)));
#endif
  }

  /// Apply AND mask to existing bits. If other mask has less words, only apply the mask to existing words
  template<> template<> void AVXMask<1>::mask_and(const AVXMask<1>& mask)    {
    MASK128_AND(0);
  }

  /// Apply AND mask to existing bits. If other mask has less words, only apply the mask to existing words
  template<> template<> void AVXMask<2>::mask_and(const AVXMask<2>& mask)    {
    MASK128_AND(1);
    MASK128_AND(0);
  }

  /// Apply AND mask to existing bits. If other mask has less words, only apply the mask to existing words
  template<> template<> void AVXMask<3>::mask_and(const AVXMask<3>& mask)    {
    MASK128_AND(2);
    MASK128_AND(1);
    MASK128_AND(0);
  }

  /// Apply AND mask to existing bits. If other mask has less words, only apply the mask to existing words
  template<> template<> void AVXMask<4>::mask_and(const AVXMask<4>& mask)    {
    MASK128_AND(3);
    MASK128_AND(2);
    MASK128_AND(1);
    MASK128_AND(0);
  }

  /// Apply AND mask to existing bits. If other mask has less words, only apply the mask to existing words
  template<> template<> void AVXMask<1>::mask_or(const AVXMask<1>& mask)    {
    MASK128_OR(0);
  }

  /// Apply AND mask to existing bits. If other mask has less words, only apply the mask to existing words
  template<> template<> void AVXMask<2>::mask_or(const AVXMask<2>& mask)    {
    MASK128_OR(1);
    MASK128_OR(0);
  }

  /// Apply AND mask to existing bits. If other mask has less words, only apply the mask to existing words
  template<> template<> void AVXMask<3>::mask_or(const AVXMask<3>& mask)    {
    MASK128_OR(2);
    MASK128_OR(1);
    MASK128_OR(0);
  }

  /// Apply AND mask to existing bits. If other mask has less words, only apply the mask to existing words
  template<> template<> void AVXMask<4>::mask_or(const AVXMask<4>& mask)    {
    MASK128_OR(3);
    MASK128_OR(2);
    MASK128_OR(1);
    MASK128_OR(0);
  }

  template<> void AVXMask<1>::dump(const string& txt)  const     {
    dump_i128(txt+" 1rst.word:",first[0]);
  }

  template<> void AVXMask<2>::dump(const string& txt)  const     {
    dump_i128(txt+" 1rst.word:",first[0]);
    dump_i128(txt+" 2nd. word:",first[1]);
  }

  template<> void AVXMask<3>::dump(const string& txt)  const     {
    dump_i128(txt+" 1rst.word:",first[0]);
    dump_i128(txt+" 2nd. word:",first[1]);
    dump_i128(txt+" 3rd. word:",first[2]);
  }

  template<> void AVXMask<4>::dump(const string& txt)  const     {
    dump_i128(txt+" 1rst.word:",first[0]);
    dump_i128(txt+" 2nd. word:",first[1]);
    dump_i128(txt+" 3rd. word:",first[2]);
    dump_i128(txt+" 4th. word:",first[3]);
  }

  template<> string AVXMask<1>::toString()  const     {
    return string_i128(first[0]);
  }

  template<> string AVXMask<2>::toString()  const     {
    return string_i128(first[0])+string_i128(first[1]);
  }

  template<> string AVXMask<3>::toString()  const     {
    return string_i128(first[0])+string_i128(first[1])+string_i128(first[2]);
  }

  template<> string AVXMask<4>::toString()  const     {
    return string_i128(first[0])+string_i128(first[1])+string_i128(first[2])+string_i128(first[3]);
  }

  template<> template<> bool AVXMask<1>::match(const AVXMask<1>& p)  const     {
    return MASK128_MATCH(first[0],p.first[0]);
  }
  template<> template<> bool AVXMask<2>::match(const AVXMask<2>& p)  const     {
    return MASK128_MATCH(first[0],p.first[0]) && MASK128_MATCH(first[1],p.first[1]);
  }

  template<> template<> bool AVXMask<3>::match(const AVXMask<3>& p)  const     {
    return MASK128_MATCH(first[0],p.first[0]) && MASK128_MATCH(first[1],p.first[1]) && MASK128_MATCH(first[2],p.first[2]);
  }

  template<> template<> bool AVXMask<4>::match(const AVXMask<4>& p)  const     {
    return MASK128_MATCH(first[0],p.first[0]) && MASK128_MATCH(first[1],p.first[1]) && MASK128_MATCH(first[2],p.first[2]) && MASK128_MATCH(first[3],p.first[3]);
  }

  template<> void AVXMask<1>::set_bits(const Registers& bits)  {
    MASK128_ASSIGN(first[0],bits.first[0]);
  }

  template<> void AVXMask<2>::set_bits(const Registers& bits)  {
    MASK128_ASSIGN(first[0],bits.first[0]);
    MASK128_ASSIGN(first[0],bits.first[1]);
  }
}

#if 0

template<> template<> void AVXMask<1>::mask_or(const AVXMask<1>& p)   {
  MASK128_ASSIGN(first[0],_mm_or_si128(first[0],p.first[0]));
}

template<> template<> void AVXMask<2>::mask_or(const AVXMask<1>& p)   {
  MASK128_ASSIGN(first[0],_mm_or_si128(first[0],p.first[0]));
}

template<> template<> void AVXMask<2>::mask_or(const AVXMask<2>& p)   {
  MASK128_ASSIGN(first[0],_mm_or_si128(first[0],p.first[0]));
  MASK128_ASSIGN(first[1],_mm_or_si128(first[1],p.first[1]));
}

template<> template<> void AVXMask<3>::mask_or(const AVXMask<1>& p)   {
  MASK128_ASSIGN(first[0],_mm_or_si128(first[0],p.first[0]));
}

template<> template<> void AVXMask<3>::mask_or(const AVXMask<2>& p)   {
  MASK128_ASSIGN(first[0],_mm_or_si128(first[0],p.first[0]));
  MASK128_ASSIGN(first[1],_mm_or_si128(first[1],p.first[1]));
}

template<> template<> void AVXMask<3>::mask_or(const AVXMask<3>& p)   {
  MASK128_ASSIGN(first[0],_mm_or_si128(first[0],p.first[0]));
  MASK128_ASSIGN(first[1],_mm_or_si128(first[1],p.first[1]));
  MASK128_ASSIGN(first[2],_mm_or_si128(first[2],p.first[2]));
}

template<> template<> void AVXMask<4>::mask_or(const AVXMask<1>& p)   {
  MASK128_ASSIGN(first[0],_mm_or_si128(first[0],p.first[0]));
}

template<> template<> void AVXMask<4>::mask_or(const AVXMask<2>& p)   {
  MASK128_ASSIGN(first[0],_mm_or_si128(first[0],p.first[0]));
  MASK128_ASSIGN(first[1],_mm_or_si128(first[1],p.first[1]));
}

template<> template<> void AVXMask<4>::mask_or(const AVXMask<3>& p)   {
  MASK128_ASSIGN(first[0],_mm_or_si128(first[0],p.first[0]));
  MASK128_ASSIGN(first[1],_mm_or_si128(first[1],p.first[1]));
  MASK128_ASSIGN(first[2],_mm_or_si128(first[2],p.first[2]));
}

template<> template<> void AVXMask<4>::mask_or(const AVXMask<4>& p)   {
  MASK128_ASSIGN(first[0],_mm_or_si128(first[0],p.first[0]));
  MASK128_ASSIGN(first[1],_mm_or_si128(first[1],p.first[1]));
  MASK128_ASSIGN(first[2],_mm_or_si128(first[2],p.first[2]));
  MASK128_ASSIGN(first[3],_mm_or_si128(first[3],p.first[3]));
}

template<> template<> void AVXMask<1>::mask_and(const AVXMask<1>& p)   {
  MASK128_ASSIGN(first[0],_mm_and_si128(first[0],p.first[0]));
}

template<> template<> void AVXMask<2>::mask_and(const AVXMask<1>& p)   {
  MASK128_ASSIGN(first[0],_mm_and_si128(first[0],p.first[0]));
}

template<> template<> void AVXMask<2>::mask_and(const AVXMask<2>& p)   {
  MASK128_ASSIGN(first[0],_mm_and_si128(first[0],p.first[0]));
  MASK128_ASSIGN(first[1],_mm_and_si128(first[1],p.first[1]));
}

template<> template<> void AVXMask<3>::mask_and(const AVXMask<1>& p)   {
  MASK128_ASSIGN(first[0],_mm_and_si128(first[0],p.first[0]));
}

template<> template<> void AVXMask<3>::mask_and(const AVXMask<2>& p)   {
  MASK128_ASSIGN(first[0],_mm_and_si128(first[0],p.first[0]));
  MASK128_ASSIGN(first[1],_mm_and_si128(first[1],p.first[1]));
}

template<> template<> void AVXMask<3>::mask_and(const AVXMask<3>& p)   {
  MASK128_ASSIGN(first[0],_mm_and_si128(first[0],p.first[0]));
  MASK128_ASSIGN(first[1],_mm_and_si128(first[1],p.first[1]));
  MASK128_ASSIGN(first[2],_mm_and_si128(first[2],p.first[2]));
}

template<> template<> void AVXMask<4>::mask_and(const AVXMask<1>& p)   {
  MASK128_ASSIGN(first[0],_mm_and_si128(first[0],p.first[0]));
}

template<> template<> void AVXMask<4>::mask_and(const AVXMask<2>& p)   {
  MASK128_ASSIGN(first[0],_mm_and_si128(first[0],p.first[0]));
  MASK128_ASSIGN(first[1],_mm_and_si128(first[1],p.first[1]));
}

template<> template<> void AVXMask<4>::mask_and(const AVXMask<3>& p)   {
  MASK128_ASSIGN(first[0],_mm_and_si128(first[0],p.first[0]));
  MASK128_ASSIGN(first[1],_mm_and_si128(first[1],p.first[1]));
  MASK128_ASSIGN(first[2],_mm_and_si128(first[2],p.first[2]));
}

template<> template<> void AVXMask<4>::mask_and(const AVXMask<4>& p)   {
  MASK128_ASSIGN(first[0],_mm_and_si128(first[0],p.first[0]));
  MASK128_ASSIGN(first[1],_mm_and_si128(first[1],p.first[1]));
  MASK128_ASSIGN(first[2],_mm_and_si128(first[2],p.first[2]));
  MASK128_ASSIGN(first[3],_mm_and_si128(first[3],p.first[3]));
}

template<> template<> bool AVXMask<2>::match(const AVXMask<1>& p)  const     {
  return MASK128_MATCH(first[0],p.first[0]);
}

template<> template<> bool AVXMask<2>::match(const AVXMask<2>& p)  const     {
  return MASK128_MATCH(first[0],p.first[0]) && MASK128_MATCH(first[1],p.first[1]);
}

template<> template<> bool AVXMask<3>::match(const AVXMask<1>& p)  const     {
  return MASK128_MATCH(first[0],p.first[0]);
}

template<> template<> bool AVXMask<3>::match(const AVXMask<2>& p)  const     {
  return MASK128_MATCH(first[0],p.first[0]) && MASK128_MATCH(first[1],p.first[1]);
}

template<> template<> bool AVXMask<3>::match(const AVXMask<3>& p)  const     {
  return MASK128_MATCH(first[0],p.first[0]) && MASK128_MATCH(first[1],p.first[1]) && MASK128_MATCH(first[2],p.first[2]);
}

template<> template<> bool AVXMask<4>::match(const AVXMask<1>& p)  const     {
  return MASK128_MATCH(first[0],p.first[0]);
}

template<> template<> bool AVXMask<4>::match(const AVXMask<2>& p)  const     {
  return MASK128_MATCH(first[0],p.first[0]) && MASK128_MATCH(first[1],p.first[1]);
}

template<> template<> bool AVXMask<4>::match(const AVXMask<3>& p)  const     {
  return MASK128_MATCH(first[0],p.first[0]) && MASK128_MATCH(first[1],p.first[1]) && MASK128_MATCH(first[2],p.first[2]);
}

template<> template<> bool AVXMask<4>::match(const AVXMask<4>& p)  const     {
  return MASK128_MATCH(first[0],p.first[0]) && MASK128_MATCH(first[1],p.first[1]) && MASK128_MATCH(first[2],p.first[2]) && MASK128_MATCH(first[3],p.first[3]);
}
#endif
