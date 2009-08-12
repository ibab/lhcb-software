// $Id: L0CaloCandidate.cpp,v 1.7 2009-08-12 09:16:24 ibelyaev Exp $
// Include files 
#include <vector>
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
  case L0DUBase::CaloType::Electron:  return "electron";
  case L0DUBase::CaloType::Photon:    return "photon  ";
  case L0DUBase::CaloType::Hadron:    return "hadron  ";
  case L0DUBase::CaloType::Pi0Local:  return "Pi0 loc ";
  case L0DUBase::CaloType::Pi0Global: return "Pi0 glob";
  case L0DUBase::CaloType::SumEt:     return "Sum Et  ";
  case L0DUBase::CaloType::SpdMult:   return "Spd Mult";
  case L0DUBase::CaloType::HadronSlave1Out:   return "Hadron Slave1 Out";
  case L0DUBase::CaloType::HadronSlave2Out:   return "Hadron Slave2 Out";
  case L0DUBase::CaloType::HadronSlave1In:   return "Hadron Slave1 In";
  case L0DUBase::CaloType::HadronSlave2In:   return "Hadron Slave2 In";
  case L0DUBase::CaloType::SumEtSlave1Out:   return "SumEt Slave1 Out";
  case L0DUBase::CaloType::SumEtSlave2Out:   return "SumEt Slave2 Out";
  case L0DUBase::CaloType::SumEtSlave1In:     return "SumEt Slave1 In";
  case L0DUBase::CaloType::SumEtSlave2In:   return "SumEt Slave2 In";
  default:                      return "unknown ";
  }
}
//=========================================================================
// Print on Gaudi MsgStream 
//=========================================================================
MsgStream& LHCb::L0CaloCandidate::fillStream( MsgStream& msg ) const {

  if ( L0DUBase::CaloType::SpdMult == m_type ) {
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
