
//----------------------------------------------------------------------------------------
/** @file RichFunctionalCKResVpForTrStoredTracks.cpp
 *
 *  Implementation file for tool : RichFunctionalCKResVpForTrStoredTracks
 *
 *  $Id: RichFunctionalCKResVpForTrStoredTracks.cpp,v 1.5 2005-06-17 15:08:36 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/10/2004
 */
//----------------------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "RichFunctionalCKResVpForTrStoredTracks.h"

//----------------------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichFunctionalCKResVpForTrStoredTracks>          s_factory ;
const        IToolFactory& RichFunctionalCKResVpForTrStoredTracksFactory = s_factory ;

// Standard constructor
RichFunctionalCKResVpForTrStoredTracks::
RichFunctionalCKResVpForTrStoredTracks ( const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent )
  : RichRecToolBase( type, name, parent )
{

  // define interface
  declareInterface<IRichCherenkovResolution>(this);

  // job options

}

StatusCode RichFunctionalCKResVpForTrStoredTracks::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
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
                                                           const Rich::ParticleIDType id ) const
{

  // This method is un-finished - so throw an exception if called
  Exception("Unfinished implementation - use another for the moment !");

  if ( !segment->ckThetaResolution().dataIsValid(id) )
  {

    // Reference to track ID object
    const RichTrackID & tkID = segment->richRecTrack()->trackID();

    // Check track parent type is TrStoredTrack
    if ( Rich::TrackParent::TrStoredTrack != tkID.parentType() ) {
      Exception( "Track parent type is not TrStoredTrack" );
    }

    // momentum for this segment
    //const double ptot = segment->trackSegment().bestMomentum().mag();

    double res = 0;

    segment->setCKThetaResolution( id, res );

  }

  return segment->ckThetaResolution( id );
}
