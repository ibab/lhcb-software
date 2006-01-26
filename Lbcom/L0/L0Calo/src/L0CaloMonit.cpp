// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Calo/src/L0CaloMonit.cpp,v 1.9 2006-01-26 16:52:13 ocallot Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "Kernel/SystemOfUnits.h"

// Event/L0Event
#include "Event/L0CaloCandidate.h"
#include "Event/RawEvent.h"
#include "Event/L0Calo.h"

// local
#include "L0CaloMonit.h"

// MANDATORY!!!

static const AlgFactory<L0CaloMonit>          Factory ;
const       IAlgFactory& L0CaloMonitFactory = Factory ;
//-----------------------------------------------------------------------------
// Implementation file for class : L0CaloMonit
//
// 31/05/2001 : Olivier Callot
//-----------------------------------------------------------------------------

// Standard creator
L0CaloMonit::L0CaloMonit( const std::string& name, 
                          ISvcLocator* pSvcLocator )  
  : GaudiHistoAlg ( name , pSvcLocator ) { 

}

//=============================================================================
// Standard destructor
//=============================================================================
L0CaloMonit::~L0CaloMonit() {};

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode L0CaloMonit::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  setHistoDir( "/stat/L0" );

  m_histElectron  = book( 1001, "Et electron "   , 0., 5.2, 104 );
  m_histPhoton    = book( 1002, "Et photon "     , 0., 5.2, 104 );
  m_histHadron    = book( 1003, "Et hadron "     , 0., 5.2, 104 );
  m_histPi0Local  = book( 1004, "Et Pi0 Local "  , 0., 5.2, 104 );
  m_histPi0Global = book( 1005, "Et Pi0 Global " , 0., 5.2, 104 );
  m_histSumEt     = book( 1006, "Sum Et "        , 0., 50., 100 );
  m_histSpdMult   = book( 1007, "Spd Mult "      , 0., 500. , 100 );

  return StatusCode::SUCCESS; 
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0CaloMonit::execute() {

  debug() << "Execute" << endreq;

  std::string containerName = LHCb::L0CaloCandidateLocation::Default;
  
  LHCb::L0CaloCandidates* candidates = get<LHCb::L0CaloCandidates>( containerName );

  LHCb::L0CaloCandidates::const_iterator cand;
  for ( cand = candidates->begin() ; candidates->end() != cand ; ++cand ) {
    if ( LHCb::L0Calo::Electron == (*cand)->type()  ) {
      m_histElectron  -> fill( (*cand)->et()/GeV, 1. );
    } else if ( LHCb::L0Calo::Photon == (*cand)->type()  ) {
      m_histPhoton    -> fill( (*cand)->et()/GeV, 1. );
    } else if ( LHCb::L0Calo::Hadron == (*cand)->type()  ) {
      m_histHadron    -> fill( (*cand)->et()/GeV, 1. );
    } else if ( LHCb::L0Calo::Pi0Local == (*cand)->type()  ) {
      m_histPi0Local  -> fill( (*cand)->et()/GeV, 1. );
    } else if ( LHCb::L0Calo::Pi0Global == (*cand)->type()  ) {
      m_histPi0Global -> fill( (*cand)->et()/GeV, 1. );
    } else if ( LHCb::L0Calo::SumEt == (*cand)->type()  ) {
      m_histSumEt     -> fill( (*cand)->et()/GeV, 1. );
    } else if ( LHCb::L0Calo::SpdMult == (*cand)->type()  ) {
      m_histSpdMult   -> fill( (*cand)->etCode(), 1. );
    }
  }

  if (  msgLevel() <= MSG::DEBUG  ) {
    // Get the Raw buffer
    LHCb::RawEvent* raw = get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );
    
    std::vector<LHCb::RawBank*> banks = raw->banks( LHCb::RawBank::L0Calo );
    std::vector<LHCb::RawBank*>::const_iterator itBank;
    for ( itBank = banks.begin() ; banks.end() != itBank ; itBank++ ) {
      unsigned int* ptData = (*itBank)->data();
      int length           = (*itBank)->size();
      while ( 0 < length ) {
        unsigned int word = *ptData++;
        length--;
        
        debug() << format( " L0Calo cand %8x ", word );
        int type = (word >>24) & 0xf;
        if ( LHCb::L0Calo::SpdMult == type ) {
          debug() << " Spd Mult = " << (word & 0xFFFF);
        } else if ( LHCb::L0Calo::SumEt == type ) {
          debug() << " Hcal SumEt = " << (word&0xFFFF);
        } else {
          double et = word&0xFF;
          LHCb::CaloCellID id( (word >> 8) & 0xFFFF );
          debug() << " Type " << type << " ID " << id << " Et " << et;
        }
        debug() << endreq;
      }
    }
  }
  return StatusCode::SUCCESS; 
}
//============================================================================
