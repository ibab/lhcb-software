// $Id: L0MuonCandidate.cpp,v 1.2 2001-07-26 13:05:37 cattanem Exp $

#include "L0Muon/L0MuonCandidate.h"


L0mPad* L0MuonCandidate::pad(unsigned int station) {
  if( station < m_pads.size() ) {
    return m_pads[station]; 
  } else {
    return 0;
  }  
}
