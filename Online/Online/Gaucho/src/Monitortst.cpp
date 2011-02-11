/*
 * Monitortst.cpp
 *
 *  Created on: Feb 10, 2011
 *      Author: beat
 */

//      ====================================================================
//      UpdateAndReset.cpp
//      --------------------------------------------------------------------
//
//      Package   : Gaucho
//
//      Author    : Eric van Herwijnen
//      Should be run as first algorithm in TopAlg
//      Checks:
//      1) If the run number has changed. If so, updateAll(true) and resetHistos
//      2) If timerelapsed flag is true. If so, updateAll(false)
//
//      ====================================================================


// Include files
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/LinkManager.h"


#include "GaudiKernel/ServiceLocatorHelper.h"
#include "GaudiKernel/IDataManagerSvc.h"

#include "RTL/rtl.h"

#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "MDF/OnlineRunInfo.h"


#include "Monitortst.h"
#include "MonitorSvc.h"
#include "RTL/rtl.h"


// Static Factory declaration
DECLARE_ALGORITHM_FACTORY(Monitortst)

// Constructor
//------------------------------------------------------------------------------
Monitortst::Monitortst(const std::string& name, ISvcLocator* ploc)
  : GaudiAlgorithm(name, ploc)
{
}


//------------------------------------------------------------------------------
StatusCode Monitortst::initialize() {
//------------------------------------------------------------------------------
  MsgStream msg( msgSvc(), name() );
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  for (int i =0;i<100;i++)
  {
    cntr[i] = i;
    char s[2048];
    sprintf(s,"Counter_%d",i);
    cntrName[i] =  std::string(s);
    sprintf(s,"Counter With Index %d",i);
    cntrComm[i] = std::string(s);
    declareInfo(cntrName[i],cntr[i],cntrComm[i]);
  }
  return StatusCode::SUCCESS;
}

StatusCode Monitortst::start()
{

  StatusCode sc = GaudiAlgorithm::start(); // must be executed first
  return StatusCode::SUCCESS;
}
//------------------------------------------------------------------------------
StatusCode Monitortst::execute()
{
//------------------------------------------------------------------------------
  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode Monitortst::stop() {
//------------------------------------------------------------------------------
  StatusCode sc = GaudiAlgorithm::stop(); // must be executed first
  return StatusCode::SUCCESS;
}
StatusCode Monitortst::finalize() {
//------------------------------------------------------------------------------
  StatusCode sc = GaudiAlgorithm::finalize(); // must be executed first

  return StatusCode::SUCCESS;
}

