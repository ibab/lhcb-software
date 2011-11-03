#ifndef FRAMEWORKTESTS_BITS_H
#define FRAMEWORKTESTS_BITS_H


#define BITS(i)     (1<<i)

#define VXD_HITS          BITS(0)
#define TRACKER_HITS      BITS(1)
#define HCAL_HITS         BITS(2)
#define ECAL_HITS         BITS(3)
#define MUON_HITS         BITS(4)
#define TPC_HITS          BITS(5)

#define VXD_DEPOSITS      BITS(17)
#define TRACKER_DEPOSITS  BITS(18)
#define TPC_DEPOSITS      BITS(19)
#define ECAL_DEPOSITS     BITS(20)
#define HCAL_DEPOSITS     BITS(21)
#define MUON_DEPOSITS     BITS(22)

#define VXD_TRACKS        BITS(1)
#define TPC_TRACKS        BITS(2)
#define TRACKER_TRACKS    BITS(3)

namespace Test {
  void dump(const char* txt,const __m128i& v);
  void m128_dump(const __m128i& v);
}

#endif // FRAMEWORKTESTS_BITS_H
