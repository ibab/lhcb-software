// Include files
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DataObject.h"
#include "Gaucho/MonStatEntity.h"
#include "Gaucho/MonRate.h"

#include "RateService.h"
#include "DimInfoMonRate.h"
#include "Gaucho/DimInfoMonObject.h"

#include "debugMacro.h"


#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IHistogram3D.h"
#include "AIDA/IProfile1D.h"
#include "AIDA/IProfile2D.h"
#include "AIDA/IAxis.h"

#include "RTL/rtl.h"

#include <cstring>
#include <string>
#include <iostream>
#include <vector>

#include <cmath>

#ifdef WIN32
namespace win {
#include <windows.h>
}
# define mysleep(x) win::Sleep(x) 
#else
# include <unistd.h>
# define mysleep(x) usleep(x*1000000) 
#endif


// Static Factory declaration
DECLARE_ALGORITHM_FACTORY(RateService)
  
  // Constructor
  //------------------------------------------------------------------------------
  RateService::RateService(const std::string& name, ISvcLocator* ploc)
    : Algorithm(name, ploc),
    m_dimName(name),
    m_found(false),
    m_dimInfoMonRate(0),
    m_numberOfMonRatesPublisher(0)
{
  m_monRateServiceName = "*";
  m_nbCounterInMonRate = 0;
  sleepTime=5;
  
  declareProperty("MonRateCounterNumber", m_nbCounterInMonRate);
  declareProperty("SleepTime",sleepTime); // Sleeping time between events, in seconds
  declareProperty("MonRateServiceName", m_monRateServiceName);
  
}

//------------------------------------------------------------------------------
StatusCode RateService::initialize() {
  //------------------------------------------------------------------------------
  StatusCode sc = Algorithm::initialize(); // must be executed first
  MsgStream msg(msgSvc(), name());
  
  
  COUT_DEBUG("MonRateServiceName   : " << m_monRateServiceName);
  COUT_DEBUG("MonRateCounterNumber : " << m_nbCounterInMonRate);
  COUT_DEBUG("SleepTime            : " << sleepTime);
  
  msg << MSG::INFO << "Initialize" << endreq;

  m_UTGID = RTL::processName();
  
  try
  {  
    sc = findServices();
    if(!sc.isSuccess())
    {
      msg << MSG::INFO << "Unable to find dim services." << endreq;
    }
    
    m_numberOfMonRatesPublisher = new RatePublisher();
    m_numberOfMonRatesPublisher->setValue(0);
    m_numberOfMonRatesPublisher->setComment("Number of MonRates processed by this RateService");
    m_numberOfMonRatesPublisher->publishService(m_UTGID + "/NumberOfMonRates");
    
  }catch(const std::exception & e)
  {
    msg << MSG::WARNING << "EXCEPTION CAUGHT IN RateService::initialize() : "
    << e.what() << endreq;
  }
  
  sc = StatusCode::SUCCESS;
  
  msg << MSG::INFO << "Initialize DONE" << endreq;
  
  return sc;
}

bool RateService::isHandledService(std::string serviceName)
{
  for(std::vector<DimInfoMonRate *>::iterator it = m_dimInfoMonRate.begin();
      it != m_dimInfoMonRate.end();
      it++)
  {
    if( *it && serviceName == (*it)->getServiceName())
      return true;
  }
  
  return false;
}

//------------------------------------------------------------------------------
StatusCode RateService::findServices() {
  //------------------------------------------------------------------------------
  MsgStream msg(msgSvc(), name());
  StatusCode sc;
  
  int nbNewServices = 0;
  
  msg << MSG::INFO << "Looking for " << m_monRateServiceName << " pattern" << endreq;
  
  
  //DimClient::setDnsNode(m_dimClientDns.c_str());
  DimBrowser dbr;
  
  // actual name and format of the found service
  char * serviceNameC;
  char * formatC;
  
  dbr.getServices( m_monRateServiceName.c_str() );

  while( 0 != dbr.getNextService(serviceNameC, formatC) )
  {
    msg << MSG::INFO << " ### Matching service" << endreq;
    msg << MSG::INFO << " ### Service Name : " << serviceNameC << " ; format : " <<
    formatC << endreq;
    
    if(isHandledService(serviceNameC))
      continue;
      
    bool exceptionRaised = false;
    
    DimInfoMonRate * pNew = 0;
    
    try
    {
      pNew = new DimInfoMonRate(serviceNameC, 5, m_UTGID, m_nbCounterInMonRate);
      
      if(pNew)
        pNew->setMsgService(msgSvc());
    
      msg << MSG::DEBUG << "creating MonRate" << endreq;

      /* create the MonDouble to receive the data
       */    
      try{
        pNew->createMonRate();
      }catch(const std::exception & e){
        msg << MSG::INFO << __LINE__ << ": error in createMonRate() : " << e.what() << endreq;
	exceptionRaised = true;
        try{
	  if(pNew) delete pNew;
	}catch(const std::exception & e)
	{
	  msg << MSG::FATAL << __LINE__ << ": fatal 1 :" << e.what() << endreq;
	  exit(1);
	}
      }

    }
    /* some exception will be probably due to a non MonRate service
     */
    catch(std::exception e)
    {
      msg << MSG::WARNING << " >>> an error occured while registering MonRate" << endreq;
      msg << MSG::WARNING << " >>> in RateService::findServices()" << endreq;
      msg << MSG::WARNING << " >>> can't register " << serviceNameC << endreq;
      exceptionRaised = true;
      try{
        if(pNew) delete pNew;
      }catch(const std::exception & e)
      {
         msg << MSG::FATAL << "fatal 2 : " << e.what() << endreq;
         exit(2);
      }
    }
    
    /* if nothing has been caught then it's OK
     */
    if(!exceptionRaised)
    {
      m_dimInfoMonRate.push_back(pNew);
      sc = StatusCode::SUCCESS;
      nbNewServices++; 
    }
          
  }
  
  msg << MSG::INFO << nbNewServices << " found" << endreq;
  
  
  msg << MSG::INFO << "Looking for DONE" <<endreq;
  
  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode RateService::execute() {
  //------------------------------------------------------------------------------
  MsgStream         msg( msgSvc(), name() );
  
//  msg << MSG::INFO << "Execute" << endreq;

    /* looking for new service
     */
  try
  {  
    if(!findServices().isSuccess())
    {
      msg << MSG::INFO << "Unable to find " << m_monRateServiceName << endreq;
    }
    
    m_numberOfMonRatesPublisher->updateService(m_dimInfoMonRate.size());
  }catch(std::exception e)
  {
   msg << MSG::WARNING << "EXCEPTION CAUGHT INT RateService::execute()" << endreq;
  }
  
//  msg << MSG::INFO << "Execute DONE, found=" << m_found << endreq;
  
  mysleep(sleepTime);
   
  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode RateService::finalize() {
  //------------------------------------------------------------------------------
  MsgStream msg(msgSvc(), name());
  msg << MSG::INFO << "finalizing...." << endreq;
  

  for(std::vector<DimInfoMonRate *>::iterator it = m_dimInfoMonRate.begin();
      it != m_dimInfoMonRate.end();
      it++)
  {
    if( *it ) delete (*it);
  }
  
  if(m_numberOfMonRatesPublisher)
    delete m_numberOfMonRatesPublisher;
  
  return StatusCode::SUCCESS;
}

