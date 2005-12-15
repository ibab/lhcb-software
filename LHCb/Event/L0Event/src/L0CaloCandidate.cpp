// $Id: L0CaloCandidate.cpp,v 1.1 2005-12-15 08:35:42 ocallot Exp $
// Include files 

#include "Kernel/SystemOfUnits.h"

#include "Event/L0CaloCandidate.h"
#include "Event/L0Calo.h"

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
  case LHCb::L0Calo::Electron:  return "electron";
  case LHCb::L0Calo::Photon:    return "photon  ";
  case LHCb::L0Calo::Hadron:    return "hadron  ";
  case LHCb::L0Calo::Hadron2:   return "hadron_2";
  case LHCb::L0Calo::Pi0Local:  return "Pi0 loc ";
  case LHCb::L0Calo::Pi0Global: return "Pi0 glob";
  case LHCb::L0Calo::SumEt:     return "Sum Et  ";
  case LHCb::L0Calo::SpdMult:   return "Spd Mult";
  default:                      return "unknown ";
  }
}
//=========================================================================
// Print on Gaudi MsgStream 
//=========================================================================
MsgStream& LHCb::L0CaloCandidate::fillStream( MsgStream& msg ) const {

  if ( LHCb::L0Calo::SpdMult == m_type ) {
    msg << "L0CaloCandidate "  <<  typeName() <<  " Multiplicity " << m_etCode;
    return msg;
  }
  msg << "L0CaloCandidate "  <<  typeName() 
      <<  format( " Et %4d GeV %6.2f ID ", m_etCode, m_et/GeV ) ;
  if ( L0Calo::SumEt != m_type ) {
    msg << " ID " << m_ID << format( "%7.1f %7.1f %7.1f Error(cm) %5.2f",
                                     m_position.x()/centimeter,
                                     m_position.y()/centimeter,
                                     m_position.z()/centimeter,
                                     m_posTol/centimeter );
  }
  return msg; 
}


//=========================================================================
//  Operators for printing on Gaudi Stream
//=========================================================================
MsgStream& operator<<( MsgStream& ms, const LHCb::L0CaloCandidate& cand) {
  return cand.fillStream( ms );
}

MsgStream& operator<<( MsgStream& ms, const LHCb::L0CaloCandidate* cand) {
  if ( 0 != cand ) {
    return cand->fillStream( ms );
  }
  ms << "L0CaloCandidate* points to null";
  return ms;
}

//=============================================================================
