#include "Gaucho/MonRate.h"
#include "Gaucho/RateMgr.h"
int main(int ,char**)
{
  unsigned long long dt;
  int i;
  long long j;
  double d;
  StatEntity se;
  MonRateBase *r1,*r2,*r3,*r4;
  RateMgr rm(0,"");
  r1 = new MonRate<int>("int","hkjashda",i);
  r2 = new MonRate<long long>("long long","hkjashda",j);
  r3 = new MonRate<double>("double","hkjashda",d);
  r4 = new MonRate<StatEntity>("StatEntity","hkjashda",se);
  rm.addRate("int","",*r1);
  rm.addRate("long long","",*r2);
  rm.addRate("double","",*r3);
  rm.addRate("StatEntity","",*r4);
  dt = 10;
  i = 30;
  j = 45;
  d = 450.0;
  se++;
  rm.makeRates(dt);
  rm.print();
  return 0;
}
