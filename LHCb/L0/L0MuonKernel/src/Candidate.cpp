// $Id: Candidate.cpp,v 1.1 2005-02-02 22:57:37 atsareg Exp $

#include <cmath>
#include <vector>
#include "L0MuonKernel/Candidate.h"
#include "L0MuonKernel/L0MuonBase.h"

L0Muon::Candidate::Candidate(int status) {
  m_status = status;
  setPt(0.);
  m_theta = 0.;
  m_phi = 0.;
}

L0Muon::Candidate::Candidate(double pt, 
                             double theta, 
			     double phi, 
			     const std::vector<MuonTileID>& pads, 
			     int status) {
  setPt(pt);
  m_theta = theta;
  m_phi = phi;
  std::vector<MuonTileID>::const_iterator ip;
  for(ip = pads.begin(); ip!=pads.end(); ip++ ) {
    m_pads.push_back(*ip);
  }	
  if( ! m_pads.empty() ) {
    setPadM1(m_pads[0]);
  }
  m_status = status;			 
}				 

double L0Muon::Candidate::ptPrecision() const
{
  return  m_pt ;
} 

double L0Muon::Candidate::pt() const  
{
  double rtmp = 40. * ((double)m_ptround);   
  return rtmp;
  
}
  
int L0Muon::Candidate::status() const
{
  return  m_status ;
} 


void L0Muon::Candidate::setPt(double pt) {

  unsigned int roundedPt = int((fabs(pt)+20.)/40.);
  if ( roundedPt > (( 1<< 7 ) - 1) ) {
    roundedPt = (1 << 7) - 1 ;
  }  
  
  if (pt >= 0.) { 
    m_ptround = roundedPt; 
  } else { 
    m_ptround = - roundedPt; 
  }
  
}

void L0Muon::Candidate::setStatus(int status) {
  m_status = status;
}

void L0Muon::Candidate::setPadM1(const MuonTileID& pad) {
  m_pads[0] = pad;
}

bool L0Muon::Candidate::operator<(const Candidate& lmc) const {
  return fabs(pt()) < fabs(lmc.pt());
}

MuonTileID L0Muon::Candidate::pad(unsigned int station) const {
  if ( station < m_pads.size() ) {
    return m_pads[station];
  } else {
    return MuonTileID();
  }
}

MuonTileID L0Muon::Candidate::padM1() const {
  return pad(0);
}
