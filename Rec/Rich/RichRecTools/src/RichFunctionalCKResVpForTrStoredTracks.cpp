
/** @file RichFunctionalCKResVpForTrStoredTracks.cpp
 *
 *  Implementation file for tool : RichFunctionalCKResVpForTrStoredTracks
 *
 *  $Id: RichFunctionalCKResVpForTrStoredTracks.cpp,v 1.2 2004-07-27 20:15:30 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "RichFunctionalCKResVpForTrStoredTracks.h"

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

  // This method is un-finished - so throw an exception if called
  Exception("Unfinished implementation - use another for the moment !");

  // Reference to track ID object
  const RichTrackID & tkID = segment->richRecTrack()->trackID();

  // Check track parent type is TrStoredTrack
  if ( Rich::TrackParent::TrStoredTrack != tkID.parentType() ) {
    Exception( "Track parent type is not TrStoredTrack" );
  }

  // momentum for this segment
  //const double ptot = segment->trackSegment().bestMomentum().mag();

  return 0;
}
