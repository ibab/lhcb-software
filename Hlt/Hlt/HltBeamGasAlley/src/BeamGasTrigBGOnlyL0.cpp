// $Id: BeamGasTrigBGOnlyL0.cpp,v 1.1.1.1 2008-07-15 13:53:25 phopchev Exp $
#include "GaudiKernel/AlgFactory.h"
#include "Event/ODIN.h"
#include "BeamGasTrigBGOnlyL0.h"


//-----------------------------------------------------------------------------
// Implementation file for class : BeamGasTrigBGOnlyL0
//
// 2008-06-16 : Plamen Hopchev <plamen.hopchev@cern.ch>
//
// The algorithm checks if we have b1-empty or empty-b2 bunch crossing.
// It rejects the event also if we don't confirm the L0 positive decision
// (whatever it comes from (PU for b2 and SPD for b1?)).
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( BeamGasTrigBGOnlyL0 );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BeamGasTrigBGOnlyL0::BeamGasTrigBGOnlyL0(const std::string& name, ISvcLocator* pSvcLocator)
  : HltAlgorithm(name, pSvcLocator)
{
  declareProperty("L0DULocation", m_l0Location = LHCb::L0DUReportLocation::Default );
  declareProperty("ODINLocation", m_odinLocation = "DAQ/ODIN");
  declareProperty("BXTypeAccept1", m_bxType1 = 1 );
  declareProperty("BXTypeAccept2", m_bxType2 = 2 );
}

//=============================================================================
// Destructor
//=============================================================================
BeamGasTrigBGOnlyL0::~BeamGasTrigBGOnlyL0() {;}

//=============================================================================
// Initialization
//=============================================================================
StatusCode BeamGasTrigBGOnlyL0::initialize()
{
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if (sc.isFailure()) return sc;  // error printed already by HltAlgorithm

  debug() << "==> Initialize" << endmsg;
  
  registerSelection();

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode BeamGasTrigBGOnlyL0::execute()
{
  debug() << "==> Execute" << endmsg;
  
  // Get The L0 Decision and reject the event if it is negative.
  if( exist<LHCb::L0DUReport>(m_l0Location) )
  {
    LHCb::L0DUReport* l0 = get<LHCb::L0DUReport>(m_l0Location);   
    bool l0Decision = l0->decision();
    debug() << " L0 decision " << l0Decision << endreq;
    if( !l0Decision ) 
    { 
      setDecision(false);
      return StatusCode::SUCCESS; 
    }  
  }
  else 
  {  fatal() << "Can't find L0 decision !!!" << endmsg;
     return StatusCode::FAILURE; }
     
  
  // Get the bunch crossing type from ODIN and
  // reject the event if it is not b1-empty or empty-b2 
  if( exist<LHCb::ODIN>(m_odinLocation) )
  { 
    LHCb::ODIN* odin = get<LHCb::ODIN>(m_odinLocation);
    int bXType = (int)odin->bunchCrossingType();   // 1=SingleBeamLeft, 2=SingleBeamRight
    bool singleBeam = ( bXType==m_bxType1 || bXType==m_bxType2 );
    //info() << "ODIN BX Type = " << odin->bunchCrossingType() 
    //       << "( " << (int)( odin->bunchCrossingType() ) << " )"
    //       << "run Number = " << odin->runNumber() << endmsg;	
    
    if( !singleBeam ) 
    {      
      setDecision(false);
      return StatusCode::SUCCESS; 
    }  
  }
  else
  { fatal() << "Can't access ODIN !!!" << endmsg;
    return StatusCode::FAILURE; }
    
  
  //If we come to here, then we have L0-Yes and BX-type with single beam ==>   
  setDecision(true);
 
  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode BeamGasTrigBGOnlyL0::finalize()
{
  debug() << "==> Finalize" << endmsg;

  return HltAlgorithm::finalize();  // must be called after all other actions
}


