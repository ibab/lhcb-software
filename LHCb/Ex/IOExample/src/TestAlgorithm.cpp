//	====================================================================
//  TestAlgorithm.cpp
//	--------------------------------------------------------------------
//
//	Package   : Tests/LHCb
//
//	Author    : Markus Frank
//
//	====================================================================

// Framework include files
#include "TestAlgorithm.h"
#include "Event/EventHeader.h"
#include "Event/MCParticle.h"


/** User example objects: TestAlgorithm

    Description:
    A small class reading a track N-tuple from a root file.

    Base Class:
    Algorithm

    Author:  M.Frank
    Version: 1.0
*/
/// Constructor: A constructor of this form must be provided.
TestAlgorithm::TestAlgorithm(const std::string& name, ISvcLocator* pSvcLocator)  
: Algorithm(name, pSvcLocator), m_nerr(0), m_numEvent(0)    {
}
/// Standard Destructor
TestAlgorithm::~TestAlgorithm()    {
}
/// Initialize
StatusCode TestAlgorithm::initialize()   {
  return initializeTest();
}

/// initialize callback
StatusCode TestAlgorithm::initializeTest()   {
  return StatusCode::SUCCESS;
}

/// Finalize
StatusCode TestAlgorithm::finalize() {
  MsgStream log(msgSvc(), name());
  if ( m_nerr == 0 )    {
    log << MSG::ALWAYS << "Test SUCCESSFULLY performed! ";
    log << "Executed " << m_numEvent << " Events." 
        << endreq;
  }
  else    {
    log << MSG::FATAL  << "Test FAILED... found " << m_nerr << " Failures. ";
    log << "Executed " << m_numEvent << " Events WITH ERRORS." 
        << endreq;
    for ( Errors::iterator i = m_errorList.begin(); i != m_errorList.end(); i++ )   {
      log << MSG::FATAL << "Line:";
      log.width(6);
      log << (*i).second << " " << (*i).first << endreq;
    }
  }
  return finalizeTest();
}

/// Finalize callback
StatusCode TestAlgorithm::finalizeTest()   {
  return StatusCode::SUCCESS;
}

/// Event callback
StatusCode TestAlgorithm::execute()   {
  m_numEvent++;
  SmartDataPtr<EventHeader> evt(eventSvc(),"/Event/Header");
  if ( evt )    {
    m_event = evt;
    return executeTest();
  }
  else  {
    if ( evt )   {
      return executeTest();
    }
  }
  error(__FILE__, "Cannot retrieve basic EVENT HEADER object.", __LINE__);
  return StatusCode::FAILURE;
}

/// Event callback
StatusCode TestAlgorithm::executeTest()   {
  return StatusCode::SUCCESS;
}

long TestAlgorithm::error(const std::string& desc, long lineNo)   {
  bool found = false;
  for ( Errors::iterator i = m_errorList.begin(); i != m_errorList.end(); i++ )   {
    if ( (*i).first == desc )   {
      found = true;
      break;
    }
  }
  if ( !found )   {
    MsgStream log(msgSvc(), name());
    log << MSG::FATAL << desc << endmsg;
    m_errorList.push_back(std::pair<std::string, long>(desc,lineNo));
  }
  m_nerr++;
  return m_nerr;
}

long TestAlgorithm::error(const std::string& file, long lineNo, const std::string& desc)   {
  return error(file, desc, lineNo);
}

long TestAlgorithm::error(const std::string& file, const std::string& desc, long lineNo)   {
  std::string msg = file;
  msg += ": ";
  msg += desc;
  return error(msg, lineNo);
}

float TestAlgorithm::rndmFloat(float mi, float ma)   {
  float  f = float(rand());
  float  mx = RAND_MAX;
  float  iv = ma - mi;
  float  mm = mi;
  return mm + iv * f/mx;
}

int TestAlgorithm::rndmInt(int mi, int ma)   {
  return int(rndmFloat(float(mi), float(ma)));
}

DataObject* TestAlgorithm::retrieveObject(IDataProviderSvc* svc, const std::string& path, const char* fname, long line)    {
  DataObject* pO;
  if ( !svc->retrieveObject(path,pO).isSuccess() )    {
    std::string err = "retrieveObject(\"";
    err += path;
    err += "\",pO).";
    error(fname,err,line);
  }
  return pO;
}

StatusCode 
TestAlgorithm::registerEvtObj(const std::string& path, 
                              DataObject* pObj, 
                              const char* fname, 
                              long line)
{
  StatusCode sc = eventSvc()->registerObject(path, pObj);
  if ( !sc.isSuccess() )  {
    std::string err = "retrieveObject(\"";
    err += path;
    err += "\",pO) of type ";
    err += System::typeinfoName(typeid(*pObj));
    error(fname,err,line);
    delete pObj;
  }
  return sc;
}

#include "GaudiKernel/AlgFactory.h"
static const AlgFactory<TestAlgorithm>  Factory;
const IAlgFactory& TestAlgorithmFactory = Factory;
