#include "GaudiKernel/SvcFactory.h"
#include "StagerChronoSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/ISvcLocator.h"

#include "GaudiKernel/SvcFactory.h"

#include <sys/time.h>
#include <sys/types.h>

// Static Factory declaration
using namespace Gaudi;
DECLARE_SERVICE_FACTORY(StagerChronoSvc)

StagerChronoSvc::StagerChronoSvc(const std::string& nam, ISvcLocator* svcLoc) :
    base_class(nam,svcLoc)
    ,m_incidentSvc(0)
    ,m_beginInputFile(0)
    ,m_endInputFile(0)
    ,m_totalWaitTime(0)
    ,m_totalProcessingTime(0)
{
  //------------------------------------------------------------------------------

}

StatusCode
StagerChronoSvc::initialize()
{
  StatusCode status = Service::initialize();
  MsgStream log(msgSvc(), "StagerChronoSvc");
  StatusCode sc = service("IncidentSvc", m_incidentSvc, true);
  if (!sc.isSuccess())
  {
    log << MSG::ERROR << "Failed to access incident service." << endmsg;
    return StatusCode::FAILURE;
  }
  m_incidentSvc->addListener(this, IncidentType::BeginInputFile);
  m_incidentSvc->addListener(this, IncidentType::EndInputFile);
  //   m_incidentSvc->addListener(this, "BEGIN_SETUP_NEXT_FILE");
  //   m_incidentSvc->addListener(this, "END_SETUP_NEXT_FILE");

  //for ETCs

  log << MSG::DEBUG << "Added listeners on begin and end of input files." << endmsg;

  log << MSG::INFO << "StagerChronoSvc:Initialize() successful" << endmsg;

  return StatusCode::SUCCESS;
}


void
StagerChronoSvc::handle(const Incident& inc)
{
  MsgStream log(msgSvc(), "StagerChronoSvc");
  log << MSG::INFO << "Handling incident '" << inc.type() << "'" << endmsg;
  log << MSG::INFO << "Incident source '" << inc.source() << "'" << endmsg;

  if (inc.type() == IncidentType::BeginInputFile)
  {

    // Time stamp before the computations
    gettimeofday( &tp, NULL );
    sec   = static_cast<double>( tp.tv_sec );
    usec = static_cast<double>( tp.tv_usec )/1E6;
    m_beginInputFile = sec + usec;
    log << MSG::INFO << "Waiting time for "<< inc.source() << endmsg;
    log << MSG::INFO << "[EndInputFile(prev) - BeginInputFile (current)] = "
        << (m_beginInputFile - m_endInputFile)<< " seconds" << endmsg;
    m_totalWaitTime = m_totalWaitTime + (m_beginInputFile-m_endInputFile);
  }
  else if (inc.type() == IncidentType::EndInputFile)
  {
    gettimeofday( &tp, NULL );
    sec = static_cast<double>( tp.tv_sec );
    usec = static_cast<double>( tp.tv_usec )/1E6;
    m_endInputFile = sec + usec;
    log << MSG::INFO << "Processing time for "<< inc.source() << endmsg;
    log << MSG::INFO << "[BeginInputFile(current)-EndInputFile(current)] = "
        << (m_endInputFile-m_beginInputFile) << " seconds" << endmsg;
    m_totalProcessingTime = m_totalProcessingTime + (m_endInputFile-m_beginInputFile);
  }
}

StagerChronoSvc::~StagerChronoSvc()
{}

StatusCode StagerChronoSvc::finalize()
{
  // /------------------------------------------------------------------------------
  MsgStream log(msgSvc(), "StagerChronoSvc");

  log << MSG::INFO <<  "StagerChronoSvc Finalize() successful" << endmsg;
  log << MSG::INFO << "Total waiting time due to staging: "<< m_totalWaitTime << " seconds" <<endmsg;
  return StatusCode::SUCCESS;
}
