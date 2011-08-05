#include <xmmintrin.h>
#include <emmintrin.h>
#ifndef _WIN32
#include <smmintrin.h>
#include <tmmintrin.h>
#endif

#include <ctime>
#include <climits>
#include <iostream>
#include "RTL/rtl.h"
#include "Test/Bits.h"
#include "Framework/EventContext.h"

using namespace Framework;
using namespace std;

#define _USE_SSE2 1
#define W_32  (CHAR_BIT*sizeof(int))


int avx_bit_test0() {
  const char* line="-----------------------------------------------------";
  unsigned int hits = VXD_HITS|ECAL_HITS|MUON_HITS|TRACKER_HITS;
  unsigned int h[] = {hits,0,0,0};
  unsigned int k[] = {hits,0,hits,0,hits,0,hits,0};
  unsigned int l[] = {hits,VXD_TRACKS,hits,VXD_DEPOSITS,hits,0,0,0,hits,0,0,0};
  unsigned int m[] = {hits,VXD_TRACKS,hits,VXD_DEPOSITS,hits,0,0,0,hits,0,0,0,0,0,0,0};
  //EventContext<3> ctxt;
  EventContext ctxt(0);
  AVXMask<1> o1(h);
  AVXMask<2> o2(k);
  AVXMask<3> o3(l);
  AVXMask<4> o4(m);
  o1.dump("IO Mask1:");
  o2.dump("IO Mask2:");
  o3.dump("IO Mask3:");
  o4.dump("IO Mask4:");

  ctxt.addData(o1);
  ctxt.dumpIO("CTXT:");

  cout << line << endl;
  cout << "o1> " << (ctxt.matchIO(o1) ? "SELECTED" : "REJECTED") << endl << line << endl;
#if 0
  ctxt.addData(o2);
  ctxt.dumpIO("CTXT:");
  cout << "o2> " << (ctxt.match(o2) ? "SELECTED" : "REJECTED") << endl << line << endl;
  cout << "o3> " << (ctxt.match(o3) ? "SELECTED" : "REJECTED") << endl << line << endl;
  cout << "o4> " << (ctxt.match(o4) ? "SELECTED" : "REJECTED") << endl << line << endl;
  ctxt.addData(o3);
  ctxt.dump("CTXT:");
  cout << "o3> " << (ctxt.match(o3) ? "SELECTED" : "REJECTED") << endl << line << endl;
  cout << "o4> " << (ctxt.match(o4) ? "SELECTED" : "REJECTED") << endl << line << endl;
#endif
  return 1;
}
