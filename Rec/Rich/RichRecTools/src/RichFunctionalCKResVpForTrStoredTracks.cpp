// $Id: RichFunctionalCKResVpForTrStoredTracks.cpp,v 1.1 2004-04-19 23:06:09 jonesc Exp $

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "RichFunctionalCKResVpForTrStoredTracks.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichFunctionalCKResVpForTrStoredTracks
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichFunctionalCKResVpForTrStoredTracks>          s_factory ;
const        IToolFactory& RichFunctionalCKResVpForTrStoredTracksFactory = s_factory ;

// Standard constructor
RichFunctionalCKResVpForTrStoredTracks::RichFunctionalCKResVpForTrStoredTracks ( const std::string& type,
                                                                                 const std::string& name,
                                                                                 const IInterface* parent )
  : RichRecToolBase( type, name, parent )
{
  declareInterface<IRichCherenkovResolution>(this);
}

StatusCode RichFunctionalCKResVpForTrStoredTracks::initialize()
{
  // Sets up various tools and services
  StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }


  return StatusCode::SUCCESS;
}

StatusCode RichFunctionalCKResVpForTrStoredTracks::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

double
RichFunctionalCKResVpForTrStoredTracks::ckThetaResolution( RichRecSegment * segment,
                                                           const Rich::ParticleIDType /* id */ ) const
{

  // Reference to track ID object
  const RichTrackID & tkID = segment->richRecTrack()->trackID();

  // Check track parent type is TrStoredTrack
  if ( Rich::TrackParent::TrStoredTrack != tkID.parentType() ) {
    Exception( "Track parent type is not TrStoredTrack" );
  }

  // momentum for this segment
  //const double ptot = segment->trackSegment().bestMomentum().mag();

  // return randomn variable for the moment
  return 0.0001;
}
