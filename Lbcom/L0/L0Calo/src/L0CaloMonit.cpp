// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Calo/src/L0CaloMonit.cpp,v 1.6 2005-01-12 09:19:38 ocallot Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// Event/L0Event
#include "Event/L0CaloCandidate.h"
#include "Event/L1Event.h"

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

  std::string containerName = L0CaloCandidateLocation::Default;
  
  L0CaloCandidates* candidates = get<L0CaloCandidates>( containerName );

  L0CaloCandidates::const_iterator cand;
  for ( cand = candidates->begin() ; candidates->end() != cand ; ++cand ) {
    if ( L0Calo::Electron == (*cand)->type()  ) {
      m_histElectron  -> fill( (*cand)->et()/GeV, 1. );
    } else if ( L0Calo::Photon == (*cand)->type()  ) {
      m_histPhoton    -> fill( (*cand)->et()/GeV, 1. );
    } else if ( L0Calo::Hadron == (*cand)->type()  ) {
      m_histHadron    -> fill( (*cand)->et()/GeV, 1. );
    } else if ( L0Calo::Pi0Local == (*cand)->type()  ) {
      m_histPi0Local  -> fill( (*cand)->et()/GeV, 1. );
    } else if ( L0Calo::Pi0Global == (*cand)->type()  ) {
      m_histPi0Global -> fill( (*cand)->et()/GeV, 1. );
    } else if ( L0Calo::SumEt == (*cand)->type()  ) {
      m_histSumEt     -> fill( (*cand)->et()/GeV, 1. );
    } else if ( L0Calo::SpdMult == (*cand)->type()  ) {
      m_histSumEt     -> fill( (*cand)->etCode(), 1. );
    }
  }

  if (  msgLevel() <= MSG::DEBUG  ) {
    // Get the L1 buffer
    L1Event* event;
    if ( exist<L1Event>( L1EventLocation::Default ) ) {
      event = get<L1Event>( L1EventLocation::Default );
    } else {      
      debug() << "Access L1Buffer for decoding" << endreq;
      L1Buffer* buffer = get<L1Buffer>( L1BufferLocation::Default );
      event = new L1Event( *buffer );
      put( event, L1EventLocation::Default );
    }

    std::vector<L1Bank> banks = event->banks( L1Buffer::L0Calo );
    std::vector<L1Bank>::const_iterator itBank;
    for ( itBank = banks.begin() ; banks.end() != itBank ; itBank++ ) {
      l1_int* ptData = (*itBank).data();
      int length     = (*itBank).dataSize();
      while ( 0 < length ) {
        int w1 = (*ptData++);
        int w2 = (*ptData++) & 0xFFFF;
        length -= 2;
        int word = ( w1 << 16) | w2;
        debug() << format( " L1 buffer cand %8x  w1%4x w2%4x", word, w1, w2);
        int type = (w1 >>8) & 0xf;
        if ( L0Calo::SpdMult == type ) {
          debug() << " Spd Mult = " << w2;
        } else if ( L0Calo::SumEt == type ) {
          debug() << " Hcal SumEt = " << w2;
        } else {
          double et = w2&0xFF;
          CaloCellID id( (word >> 8) & 0xFFFF );
          debug() << " Type " << type << " ID " << id << " Et " << et;
        }
        debug() << endreq;
      }
    }
  }
  return StatusCode::SUCCESS; 
}
//============================================================================
