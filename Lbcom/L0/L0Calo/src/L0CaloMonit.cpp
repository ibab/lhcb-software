// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Calo/src/L0CaloMonit.cpp,v 1.5 2004-04-28 06:10:05 ocallot Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

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
  : Algorithm      ( name , pSvcLocator            ) { 

}

//=============================================================================
// Standard destructor
//=============================================================================
L0CaloMonit::~L0CaloMonit() {};

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode L0CaloMonit::initialize() {
  MsgStream msg(messageService(), name());
  msg << MSG::DEBUG << " >>> Initialize" << endreq;

  std::string histoDir = "/stat/L0" ;
  std::string hName   ;

  hName             = "Et electron " ;
  m_histElectron    = histoSvc()->book( histoDir, 1001, hName, 104, 0., 5.2 );

  hName             = "Et photon " ;
  m_histPhoton      = histoSvc()->book( histoDir, 1002, hName, 104, 0., 5.2 );

  hName             = "Et hadron " ;
  m_histHadron      = histoSvc()->book( histoDir, 1003, hName, 104, 0., 5.2 );

  hName             = "Et second hadron " ;
  m_histHadron2     = histoSvc()->book( histoDir, 1004, hName, 104, 0., 5.2 );

  hName             = "Et Pi0 Local " ;
  m_histPi0Local    = histoSvc()->book( histoDir, 1005, hName, 104, 0., 5.2 );

  hName             = "Et Pi0 Global " ;
  m_histPi0Global   = histoSvc()->book( histoDir, 1006, hName, 104, 0., 5.2 );

  hName             = "Et Sum Et " ;
  m_histSumEt       = histoSvc()->book( histoDir, 1007, hName, 100, 0., 50. );

  return StatusCode::SUCCESS; 
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0CaloMonit::execute() {
  MsgStream msg(messageService(), name());
  msg << MSG::DEBUG << " >>> Execute" << endreq;

  std::string containerName = L0CaloCandidateLocation::Default;
  
  SmartDataPtr< L0CaloCandidates > candidates ( eventSvc(), containerName );
  if( 0 == candidates ) { 
    msg << MSG::ERROR << "Unable to retrieve " << containerName << endreq; 
    return StatusCode::SUCCESS;
  }   

  L0CaloCandidates::const_iterator cand;
  for ( cand = candidates->begin() ; candidates->end() != cand ; ++cand ) {
    if ( L0Calo::Electron == (*cand)->type()  ) {
      m_histElectron  -> fill( (*cand)->et()/GeV, 1. );
    } else if ( L0Calo::Photon == (*cand)->type()  ) {
      m_histPhoton    -> fill( (*cand)->et()/GeV, 1. );
    } else if ( L0Calo::Hadron == (*cand)->type()  ) {
      m_histHadron    -> fill( (*cand)->et()/GeV, 1. );
    } else if ( L0Calo::Hadron2 == (*cand)->type()  ) {
      m_histHadron2   -> fill( (*cand)->et()/GeV, 1. );
    } else if ( L0Calo::Pi0Local == (*cand)->type()  ) {
      m_histPi0Local  -> fill( (*cand)->et()/GeV, 1. );
    } else if ( L0Calo::Pi0Global == (*cand)->type()  ) {
      m_histPi0Global -> fill( (*cand)->et()/GeV, 1. );
    } else if ( L0Calo::SumEt == (*cand)->type()  ) {
      m_histSumEt     -> fill( (*cand)->et()/GeV, 1. );
    }
  }

  if (  msg.level() <= MSG::DEBUG  ) {
    // Get the L1 buffer
    L1Event* event;
    SmartDataPtr<L1Event> myEvent ( eventSvc(), L1EventLocation::Default );
    if ( 0 == myEvent ) {
      msg << MSG::DEBUG << "Access L1Buffer for decoding" << endreq;
      SmartDataPtr<L1Buffer> buffer ( eventSvc(), L1BufferLocation::Default );
      if ( 0 == buffer ) {
        msg << MSG::ERROR << "== Unable to find L1Buffer. Abort." << endreq;
        return StatusCode::SUCCESS;
      }
      event = new L1Event( buffer );
      eventSvc()->registerObject( L1EventLocation::Default, event );
    } else {
      event = myEvent;
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
        msg << MSG::DEBUG << format( "  cand %8x  w1%4x w2%4x", word, w1, w2)
            << endreq;
      }
    }
  }
 
  return StatusCode::SUCCESS; 
};
//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0CaloMonit::finalize() {
  
  MsgStream msg(messageService(), name());
  msg << MSG::DEBUG << " >>> Finalize" << endreq;
  return StatusCode::SUCCESS;
}
