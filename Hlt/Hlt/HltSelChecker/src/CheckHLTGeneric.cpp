// $Id: CheckHLTGeneric.cpp,v 1.1 2005-01-27 15:55:20 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "CheckHLTGeneric.h"
#include "Event/L1Score.h"
//-----------------------------------------------------------------------------
// Implementation file for class : CheckHLTGeneric
//
// 2005-01-21 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<CheckHLTGeneric>          s_factory ;
const        IAlgFactory& CheckHLTGenericFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CheckHLTGeneric::CheckHLTGeneric( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
{
  declareProperty( "ScoreLocation" ,
                   m_scoreLocation = "/Event/Trig/HltGenericScore" );
}
//=============================================================================
// Destructor
//=============================================================================
CheckHLTGeneric::~CheckHLTGeneric() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CheckHLTGeneric::initialize() {

  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode CheckHLTGeneric::execute() {

  debug() << "==> Execute" << endmsg;

  bool passed = false ;
  
  // code goes here  
  if (exist<L1Score>(m_scoreLocation)){
    L1Score* score = get<L1Score>(m_scoreLocation);
    passed = score->decision();
  }      

  setFilterPassed(passed);   // Mandatory. Set to true if event is accepted.
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CheckHLTGeneric::finalize() {

  debug() << "==> Finalize" << endmsg;

  return  StatusCode::SUCCESS;
}

//=============================================================================
