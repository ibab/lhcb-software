//	====================================================================
//  UpdateAndReset.cpp
//	--------------------------------------------------------------------
//
//	Package   : Gaucho
//
//	Author    : Eric van Herwijnen
//
//	====================================================================


// Include files
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ToolFactory.h"

#include "RTL/rtl.h"

#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "Event/ODIN.h"
#include "Event/RecHeader.h"

#include "UpdateAndReset.h"
#include "MonitorSvc.h"


// Static Factory declaration
DECLARE_ALGORITHM_FACTORY(UpdateAndReset)


// Constructor
//------------------------------------------------------------------------------
UpdateAndReset::UpdateAndReset(const std::string& name, ISvcLocator* ploc)
  : GaudiAlgorithm(name, ploc) {}

//------------------------------------------------------------------------------
StatusCode UpdateAndReset::initialize() {
  //------------------------------------------------------------------------------
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  MsgStream msg(msgSvc(), name());    
  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode UpdateAndReset::execute() {
  //------------------------------------------------------------------------------
  MsgStream         msg( msgSvc(), name() );
  

  LHCb::ODIN* odin=NULL;
  struct tm *currentTimeInfo;
  m_currentTime = time (NULL);
  currentTimeInfo = localtime(&m_currentTime);


  if (exist<LHCb::ODIN> ( LHCb::ODINLocation::Default ) ) {
     odin=get<LHCb::ODIN> ( LHCb::ODINLocation::Default ); }
  else {
       msg << MSG::WARNING<< "No ODIN Bank found. Can't update DIM services. " <<endreq;   
       return StatusCode::SUCCESS;
  }     
  int currentRunNumber = odin->runNumber();
  if (currentRunNumber != m_runNumber)
  { 
     //fast or other run change
     //update immediately and reset
     updateAll(true);
     resetHistos();
     }
  else {  
     m_timerElapsed=getTimerElapsed();
     msg << MSG::INFO << "getTimerElapsed " << m_timerElapsed << endreq;
     //check if update flag is set
     //update if so
     if (m_timerElapsed) updateAll(false);
  }
  m_runNumber = currentRunNumber;
  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode UpdateAndReset::finalize() {
  //------------------------------------------------------------------------------
  MsgStream msg(msgSvc(), name());
  msg << MSG::INFO << "finalizing...." << endreq;

  return StatusCode::SUCCESS;
}

