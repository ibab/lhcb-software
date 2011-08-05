#include <xmmintrin.h>
#include <iostream>
#include <cmath>
using namespace std;

int main(int, char**) {
  //int _mm_testz_si128 (__m128i s1, __m128i s2);
  __m128i s1 = _mm_cvtsi32_si128(0+16);
  __m128i s2 = _mm_cvtsi32_si128(4+16);
  int res, *q;
  //res = _mm_testz_si128(s1,s2);
  __m128i r = _mm_and_si128(s1,s2);
  res = _mm_cvtsi128_si32(r);
  cout << "r:" << res << endl;
  {
    __m128i r2 = _mm_cmpeq_epi32(s1,s2);
    int* q = (int*)&r2;
    cout << "_mm_cmpeq_epi32:r:" 
	 << q[0] << " " << q[1] << " " << q[2] << " " << q[3] << " " << endl;
  }
  {
    __m128i r2 = _mm_sad_epu8(s1,s2);
    int* q = (int*)&r2;
    cout << "_mm_sad_epu8:r:" 
	 << q[0] << " " << q[1] << " " << q[2] << " " << q[3] << " " << endl;
  }
  return 1;
}
