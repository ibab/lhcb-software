// $Id: L0MuonCandidate.cpp,v 1.5 2002-07-24 17:04:42 atsareg Exp $

#include <cmath>
#include <vector>
#include "Event/L0MuonCandidate.h"
#include "L0MuonBase.h"

L0MuonCandidate::L0MuonCandidate(int status) {
  setStatus(status);
}

L0MuonCandidate::L0MuonCandidate(double pt, 
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
  setStatus(status);			 
}				 

double L0MuonCandidate::pt() const
{

  int tmp = (m_bitcode & L0MuonBase::MaskPt) >> L0MuonBase::ShiftPt;
  double rtmp = L0MuonBase::PT_BIN_WIDTH * ((double)tmp);

  int sign = (m_bitcode & L0MuonBase::MaskPtSign)
               >> L0MuonBase::ShiftPtSign;
  return  sign ? rtmp : -rtmp ;

} 

int L0MuonCandidate::status() const
{
  int tmp = (m_bitcode & L0MuonBase::MaskStatus) >> L0MuonBase::ShiftStatus;
  return  tmp ;
} 

void L0MuonCandidate::setBit(int value, 
                             int shift, 
			     int mask) {
  int tmp1 , tmp2 ;
  tmp1 = ( value << shift ) & mask ; 
  tmp2 = m_bitcode & ~mask ; 
  m_bitcode = ( tmp1 | tmp2 ) ;			     
}

void L0MuonCandidate::setPt(double pt) {

  if ( pt > 0.) {
    setBit(1,L0MuonBase::ShiftPtSign,L0MuonBase::MaskPtSign);
  } else {
    setBit(0,L0MuonBase::ShiftPtSign,L0MuonBase::MaskPtSign);
  }
  unsigned int roundedPt = int((fabs(pt)+L0MuonBase::PT_BIN_WIDTH/2.)/
                                              L0MuonBase::PT_BIN_WIDTH);
  if ( roundedPt > (( 1<< L0MuonBase::BitsPt ) - 1) ) {
    roundedPt = (1 << L0MuonBase::BitsPt) - 1 ;
  }  
  
  // cout << "L0MuonCandidate::setPt " << pt << " " << roundedPt << endl;
  
  setBit(roundedPt,L0MuonBase::ShiftPt,L0MuonBase::MaskPt);
  
}

void L0MuonCandidate::setStatus(int status) {
  setBit(status,L0MuonBase::ShiftStatus,L0MuonBase::MaskStatus);
}

void L0MuonCandidate::setPadM1(const MuonTileID& pad) {
  setBit(pad.region(),L0MuonBase::ShiftRegion,L0MuonBase::MaskRegion);
  setBit(pad.nX(),L0MuonBase::ShiftX,L0MuonBase::MaskX);
  setBit(pad.nY(),L0MuonBase::ShiftY,L0MuonBase::MaskY);
}

bool L0MuonCandidate::operator<(const L0MuonCandidate& lmc) const {
  return pt() < lmc.pt();
}

MuonTileID L0MuonCandidate::pad(unsigned int station) const {
  if ( station < m_pads.size() ) {
    return m_pads[station];
  } else {
    return MuonTileID();
  }
}

MuonTileID L0MuonCandidate::padM1() const {
  return pad(0);
}
