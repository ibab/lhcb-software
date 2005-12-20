// $Id: L0MuonCandidate.cpp,v 1.9 2005-12-20 13:47:37 ocallot Exp $

#include <cmath>
#include <vector>
#include "Event/L0MuonCandidate.h"

LHCb::L0MuonCandidate::L0MuonCandidate() : 
                 m_pt(0.0),
                 m_theta(0.0),
                 m_phi(0.0),
					       m_pads(),
                 m_encodedPt(0){
}

LHCb::L0MuonCandidate::L0MuonCandidate(double pt, double theta, double phi, const std::vector<MuonTileID> &pads, int encodedPt) {
  m_pt        = pt;
  m_theta     = theta;
  m_phi       = phi;
  m_pads      = pads;
  m_encodedPt = encodedPt;
}				 


LHCb::MuonTileID LHCb::L0MuonCandidate::pad(unsigned int station) const {
  if ( station < m_pads.size() ) {
    return m_pads[station];
  } else {
    return MuonTileID();
  }
}


bool LHCb::L0MuonCandidate::operator<(const LHCb::L0MuonCandidate& lmc) const {
  return pt() < lmc.pt();
}

