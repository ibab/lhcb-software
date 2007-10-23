
//-----------------------------------------------------------------------------
/** @file RichGlobalPIDAlgBase.cpp
 *
 *  Implementation file for RICH Global PID algorithm base class : Rich::Rec::GlobalPID::AlgBase
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDAlgBase.cpp,v 1.11 2007-10-23 10:43:07 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-05-10
 */
//-----------------------------------------------------------------------------

// local
#include "RichGlobalPIDAlgBase.h"

// namespaces
using namespace Rich::Rec::GlobalPID;

//-----------------------------------------------------------------------------

// Standard constructor, initializes variables
AlgBase::AlgBase( const std::string& name,
                  ISvcLocator* pSvcLocator )
  : Rich::Rec::AlgBase ( name , pSvcLocator ),
    m_GPIDtracks       ( NULL ),
    m_GPIDSummary      ( NULL ),
    m_GPIDs            ( NULL ),
    m_richGPIDTrackLocation   ( LHCb::RichGlobalPIDTrackLocation::Default ),
    m_richGPIDSummaryLocation ( LHCb::RichGlobalPIDSummaryLocation::Default ),
    m_richGPIDLocation        ( LHCb::RichGlobalPIDLocation::Default ),
    m_richGPIDName            ( "RichGloPID" ),
    m_richPartProp     ( NULL )
{

  if      ( context() == "Offline" )
  {
    m_richGPIDName = "RichGloPID";
    m_richGPIDTrackLocation   = LHCb::RichGlobalPIDTrackLocation::Offline;
    m_richGPIDSummaryLocation = LHCb::RichGlobalPIDSummaryLocation::Offline;
    m_richGPIDLocation        = LHCb::RichGlobalPIDLocation::Offline;
  }
  else if ( context() == "HLT" )
  {
    m_richGPIDName = "RichGloHltPID";
    m_richGPIDTrackLocation   = LHCb::RichGlobalPIDTrackLocation::HLT;
    m_richGPIDSummaryLocation = LHCb::RichGlobalPIDSummaryLocation::HLT;
    m_richGPIDLocation        = LHCb::RichGlobalPIDLocation::HLT;
  }

  declareProperty( "RichGlobalPIDTrackLocation", m_richGPIDTrackLocation );
  declareProperty( "RichGlobalPIDLocation", m_richGPIDLocation );
  declareProperty( "RichGlobalPIDSummaryLocation", m_richGPIDSummaryLocation );
  declareProperty( "GlobalName", m_richGPIDName );
  declareProperty( "ProcStatusLocation",
                   m_procStatLocation = LHCb::ProcStatusLocation::Default );

}

//  Initialize
StatusCode AlgBase::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = Rich::Rec::AlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // get tools
  acquireTool( "RichParticleProperties",  m_richPartProp );
  
  // PID types
  m_pidTypes = m_richPartProp->particleTypes();
  info() << "Particle types considered                 = " << m_pidTypes << endreq;
 
  return sc;
}

// Destructor
AlgBase::~AlgBase() { }
