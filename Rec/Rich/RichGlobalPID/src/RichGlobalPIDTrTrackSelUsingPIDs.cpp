
//--------------------------------------------------------------------------
/** @file RichGlobalPIDTrTrackSelUsingPIDs.cpp
 *
 *  Implementation file for RICH Global PID algorithm class : 
 *  Rich::Rec::GlobalPID::TrackSelUsingPIDs
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   10/12/2007
 */
//--------------------------------------------------------------------------

// local
#include "RichGlobalPIDTrTrackSelUsingPIDs.h"

// From GaudiKernel
#include "GaudiKernel/SystemOfUnits.h"

// boost
#include "boost/assign/list_of.hpp"

// namespaces
using namespace Rich::Rec::GlobalPID;

//--------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( TrackSelUsingPIDs )

// Standard constructor, initializes variables
TrackSelUsingPIDs::TrackSelUsingPIDs( const std::string& name,
                                      ISvcLocator* pSvcLocator )
  : TrackSel ( name, pSvcLocator ),
    m_pids   ( NULL )
{
  declareProperty( "RichPIDLocation",
                   m_richPIDLocation = LHCb::RichPIDLocation::Default );
  
  //                                               El   Mu   Pi   Ka   Pr
  std::vector<double> tmp = boost::assign::list_of(9e9)(9e9)(9e9)(000)(000);
  declareProperty( "DLLCuts", m_dllCuts = tmp);
}

// Destructor
TrackSelUsingPIDs::~TrackSelUsingPIDs() {}

//  Initialize
StatusCode TrackSelUsingPIDs::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = TrackSel::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Print out Dll cuts
  info() << "PID information from " << m_richPIDLocation
         << " DLL cuts = " << m_dllCuts << endmsg;

  return sc;
}

StatusCode TrackSelUsingPIDs::eventInit()
{
  m_pids = NULL;
  return TrackSel::eventInit();
}

bool TrackSelUsingPIDs::trackSelection( LHCb::RichRecTrack * track ) const
{
  // first run base class selection
  if ( !track || !TrackSel::trackSelection(track) ) return false;

  // Pointer to parent Track
  const LHCb::Track * pTrack = 
    dynamic_cast<const LHCb::Track*>(track->parentTrack());
  if ( !pTrack ) return false;

  // Do RichPID selection
  if ( !richPIDs() ) return false;
  const LHCb::RichPID * pid = richPIDs()->object( track->key() );
  if ( !pid || pid->track() != pTrack )
  {
    pid = NULL;
    for ( LHCb::RichPIDs::const_iterator iPID = richPIDs()->begin();
          iPID != richPIDs()->end(); ++iPID )
    {
      if ( (*iPID)->track() == pTrack ) { pid = *iPID; break; }
    }
  }

  // Did we find a pid object
  if ( !pid ) return false;

  // double check
  if ( track->parentTrack() != pid->track() )
  {
    Warning( "Problem finding RichPID for given track" );
    return false;
  }

  // Apply Dll selection cuts
  if ( pid->particleDeltaLL(Rich::Electron) > m_dllCuts[Rich::Electron] ||
       pid->particleDeltaLL(Rich::Muon)     > m_dllCuts[Rich::Muon]     ||
       pid->particleDeltaLL(Rich::Pion)     > m_dllCuts[Rich::Pion]     ||
       pid->particleDeltaLL(Rich::Kaon)     > m_dllCuts[Rich::Kaon]     ||
       pid->particleDeltaLL(Rich::Proton)   > m_dllCuts[Rich::Proton]
       ) return true;

  // if we get here, track is rejected
  return false;
}
