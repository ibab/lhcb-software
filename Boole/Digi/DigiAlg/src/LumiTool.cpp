// Include files
#include "LumiTool.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/SmartIF.h"
#include "Event/GenHeader.h"
#include "GaudiKernel/SystemOfUnits.h"
#include <fstream>

//--------------------------------------------------------------------
//
//  ClassName  : LumiTool
// 
//  Determines probability of interactions for a given luminosity and
//  returns a number of interactions according to this probability.
//
//  The instantaneous luminosity is taken from /Event/Gen/Header.
//
//  Author :  Marco Cattaneo  29/09/2003
//  Based on SicbLumiTool
//--------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( LumiTool )

// Standard Constructor
LumiTool::LumiTool( const std::string& type, 
                    const std::string& name,
                    const IInterface* parent ) 
  : GaudiTool( type, name, parent ) {

  declareProperty( "BunchCrossRate",    m_bunchCrossRate = 29.49 );
  declareProperty( "TotalCrossSection", m_totalXSection  = 102.4 );
  m_randSvc = 0;

  // Declare the interface that is implemented
  declareInterface<ILumiTool>(this);

}

// Destructor
LumiTool::~LumiTool() {
}

StatusCode LumiTool::initialize() 
{

  GaudiTool::initialize();

  m_randSvc = svc<IRndmGenSvc>( "RndmGenSvc", true );

  // Print the beam parameter data
  info() << "BunchCrossRate  " << m_bunchCrossRate << endmsg;
  info() << "Total X-section " << m_totalXSection  << endmsg;
  
  return StatusCode::SUCCESS;
}


StatusCode LumiTool::numInteractions( int& nEvents ) {

  nEvents = -1;

  // Get luminosity from generated event
  LHCb::GenHeader* pEvent = get<LHCb::GenHeader>( 
                                LHCb::GenHeaderLocation::Default );
  double currentLumi = pEvent->luminosity() * Gaudi::Units::cm2 
                                           * Gaudi::Units::s / 1.e32;

  if( 0. < currentLumi ) {
    if(msgLevel(MSG::DEBUG)) 
      debug() << " Luminosity of main event: " << currentLumi << endmsg;
  } 
  else { 
    err() << " Bad lumi in main event: " << currentLumi << endmsg;
    return StatusCode::FAILURE;
  }

  //  Average number of interactions per bunch crossing
  double averageInter = m_totalXSection * currentLumi / m_bunchCrossRate / 10.;
  
  SmartIF<IRndmGen> genRndmPois;
  StatusCode sc;

  sc = m_randSvc->generator(Rndm::Poisson(averageInter), genRndmPois.pRef());
  if(sc.isSuccess()){
    double fltNEvents = genRndmPois->shoot();
    double tmp = fltNEvents - (int)fltNEvents;
    if ( tmp >= 0.5){
      nEvents = (int)(fltNEvents+0.5);
    }
    else {
      nEvents = (int)(fltNEvents);
    }
    if(msgLevel(MSG::DEBUG)) debug() << " averageInter, fltNEvents, nEvents "
                                     << averageInter  << " " << fltNEvents << " "
                                     << nEvents << endmsg;
  }
  else {
    return Error( "Failed to get Poisson random number" );
  }
  
  return sc;
}
