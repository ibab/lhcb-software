/*
 * AligOnlIterator.cpp
 *
 *  Created on: Oct 6, 2014
 *      Author: beat
 */

#include "GaudiKernel/Service.h"
#include "GaudiKernel/Time.h"
#include "GaudiKernel/Algorithm.h"

#include "RTL/rtl.h"


#include <boost/filesystem.hpp>

#include "GaudiKernel/IDetDataSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "DetDesc/RunChangeIncident.h"

//#include <stdio.h>
//#include <stdlib.h>

class DBXferAlg : public Algorithm
{
  public:
    DBXferAlg(const std::string& name, ISvcLocator *svcloc);
    //      StatusCode init(std::string, std::string);
    StatusCode initialize();
    StatusCode finalize();
    StatusCode stop();
    StatusCode execute();
    //      StatusCode de_init();
//    StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
  private:
    std::string m_onlinexmldir;
    std::string m_alignxmldir;
    Gaudi::Time::ValueType m_RunStartTime;
    int m_RunNumber;

};

DECLARE_ALGORITHM_FACTORY(DBXferAlg)

DBXferAlg::DBXferAlg( const std::string& name, ISvcLocator *svcloc) : Algorithm(name,svcloc)
{
  declareProperty("OnlineXmlDir", m_onlinexmldir = "/group/online/alignment");
  declareProperty("RunStartTime",m_RunStartTime);
  declareProperty("RunNumber",m_RunNumber);
}

StatusCode DBXferAlg::initialize()
{
  StatusCode sc = Algorithm::initialize();
  return sc;
}

StatusCode DBXferAlg::finalize()
{
  return Algorithm::finalize();
}

StatusCode DBXferAlg::execute()
{
  StatusCode sc = StatusCode::SUCCESS;
  // only called once

//  int runnr = 0;
  IDetDataSvc* detDataSvc(0);
  sc = service("DetectorDataSvc", detDataSvc, false);
  if (sc.isFailure())
  {
//    error() << "Could not retrieve DetectorDataSvc" << endmsg;
    return sc;
  }
  IIncidentSvc* incSvc;
  sc = service("IncidentSvc",incSvc,false);

    // 4. read ASDs and compute new constants
    detDataSvc->setEventTime(m_RunStartTime);
    incSvc->fireIncident(RunChangeIncident(name(), m_RunNumber));

  return sc;
}


StatusCode DBXferAlg::stop()
{
// called only once
  return StatusCode::SUCCESS;
}

//StatusCode DBXferAlg::queryInterface(const InterfaceID& riid,
//    void** ppvIF)
//{
//  if (LHCb::IAlignIterator::interfaceID().versionMatch(riid))
//  {
//    *ppvIF = (IAlignIterator*) this;
//    addRef();
//    return StatusCode::SUCCESS;
//  }
//  return GaudiTool::queryInterface(riid, ppvIF);
//}

