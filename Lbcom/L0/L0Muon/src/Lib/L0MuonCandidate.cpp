// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/src/Lib/L0MuonCandidate.cpp,v 1.1 2001-06-07 16:35:56 atsareg Exp $

#include "L0Muon/L0MuonCandidate.h"

extern const CLID& CLID_L0MuonCandidate = 6002 ;

L0mPad* L0MuonCandidate::pad(unsigned int station) {
  if( station < m_pads.size() ) {
    return m_pads[station]; 
  } else {
    return 0;
  }  
}
