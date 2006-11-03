// $Id: L0CaloCandidate.cpp,v 1.4 2006-11-03 14:58:50 cattanem Exp $
// Include files 

#include "GaudiKernel/SystemOfUnits.h"

#include "Event/L0CaloCandidate.h"
#include "Event/L0DUBase.h"

//-----------------------------------------------------------------------------
// Auxilliary Implementation file for L0CaloCandidate Event classes
//
// 05/04/2002 : Olivier Callot
//-----------------------------------------------------------------------------


//=========================================================================
//  Return the name of the candidate
//=========================================================================
std::string LHCb::L0CaloCandidate::typeName() const {
  switch (m_type) {
  case L0DUBase::Fiber::CaloElectron:  return "electron";
  case L0DUBase::Fiber::CaloPhoton:    return "photon  ";
  case L0DUBase::Fiber::CaloHadron:    return "hadron  ";
  case L0DUBase::Fiber::CaloPi0Local:  return "Pi0 loc ";
  case L0DUBase::Fiber::CaloPi0Global: return "Pi0 glob";
  case L0DUBase::Fiber::CaloSumEt:     return "Sum Et  ";
  case L0DUBase::Fiber::CaloSpdMult:   return "Spd Mult";
  default:                      return "unknown ";
  }
}
//=========================================================================
// Print on Gaudi MsgStream 
//=========================================================================
MsgStream& LHCb::L0CaloCandidate::fillStream( MsgStream& msg ) const {

  if ( L0DUBase::Fiber::CaloSpdMult == m_type ) {
    msg << "L0CaloCandidate "  <<  typeName() <<  " Multiplicity " << m_etCode;
    return msg;
  }
  msg << "L0CaloCandidate "  <<  typeName() 
      <<  format( " Et %4d GeV %6.2f ID ", m_etCode, m_et/Gaudi::Units::GeV ) ;
  if ( L0DUBase::Fiber::CaloSumEt != m_type ) {
    msg << " ID " << m_ID << format( "%7.1f %7.1f %7.1f Error(cm) %5.2f",
                                     m_position.x()/Gaudi::Units::centimeter,
                                     m_position.y()/Gaudi::Units::centimeter,
                                     m_position.z()/Gaudi::Units::centimeter,
                                     m_posTol/Gaudi::Units::centimeter );
  }
  return msg; 
}


//=========================================================================
//  Operators for printing on Gaudi Stream
//=========================================================================
MsgStream& LHCb::operator<<( MsgStream& ms, const LHCb::L0CaloCandidate& cand) {
  return cand.fillStream( ms );
}

MsgStream& LHCb::operator<<( MsgStream& ms, const LHCb::L0CaloCandidate* cand) {
  if ( 0 != cand ) {
    return cand->fillStream( ms );
  }
  ms << "L0CaloCandidate* points to null";
  return ms;
}

//=============================================================================
