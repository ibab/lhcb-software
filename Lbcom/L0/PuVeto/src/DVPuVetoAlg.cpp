// Include files 
#include <math.h>
// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ObjectVector.h"

// from L0Event
#include "Event/L0PuVeto.h"
// local
#include "DVPuVetoAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DVPuVetoAlg
//
// 22/01/2003 : Eduardo Rodrigues
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const AlgFactory<DVPuVetoAlg>           s_factory ;
const        IAlgFactory& DVPuVetoAlgFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DVPuVetoAlg::DVPuVetoAlg( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator )
  , m_nameOfOutputPileUpVeto ( L0PuVetoLocation::Default )
  , m_lowThreshold          (    3.     )
  , m_highThreshold         (    2.     )
  , m_highPosition          (    0. *mm )
  , m_secondPosition        ( -500. *mm )
{
  declareProperty( "PuVetoData"         , m_nameOfOutputPileUpVeto );
  declareProperty( "LowThreshold"       , m_lowThreshold          );
  declareProperty( "HighThreshold"      , m_highThreshold         );
  declareProperty( "HighPosition"       , m_highPosition          );
  declareProperty( "SecondPosition"     , m_secondPosition        );
}

//=============================================================================
// Destructor
//=============================================================================
DVPuVetoAlg::~DVPuVetoAlg() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode DVPuVetoAlg::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;
  log << "" << endreq;

  log << MSG::INFO << "======= Pile-up Veto settings =======" << endreq;
  log << format( "Peak low  threshold      : %5.2f", m_lowThreshold )
      << endreq;
  log << format( "Peak high threshold      : %5.2f", m_highThreshold )
      << endreq;
  log << format( "Peak high thr. position  : %5.2f mm", m_highPosition )
      << endreq;
  log << format( "Peak position cut        : %5.2f mm", m_secondPosition )
      << endreq;
  log << "=====================================" << endreq;
  log << "" << endreq;

  return StatusCode::SUCCESS;
}

  
//=============================================================================
// Main execution
//=============================================================================
StatusCode DVPuVetoAlg::execute() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;
  
  // ----------------------------------
  // Override the pile-up veto decision
  // ----------------------------------
  L0PuVeto* newL0PileUp;
  SmartDataPtr<L0PuVeto> oldL0PileUp ( eventSvc(), m_nameOfOutputPileUpVeto );
  if ( 0 == oldL0PileUp ) {
    log << MSG::ERROR << "Unable to retrieve L0PuVeto !" << endreq;
    return StatusCode::FAILURE;
  }
  else {
    int    decision;
    int    oldDecision = oldL0PileUp -> decision();
    double sum2 = oldL0PileUp -> sumPeak2();
    double pos2 = oldL0PileUp -> zPosPeak2();
    if ( (  m_lowThreshold > sum2 ) ||
         ( (m_highThreshold > sum2) && (m_highPosition > pos2) ) ||
         (  m_secondPosition > pos2 ) ) {
      decision = 0;
    }
    else {
      decision = 1;    // Multiple interaction
    }
    newL0PileUp = oldL0PileUp;
    newL0PileUp -> setDecision( decision );
    log << MSG::DEBUG
        <<" Old decision = "
        << oldDecision
        <<"; New decision = "
        << decision
        << endreq;
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode DVPuVetoAlg::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
