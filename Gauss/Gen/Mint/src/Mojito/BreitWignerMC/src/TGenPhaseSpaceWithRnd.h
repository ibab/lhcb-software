#ifndef TGENPHASESPACE_WITH_RND_HH
#define TGENPHASESPACE_WITH_RND_HH

#include "TGenPhaseSpace.h"
#include "TRandom.h"

class TGenPhaseSpaceWithRnd : public TGenPhaseSpace{
  TRandom* _rnd;
 public:
 TGenPhaseSpaceWithRnd(TRandom* rnd=gRandom) : _rnd(rnd){}
  void setRandom(TRandom* rnd=gRandom){_rnd = rnd;}
  Double_t Generate(){
    TRandom* oldGRandom = gRandom;
    gRandom = _rnd;
    Double_t returnVal = TGenPhaseSpace::Generate();
    gRandom = oldGRandom;
    return returnVal;
  }
};

#endif
//
