// $Id: L0MuonCandidate.cpp,v 1.11 2008-04-10 13:07:10 jucogan Exp $

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

LHCb::L0MuonCandidate::L0MuonCandidate(double pt, double theta, double phi, 
                                       const std::vector<LHCb::MuonTileID> &pads, int encodedPt) {
  m_pt        = pt;
  m_theta     = theta;
  m_phi       = phi;
  m_pads      = pads;
  m_encodedPt = encodedPt;
}

std::vector<LHCb::MuonTileID> LHCb::L0MuonCandidate::muonTileIDs(unsigned int station) const {
  std::vector<LHCb::MuonTileID> mids;
  if ( station < m_pads.size() ) {
    mids = m_pads[station].layout().tiles(m_pads[station]);
  } 
  return mids;
}


bool LHCb::L0MuonCandidate::operator<(const LHCb::L0MuonCandidate& lmc) const {
  return pt() < lmc.pt();
}

bool LHCb::L0MuonCandidate::operator==(const LHCb::L0MuonCandidate& lmc) const {

  if (encodedPt()!=lmc.encodedPt()) return false;
  for (int sta=0; sta<3; ++sta){
    std::vector<LHCb::MuonTileID> p0=muonTileIDs(sta);
    std::vector<LHCb::MuonTileID> p1=lmc.muonTileIDs(sta);
    if (p0!=p1) return false;
  }

  return true;
}


