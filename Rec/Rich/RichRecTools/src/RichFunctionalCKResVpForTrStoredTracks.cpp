
/** @file RichFunctionalCKResVpForTrStoredTracks.cpp
 *
 *  Implementation file for tool : RichFunctionalCKResVpForTrStoredTracks
 *
 *  $Id: RichFunctionalCKResVpForTrStoredTracks.cpp,v 1.4 2004-10-30 19:38:44 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.3  2004/10/27 14:39:41  jonrob
 *  Various updates
 *
 *  Revision 1.2  2004/07/27 20:15:30  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/10/2004
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
