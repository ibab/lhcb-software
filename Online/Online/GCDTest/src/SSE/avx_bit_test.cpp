#include <ctime>
#include "Framework/AVX.h"

using namespace Framework;
using namespace std;

namespace {

  struct TheTest {

    const __m128i* bit128;

    TheTest()  {
      bit128 = AVXMemory::SSE_bits().first;
    }

    enum { SSE2_BITS=AVXMask<1>::SSE2_BITS, SLOW=1 };
    clock_t loop_time;    

    template <class _M> void test_loop(_M)  {
      int b1 = 0;
      clock_t start = clock();
      for(size_t i=0; i<SSE2_BITS; ++i)  {
        for(size_t k=0; k<SSE2_BITS*SLOW; ++k)   {
          for(size_t j=0; j<k; j+=3)   {
            for(size_t b=0; b<SSE2_BITS;++b)   {
              ++b1;
            }
          }
        }
      }
      clock_t end = clock();
      ::printf("LOOP COST:       took %7ld clock ticks [%ld ticks/second] [%d turns]\n",
	       long(end-start),long(CLOCKS_PER_SEC),b1);
      loop_time = end-start;
    }

    template <typename _M> void test_first(_M mask)  {
      typedef typename _M::Registers Registers;
      long long int b1 = 0, b2 = 0;
      clock_t start, end;

      start = clock();
      Registers regs(mask.registers());
      for(size_t i=0; i<SSE2_BITS; ++i)  {
        mask.assign(Registers(&bit128[i],regs.second));
        for(size_t k=0; k<SSE2_BITS*SLOW; ++k)   {
          for(size_t j=0; j<k; j+=3)   {
            for(size_t b=0; b<SSE2_BITS;++b)   {
              //mask.assign(Registers(&bit128[b],regs.second));
              b2 += mask.first_set_int32();
            }
          }
        }
      }
      end = clock();
      ::printf("first_set_int32: took %7ld clock ticks - %7ld (corrected) [%lld] [0x%llX]\n",
	       long(end-start),long(end-start-loop_time),b2,b2);
      b2 = b1 = 0;
      start = clock();
      for(size_t i=0; i<SSE2_BITS; ++i)  {
        mask.assign(Registers(&bit128[i],regs.second));
        for(size_t k=0; k<SSE2_BITS*SLOW; ++k)   {
          for(size_t j=0; j<k; j+=3)   {
            for(size_t b=0; b<SSE2_BITS;++b)   {
              //mask.assign(Registers(&bit128[SSE2_BITS-b-1],regs.second));
              b2 += mask.first_set_SSE();
            }
	    //exit(0);
          }
        }
      }
      end = clock();
      ::printf("first_set_SSE:   took %7ld clock ticks - %7ld (corrected) [%lld] [0x%llX]\n",
	       long(end-start),long(end-start-loop_time),b2,b2);
    }
    template <typename _M> void test_set_bit(_M mask)  {
      int b1 = 0;
      clock_t start = clock();
      for(size_t i=0; i<SSE2_BITS; ++i)  {
        for(size_t k=0; k<SSE2_BITS*SLOW; ++k)   {
          for(size_t j=0; j<k; j+=3)   {
            for(size_t b=0; b<SSE2_BITS;++b)   {
              mask.set_bit(b);
              ++b1;
            }
          }
        }
      }
      clock_t end = clock();
      ::printf("set_bit:         took %7ld clock ticks - %7ld (corrected) [%d turns]\n",
	       long(end-start),long(end-start-loop_time),b1);
    }
    template <class _M> void test_clr_bit(_M mask)  {
      int b1 = 0;
      clock_t start = clock();
      for(size_t i=0; i<SSE2_BITS; ++i)  {
        for(size_t k=0; k<SSE2_BITS*SLOW; ++k)   {
          for(size_t j=0; j<k; j+=3)   {
            for(size_t b=0; b<SSE2_BITS;++b)   {
              mask.clear_bit(b);
              ++b1;
            }
          }
        }
      }
      clock_t end = clock();
      ::printf("clear_bit:       took %7ld clock ticks - %7ld (corrected) [%d turns]\n",
	       long(end-start),long(end-start-loop_time),b1);
    }
    template <class _M> void test_zero(_M mask)  {
      int b1 = 0;
      clock_t start = clock();
      for(size_t i=0; i<SSE2_BITS; ++i)  {
        for(size_t k=0; k<SSE2_BITS*SLOW; ++k)   {
          for(size_t j=0; j<k; j+=3)   {
            for(size_t b=0; b<SSE2_BITS;++b)   {
              mask.zero();
              ++b1;
            }
          }
        }
      }
      clock_t end = clock();
      ::printf("zero:            took %7ld clock ticks - %7ld (corrected) [%d turns]\n",
	       long(end-start),long(end-start-loop_time),b1);
    }
    template <class _M> void test_match(_M mask, _M match)  {
      typedef typename _M::Registers Registers; 
      int b1 = 0;
      clock_t start, end;
      Registers regs(mask.registers());
      start = clock();
      for(size_t i=0; i<SSE2_BITS; ++i)  {
        mask.assign(Registers(&bit128[i],regs.second));
        for(size_t k=0; k<SSE2_BITS*SLOW; ++k)   {
          for(size_t j=0; j<k; j+=3)   {
            for(size_t b=0; b<SSE2_BITS;++b)   {
              mask.mask_and(match);
            }
          }
        }
      }
      end = clock();
      ::printf("mask_and:        took %7ld clock ticks - %7ld (corrected)\n",
	       long(end-start),long(end-start-loop_time));
      start = clock();
      for(size_t i=0; i<SSE2_BITS; ++i)  {
        mask.assign(Registers(&bit128[i],regs.second));
        for(size_t k=0; k<SSE2_BITS*SLOW; ++k)   {
          for(size_t j=0; j<k; j+=3)   {
            for(size_t b=0; b<SSE2_BITS;++b)   {
              mask.mask_or(match);
            }
          }
        }
      }
      end = clock();
      ::printf("mask_or:         took %7ld clock ticks - %7ld (corrected)\n",
	       long(end-start),long(end-start-loop_time));
      start = clock();
      b1 = 0;
      match.assign(Registers(&bit128[120],regs.second));
      for(size_t i=0; i<SSE2_BITS; ++i)  {
        mask.assign(Registers(&bit128[i],regs.second));
        for(size_t k=0; k<SSE2_BITS*SLOW; ++k)   {
          for(size_t j=0; j<k; j+=3)   {
            for(size_t b=0; b<SSE2_BITS;++b)  {
              b1 += mask.match(match);
            }
          }
        }
      }
      end = clock();
      ::printf("mask_match:      took %7ld clock ticks - %7ld (corrected) [%d]\n",
	       long(end-start),long(end-start-loop_time),b1);
    }
    template <class _M> void test_test_bit(_M mask)  {
      typedef typename _M::Registers Registers;
      int b1 = 0;
      clock_t start, end;
      Registers regs(mask.registers());
      start = clock();
      b1 = 0;
      for(size_t i=0; i<SSE2_BITS; ++i)  {
        mask.assign(Registers(&bit128[i],regs.second));
        for(size_t k=0; k<SSE2_BITS*SLOW; ++k)   {
          for(size_t j=0; j<k; j+=3)   {
            for(size_t b=0; b<SSE2_BITS;++b)  {
              b1 += mask.test_bit(b);
            }
          }
        }
      }
      end = clock();
      ::printf("test_bit:        took %7ld clock ticks - %7ld (corrected) [%d]\n",
	       long(end-start),long(end-start-loop_time),b1);
    }
    template<class _M> void test_type(const char* txt, const _M& p, const _M& q)  {
      ::printf("----> Test %s performance:\n",txt);
      this->test_loop(p);
      this->test_zero(p);
      this->test_first(p);
      this->test_set_bit(p);
      this->test_clr_bit(p);
      this->test_test_bit(p);
      this->test_match(p,q);
    }
  };
}

#define RUN_TYPE(x) test.test_type(#x,x,x)

int avx_bit_test3()  {
  TheTest test;
  RUN_TYPE(AVXMask<1>());
  RUN_TYPE(AVXMask<2>());
  RUN_TYPE(AVXMask<4>());
  RUN_TYPE(AVXHeap(128));
  RUN_TYPE(AVXHeap(512));
  RUN_TYPE(AVXHeap(2048));
  return 0;
}
