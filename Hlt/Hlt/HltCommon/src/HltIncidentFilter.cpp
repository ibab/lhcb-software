// $Id: HltIncidentFilter.cpp,v 1.7 2009-08-27 14:41:47 graven Exp $
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
HltIncidentFilter::HltIncidentFilter( const string& name, ISvcLocator* pSvcLocator )
    : GaudiHistoAlg( name, pSvcLocator ), m_keep( false )
{
    declareProperty( "AcceptQuota", m_quota );
    declareProperty( "HltAcceptIncident", s_incident = "RequestHltAccept" );
}
//=============================================================================
// Destructor
//=============================================================================
HltIncidentFilter::~HltIncidentFilter() {};

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltIncidentFilter::initialize()
{
    debug() << "==> Initialize" << endmsg;
    StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error printed already by GaudiAlgorithm

    IIncidentSvc* incidentSvc = svc<IIncidentSvc>( "IncidentSvc" );
    bool rethrow = false;
    bool oneShot = false;
    incidentSvc->addListener( this, s_incident, int( 0 ), rethrow, oneShot );
    incidentSvc->addListener( this, "EndEvent", int( 0 ), rethrow, oneShot );

    declareInfo( "#accept", counter( "#accept" ),
                 std::string( "Events accepted / Incidents recieved by " ) +
                     name() );

    return StatusCode::SUCCESS;
};
//=============================================================================
// Finalization
//=============================================================================
StatusCode HltIncidentFilter::finalize()
{
    StatusCode sc = GaudiHistoAlg::finalize();
    for ( std::map<std::string, stat>::const_iterator i = m_stat.begin();
          i != m_stat.end(); ++i ) {
        info() << i->first << " accepted: " << i->second.accept << " events"
               << " requested: " << i->second.request << " events " << endmsg;
    }
    return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltIncidentFilter::execute()
{
    setFilterPassed( m_keep );
    if ( msgLevel( MSG::DEBUG ) )
        debug() << "HltIncidentFilter: " << ( m_keep ? "accept" : "reject" )
                << endmsg;
    return StatusCode::SUCCESS;
};

//=============================================================================
// Incident handler
//=============================================================================
void HltIncidentFilter::handle( const Incident& incident )
{
    if ( m_keep && incident.type() == "EndEvent" ) { // only do string comparison if
                                                     // really needed...
        m_keep = false;
    } else if ( incident.type() == s_incident ) {
        stat& s = m_stat[incident.source()];
        ++s.request;     // always keep track of requests...
        if ( !m_keep ) { // only check quota if event not yet accepted...
            std::map<std::string, int>::const_iterator i =
                m_quota.find( incident.source() );
            typedef unsigned long ul;
            //@TODO: always keep 1 (and 1 only) if not listed in quota??
            m_keep = ( i != m_quota.end() &&
                       ( i->second < 0 || s.accept < ul( i->second ) ) );
        }
        if ( m_keep ) ++s.accept;
    }
    counter( "#accept" ) += m_keep;
}
