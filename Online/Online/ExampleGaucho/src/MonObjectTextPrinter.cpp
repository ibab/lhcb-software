//====================================================================
//  MonObjectTextPrinter.cpp
//--------------------------------------------------------------------
//
//  Package    : MonObjectTextPrinter
//
//  Description: This program prints MonObjects published by tasks using
//               the Gaucho MonitorSvc. 
//
//  Author     : Juan Otalora
//  Created    : 26/02/2008
//  Changes    : 
//
//====================================================================
// Include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "Gaucho/DimInfoMonObject.h"
#include "MonObjectTextPrinter.h"

#include <cstring>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>

#include <time.h>

// for online monitoring
#include "GaudiKernel/IMonitorSvc.h" 
#include "GaudiKernel/IHistogramSvc.h" 
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "RTL/rtl.h"

// Static Factory declaration
DECLARE_ALGORITHM_FACTORY(MonObjectTextPrinter)

MonObjectTextPrinter::MonObjectTextPrinter(const std::string& name, ISvcLocator* ploc) : Algorithm(name, ploc) 
{
  declareProperty("svcname", m_svcname);
  m_dimInfoMonObject = 0;
}

StatusCode MonObjectTextPrinter::initialize() {
  m_refreshtime=10;
  m_monObjectCreated = false;
  StatusCode sc = Algorithm::initialize(); // mdust be executed first
  MsgStream msg(msgSvc(), name());
  if( !sc.isSuccess() )   {
    msg << MSG::FATAL << "Unable to initialize Algorithm" << endreq;
    return sc;
  }

  if (m_svcname.find("Mon", 0) == std::string::npos )
  {// nao MonObject
    std::cout << "It is not a MonObject " << std::endl;
    return StatusCode::FAILURE;
  }
  
  m_dimInfoMonObject = new DimInfoMonObject(m_svcname, m_refreshtime);
  
  return StatusCode::SUCCESS;
}

MonObjectTextPrinter::~MonObjectTextPrinter()
{
  if (m_dimInfoMonObject) {delete m_dimInfoMonObject; m_dimInfoMonObject=0;}
}

StatusCode MonObjectTextPrinter::execute() {
  MsgStream  msg( msgSvc(), name() );
   
  if (!m_monObjectCreated) 
  {
    if (m_dimInfoMonObject->createMonObject()) 
    {
      msg << MSG::INFO << "MonObject Created for service " << m_svcname << endreq;
      m_dimInfoMonObject->monObject()->print();
      m_monObjectCreated = true;
    }
    else {
      msg << MSG::WARNING << "No Service Found ==> " << m_svcname << endreq;
    }
  }
  else {
    if (m_dimInfoMonObject->loadMonObject()) 
    {
      msg << MSG::INFO << "MonObject Loaded for service " << m_svcname << endreq;
      m_dimInfoMonObject->monObject()->print();
    }
    else {
      msg << MSG::WARNING << "No Service Found ==> " << m_svcname << endreq;
    }
  }
  sleep(10);
  return StatusCode::SUCCESS;
}

StatusCode MonObjectTextPrinter::finalize() {
  MsgStream msg(msgSvc(), name());
  msg << MSG::INFO << "finalizing...." << endreq;
  return StatusCode::SUCCESS;
}


