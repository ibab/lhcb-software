// $Id: L0Calo.cpp,v 1.3 2002-04-15 11:13:50 ocallot Exp $
// Include files 

#include "Event/L0CaloCandidate.h"

//-----------------------------------------------------------------------------
// Auxilliary Implementation file for L0Calo Event classes
//
// 05/04/2002 : Olivier Callot
//-----------------------------------------------------------------------------


//=========================================================================
//  Return the name of the candidate
//=========================================================================
std::string L0CaloCandidate::typeName() const {
  switch (m_type) {
  case L0Calo::Electron:  return "electron";
  case L0Calo::Photon:    return "photon  ";
  case L0Calo::Hadron:    return "hadron  ";
  case L0Calo::Hadron2:   return "hadron_2";
  case L0Calo::Pi0Local:  return "Pi0 loc ";
  case L0Calo::Pi0Global: return "Pi0 glob";
  case L0Calo::SumEt:     return "Sum Et  ";
  case L0Calo::SpdMult:   return "Spd Mult";
  default:            return "unknown ";
  }
}


//=========================================================================
// Print on Gaudi MsgStream 
//=========================================================================
MsgStream& L0CaloCandidate::fillStream( MsgStream& log ) const {

  if ( L0Calo::SpdMult == m_type ) {
    log << "L0Calo "  <<  typeName() <<  " Multiplicity " << m_etCode;
    return log;
  }

  log << "L0Calo "  <<  typeName() <<  " Et(GeV) " ;
  char line[80];
  sprintf( line, "%4d %6.2f ", m_etCode, m_et/GeV );
  log << line;
  if ( L0Calo::SumEt != m_type ) {
    log << " ID " << m_ID << " x,y,z(cm)";
    sprintf( line, "%7.1f %7.1f %7.1f Error(cm) %5.2f",
      m_position.x()/centimeter,
      m_position.y()/centimeter,
      m_position.z()/centimeter,
      m_posTol/centimeter );
      log << line; 
    }
  return log; 
}


//=========================================================================
//  Operators for printing on Gaudi Stream
//=========================================================================
MsgStream& operator<<( MsgStream& ms, const L0CaloCandidate& cand) {
  return cand.fillStream( ms );
}

MsgStream& operator<<( MsgStream&  ms, const L0CaloCandidate* cand) {
  if ( 0 != cand ) {
    ms << *cand;
  } else {
    ms << "L0CaloCandidate* points to null";
  }
  return ms;
}

//=============================================================================
