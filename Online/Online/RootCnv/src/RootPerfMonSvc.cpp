// $Id: RootPerfMonSvc.cpp,v 1.12 2010-09-27 15:43:53 frankb Exp $
//====================================================================
//  RootPerfMonSvc implementation
//--------------------------------------------------------------------
//
//  Description: Implementation of the ROOT data storage
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/DataIncident.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/System.h"
#include "RootCnv/RootPerfMonSvc.h"
#include "RootUtils.h"

using namespace std;
using namespace Gaudi;
typedef const string& CSTR;

#define S_OK   StatusCode::SUCCESS
#define S_FAIL StatusCode::FAILURE

// Standard constructor
RootPerfMonSvc::RootPerfMonSvc(CSTR nam, ISvcLocator* svc)
  : Service( nam, svc), m_incidentSvc(0)
{
  declareProperty("IOPerfStats",      m_ioPerfStats);
}

// Standard destructor
RootPerfMonSvc::~RootPerfMonSvc() {
}

// Small routine to issue exceptions
StatusCode RootPerfMonSvc::error(CSTR msg)  {
  if ( m_log ) {
    log() << MSG::ERROR << "Error: " << msg << endmsg;
    return S_FAIL;
  }
  MsgStream m(msgSvc(),name());
  m << MSG::ERROR << "Error: " << msg << endmsg;
  return S_FAIL;
}

// Initialize the Db data persistency service
StatusCode RootPerfMonSvc::initialize()  {
  string cname;
  StatusCode status = Service::initialize();
  if ( !status.isSuccess() )
    return error("Failed to initialize Service base class.");
  m_log = new MsgStream(msgSvc(),name());
  if( !(status=service("IncidentSvc", m_incidentSvc)).isSuccess() )
    return error("Unable to localize interface from service:IncidentSvc");
  return S_OK;
}

// Finalize the Db data persistency service
StatusCode RootPerfMonSvc::finalize()    {
  log() << MSG::INFO;
  deletePtr(m_log);
  releasePtr(m_incidentSvc);
  return Service::finalize();
}
