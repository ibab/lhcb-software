// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Calo/src/L0CaloMonit.cpp,v 1.4 2002-07-01 08:17:12 ocallot Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// Event/L0Event
#include "Event/L0CaloCandidate.h"

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
  MsgStream log(messageService(), name());
  log << MSG::DEBUG << " >>> Initialize" << endreq;

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
  MsgStream log(messageService(), name());
  log << MSG::DEBUG << " >>> Execute" << endreq;

  std::string containerName = L0CaloCandidateLocation::Default;
  
  SmartDataPtr< L0CaloCandidates > candidates ( eventSvc(), containerName );
  if( 0 == candidates ) { 
    log << MSG::ERROR << "Unable to retrieve " << containerName << endreq; 
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
  
  return StatusCode::SUCCESS; 
};
//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0CaloMonit::finalize() {
  
  MsgStream log(messageService(), name());
  log << MSG::DEBUG << " >>> Finalize" << endreq;
  return StatusCode::SUCCESS;
}
