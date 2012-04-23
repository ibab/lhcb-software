#include "Gaucho/MonRate.h"
#include "Gaucho/RateMgr.h"
int main(int ,char**)
{
  unsigned long long dt;
  int i;
  long long j;
  double d;
  long long al[100];
  int ai[100];
  StatEntity se;
  MonRateBase *r1,*r2,*r3,*r4, *r5,*r6;
  RateMgr rm(0,"");
  int ii;
  for (ii=0;ii<100;ii++)
  {
    al[ii] = ii+1;
    ai[ii] = ii+2;
  }
  r1 = new MonRate<int>("int","hkjashda",i);
  r2 = new MonRate<long long>("long long","hkjashda",j);
  r3 = new MonRate<double>("double","hkjashda",d);
  r4 = new MonRate<StatEntity>("StatEntity","hkjashda",se);
  r5 = new MonRateA<long long*>("long*","jashdjah","X",al,sizeof(al));
  r6 = new MonRateA<int*>("int*","jashdjah","i",ai,sizeof(ai));
  rm.addRate("int",*r1);
  rm.addRate("long long",*r2);
  rm.addRate("double",*r3);
  rm.addRate("StatEntity",*r4);
  rm.addRate("long*",*r5);
  rm.addRate("int*",*r6);
  dt = 100000000;
  i = 30;
  j = 45;
  d = 450000000.0;
  se++;
  rm.makeRates(dt);
  rm.print();
  MonRateBase *br1 = r1;
  MonRateBase *br6 = r6;
  delete br1;
  delete br6;
  return 0;
}
