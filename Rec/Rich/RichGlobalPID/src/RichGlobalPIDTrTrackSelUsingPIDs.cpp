
//--------------------------------------------------------------------------
/** @file RichGlobalPIDTrTrackSelUsingPIDs.cpp
 *
 *  Implementation file for RICH Global PID algorithm class : Rich::Rec::GlobalPID::TrackSelUsingPIDs
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDTrTrackSelUsingPIDs.cpp,v 1.1 2007-12-10 17:38:07 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   10/12/2007
 */
//--------------------------------------------------------------------------

// local
#include "RichGlobalPIDTrTrackSelUsingPIDs.h"

// From GaudiKernel
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// boost
#include "boost/assign/list_of.hpp"

// namespaces
using namespace Rich::Rec::GlobalPID;

//--------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( TrackSelUsingPIDs );

// Standard constructor, initializes variables
TrackSelUsingPIDs::TrackSelUsingPIDs( const std::string& name,
                                      ISvcLocator* pSvcLocator )
  : TrackSel ( name, pSvcLocator ),
    m_pids   ( NULL )
{
  declareProperty( "RichPIDLocation",
                   m_richPIDLocation = LHCb::RichPIDLocation::Default );
  declareProperty( "DLLCuts",                  //     El   Mu   Pi   Ka   Pr
                   m_dllCuts = boost::assign::list_of(9e9)(9e9)(9e9)(000)(000) );
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
         << " DLL cuts = " << m_dllCuts << endreq;

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
  if ( !TrackSel::trackSelection(track) ) return false;

  // Do RichPID selection
  if ( !richPIDs() ) return false;
  const LHCb::RichPID * pid = richPIDs()->object( track->key() );

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
