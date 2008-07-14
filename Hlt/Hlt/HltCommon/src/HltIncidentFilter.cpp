// $Id: HltIncidentFilter.cpp,v 1.1 2008-07-14 07:15:49 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IIncidentSvc.h"

// local
#include "HltIncidentFilter.h"

using namespace std;
using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltIncidentFilter
//
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltIncidentFilter );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltIncidentFilter::HltIncidentFilter( const string& name,
                    ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
  , m_keep(false)
{
    declareProperty("AcceptQuota", m_quota );
    declareProperty("HltAcceptIncident", s_incident = "RequestHltAccept" );
}
//=============================================================================
// Destructor
//=============================================================================
HltIncidentFilter::~HltIncidentFilter() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltIncidentFilter::initialize() {
  debug() << "==> Initialize" << endmsg;
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  IIncidentSvc *incidentSvc = svc<IIncidentSvc>("IncidentSvc");
  // add listener to be triggered by first BeginEvent with low priority
  // so it gets called first
  bool rethrow = false;
  bool oneShot = false;
  incidentSvc->addListener(this,s_incident,int(0),rethrow,oneShot);
  incidentSvc->addListener(this,"EndEvent",int(0),rethrow,oneShot);

  registerSelection();

  return StatusCode::SUCCESS;
};
//=============================================================================
// Finalization
//=============================================================================
StatusCode HltIncidentFilter::finalize() {
    StatusCode sc = HltAlgorithm::finalize();
    for (std::map<std::string,stat>::const_iterator i = m_stat.begin(); i!= m_stat.end(); ++i) {
        info() << i->first 
               << " requested: " << i->second.request << " events " 
               << " accepted: "  << i->second.accept << " events"
               << endmsg;
    }
    return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltIncidentFilter::execute() {
  setDecision(m_keep);
  if (msgLevel(MSG::DEBUG)) debug() << "HltIncidentFilter: " << (m_keep?"accept":"reject") << endreq;
  return StatusCode::SUCCESS;
};

void HltIncidentFilter::handle(const Incident& incident) {
    if ( m_keep && incident.type() == "EndEvent") { // only do string comparison if really needed...
        m_keep = false; 
    } else if ( incident.type() == s_incident )  {
        stat& s = m_stat[incident.source()];
        ++s.request; // always keep track of requests...
        if (!m_keep) { // only check quota if event not yet accepted...
            std::map<std::string,int>::const_iterator i = m_quota.find(incident.source());
            typedef unsigned long ul;
            m_keep = ( i!=m_quota.end() && ( i->second < 0 || s.accept < ul(i->second) ) );
        }
        if (m_keep) ++s.accept;
    }
}
