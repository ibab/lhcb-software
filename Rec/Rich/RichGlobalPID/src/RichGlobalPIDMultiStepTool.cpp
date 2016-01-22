
//--------------------------------------------------------------------------
/** @file RichGlobalPIDMultiStepTool.cpp
 *
 *  Implementation file for RICH Global PID tool : Rich::Rec::GlobalPID::MultiStepTool
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2008-03-01
 */
//--------------------------------------------------------------------------

// local
#include "RichGlobalPIDMultiStepTool.h"

// namespaces
using namespace Rich::Rec::GlobalPID;

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MultiStepTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MultiStepTool::MultiStepTool( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : Rich::Rec::GlobalPID::ToolBase ( type, name, parent )
{
  // interfaces
  declareInterface<IRichGlobalPID> ( this );
  declareInterface<IRichPID>       ( this );
  // JOs
  declareProperty( "NSteps", m_nSteps = 2 );
}

//=============================================================================
// Destructor
//=============================================================================
MultiStepTool::~MultiStepTool() {}

//  Initialize
StatusCode MultiStepTool::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = Rich::Rec::GlobalPID::ToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // setup tools
  acquireTool( "GPIDTrackCreator", m_gtkCreator );
  for ( unsigned int iStep = 0; iStep<m_nSteps; ++iStep )
  {
    std::ostringstream bkgEstiName, pidLikeName;
    const IPixelBackgroundEsti * bkgEst(0);
    bkgEstiName << "BackEsti" << iStep;
    m_bckTools.push_back( acquireTool( "PixelBackgroundTool",
                                       bkgEstiName.str(),
                                       bkgEst, this ) );
    const IRichGlobalPID * pidLike(0);
    pidLikeName << "Likelihood" << iStep;
    m_pidTools.push_back( acquireTool( "GPIDLikelihoodTool",
                                       pidLikeName.str(),
                                       pidLike, this ) );
  }

  // pre-cache tools
  trackCreator();
  photonCreator();
  pixelCreator();

  return sc;
}

//=============================================================================

const LHCb::RichPID * MultiStepTool::pid( const LHCb::Track * track ) const
{
  // do we already have a GPID object for this track
  LHCb::RichGlobalPID * gpid = findGPID(track);
  if ( gpid && gpid->track() == track )
  {
    debug() << "Found pre-existing PID object for track " << track->key() << endmsg;
    return gpid;
  }

  // make/get the working RICH track
  auto * rtrack = trackCreator()->newTrack(track);

  // make a GPID track
  auto * gtrack = ( rtrack ? m_gtkCreator->createTrack(rtrack) : nullptr );

  // do the PID and return
  return ( gtrack ? this->pid(gtrack) : nullptr );
}

void MultiStepTool::pids( const LHCb::Track::ConstVector & tracks,
                          LHCb::RichPID::ConstVector & richpids ) const
{
  // purge output PID container
  richpids.clear();
  richpids.reserve(tracks.size());

  // make a local container of Global PID tracks
  LHCb::RichGlobalPIDTrack::Vector gtracks;
  gtracks.reserve(tracks.size());
  for ( const auto * T : tracks )
  {
    // do we already have a GPID object for this track
    LHCb::RichGlobalPID * gpid = findGPID(T);
    if ( gpid && gpid->track() == T )
    {
      debug() << "Found pre-existing PID object for track " << T->key() << endmsg;
    }
    else
    {
      // Need to PID this track

      // make/get the working RICH track
      auto * rtrack = trackCreator()->newTrack(T);

      // make a GPID track
      auto * gtrack = ( rtrack ? m_gtkCreator->createTrack(rtrack) : nullptr );

      // add to list of tracks to PID
      if ( gtrack ) gtracks.push_back( gtrack );

      // update PID pointer
      gpid = ( gtrack ? gtrack->globalPID() : nullptr );
    }

    // Add PID to returned list
    richpids.push_back(gpid);

  } // loop over tracks

  // PID these tracks
  if (!gtracks.empty()) this->pids(gtracks);
}

//=============================================================================

LHCb::RichGlobalPID * MultiStepTool::pid( LHCb::RichGlobalPIDTrack * track ) const
{
  // make sure pixels are decoded
  // currently all or nothing... Maybe need region-on-demand later on ??
  if ( pixelCreator()->newPixels() )
  {

    // Turn 'off' all tracks. Selected tracks are turned back on later
    for ( auto * rtk : *richTracks() ) { rtk->setInUse(false); }

    // turn on the selected track
    track->richRecTrack()->setInUse(true);

    // make sure this track has its photons
    photonCreator()->reconstructPhotons(track->richRecTrack());

    // Run the various steps on this track
    for ( unsigned int iStep = 0; iStep<m_nSteps; ++iStep )
    {
      m_bckTools[iStep]->computeBackgrounds( track->richRecTrack() );
      m_pidTools[iStep]->pid( track );
    }

    // finalise the PID object
    m_gtkCreator->finaliseTrack(track);

  }

  // return the PID object
  return track->globalPID();
}

void MultiStepTool::pids( const LHCb::RichGlobalPIDTrack::Vector & tracks ) const
{
  // make sure pixels are decoded
  // currently all or nothing... Maybe need region-on-demand later on ??
  if ( pixelCreator()->newPixels() )
  {

    // Turn 'off' all tracks. Selected tracks are turned back on later
    for ( auto * rtk : *richTracks() ) { rtk->setInUse(false); }

    // make a local vector of RichRecTracks
    LHCb::RichRecTrack::Vector rtracks;
    rtracks.reserve(tracks.size());
    for ( auto * gT : tracks )
    {
      rtracks.push_back( gT->richRecTrack() );
      gT->richRecTrack()->setInUse(true);
      photonCreator()->reconstructPhotons(gT->richRecTrack());
    }

    // PID these tracks
    for ( unsigned int iStep = 0; iStep<m_nSteps; ++iStep )
    {
      m_bckTools[iStep]->computeBackgrounds( rtracks );
      m_pidTools[iStep]->pids( tracks );
    }

    // Finalise the PIDs
    for ( auto * gT : tracks ) { m_gtkCreator->finaliseTrack(gT); }

  }
}

//=============================================================================

LHCb::RichGlobalPID * MultiStepTool::findGPID( const LHCb::Track * track ) const
{
  LHCb::RichGlobalPID * gpid = nullptr;

  if ( track )
  {
    // First try using same track key convention
    gpid = gpidPIDs()->object(track->key());
    if ( !gpid || gpid->track() != track )
    {
      
      // Key convention failed, so try direct search
      gpid = nullptr;
      for ( auto * ipid : *gpidPIDs() )
      {
        if ( ipid->track() == track ) { gpid = ipid; break; }
      }

    } // not found via key

  } // track OK

  return gpid;
}

//=============================================================================
