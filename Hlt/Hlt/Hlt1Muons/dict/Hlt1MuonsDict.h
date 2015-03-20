#ifndef DICT_HLT1MUONSDICT_H 
#define DICT_HLT1MUONSDICT_H 1

#include <vector>

#include "Hlt1Muons/Candidate.h"
// #include "Hlt1Muons/Hlt1MuonHit.h"
#include "Hlt1Muons/IMatchVeloMuon.h"

namespace 
{
  struct _Instantiations 
  {
    // begin instantiations
    std::vector<Candidate*>         _i1;
    std::vector<const Candidate*>   _i2;
    // std::vector<Hlt1MuonHit*>       _i3;
    // std::vector<const Hlt1MuonHit*> _i4;
    // end instantiations
  };
}


#endif // DICT_HLT1MUONSDICT_H
