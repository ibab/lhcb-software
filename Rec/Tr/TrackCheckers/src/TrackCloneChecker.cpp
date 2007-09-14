// $Id: TrackCloneChecker.cpp,v 1.1 2007-09-14 13:01:15 jonrob Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "TrackCloneChecker.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackCloneChecker
//
// 2007-09-13 : Chris Jones
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrackCloneChecker );

//=============================================================================
// Standard constructor
//=============================================================================
TrackCloneChecker::TrackCloneChecker( const std::string& name,
                                      ISvcLocator* pSvcLocator )
  : TrackCheckerBase ( name , pSvcLocator )
{
  declareProperty( "CloneLinkerLocation",
                   m_cloneInfoTES = LHCb::TrackLocation::Default+"Clones");
}

//=============================================================================
// Destructor
//=============================================================================
TrackCloneChecker::~TrackCloneChecker() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TrackCloneChecker::execute()
{

  // Get the track clone linker info
  LinkedFrom<LHCb::Track,LHCb::Track> linker( evtSvc(), msgSvc(), m_cloneInfoTES );
  if ( linker.notFound() )
  {
    return Warning("Failed to load Track clone information from "+m_cloneInfoTES,
                   StatusCode::SUCCESS);
  }

  // get the input data
  LHCb::Tracks* tracks = get<LHCb::Tracks>(inputContainer());

  // set up stuff for this event for base class
  initializeEvent();

  // loop over tracks
  for ( LHCb::Tracks::const_iterator iTk = tracks->begin();
        iTk != tracks->end(); ++iTk )
  {

    // MCP for main track
    const LHCb::MCParticle * mcP = mcTruth(*iTk);
    if ( !mcP || !selected(mcP) ) continue;

    // pick up the clone info for this track
    LHCb::Track * cloneTrack = linker.first( *iTk );
    while ( cloneTrack != NULL )
    {

      // KL distance
      const double logFLdist = log10(linker.weight());
      
      // MCP for clone track
      const LHCb::MCParticle * mcP_clone = mcTruth(cloneTrack);
      const bool mcSel = ( mcP_clone ? selected(mcP_clone) : false );
      if ( mcP_clone && mcSel )
      {
        if ( mcP == mcP_clone )
        {
          plot1D( logFLdist, "KLDtrueClones", "Log10(KLDistance) | True Clones", -5, 13, 100 );
        }
        else
        {
          plot1D( logFLdist, "KLDnotClones", "Log10(KLDistance) | Not Clones", -5, 13, 100 );
        }
      }
      else if ( mcP_clone && !mcSel )
      {
        plot1D( logFLdist, "KLDrejMCPs", "Log10(KLDistance) | Rejected MCParticles", -5, 13, 100 );
      }
      else
      {
        plot1D( logFLdist, "KLDghosts", "Log10(KLDistance) | Ghosts", -5, 13, 100 );
      }

      cloneTrack = linker.next();
    } // clone track

  }

  return StatusCode::SUCCESS;
}

//=============================================================================
