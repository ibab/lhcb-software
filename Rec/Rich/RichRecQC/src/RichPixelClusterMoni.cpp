
//-----------------------------------------------------------------------------
/** @file RichPixelClusterMoni.cpp
 *
 *  Implementation file for algorithm class : Rich::Rec::PixelClusterMoni
 *
 *  $Id: RichPixelClusterMoni.cpp,v 1.2 2010-01-31 13:57:27 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   31/02/2010
 */
//-----------------------------------------------------------------------------

// local
#include "RichPixelClusterMoni.h"

// working namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PixelClusterMoni );

// Standard constructor, initializes variables
PixelClusterMoni::PixelClusterMoni( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : HistoAlgBase ( name, pSvcLocator )
{ }

// Destructor
PixelClusterMoni::~PixelClusterMoni() { }

//  Initialize
StatusCode PixelClusterMoni::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = HistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  return sc;
}

StatusCode PixelClusterMoni::prebookHistograms()
{

  for ( Rich::Detectors::const_iterator rich = Rich::detectors().begin();
        rich != Rich::detectors().end(); ++rich )
  { 
    richHisto1D( *rich,
                 "clusterSize", "Pixel Cluster Sizes",
                 -0.5, 100.5, 101 );
  }

  return StatusCode::SUCCESS;
}

// Main execution
StatusCode PixelClusterMoni::execute()
{

  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Make sure all pixels have been formed
  if ( richPixels()->empty() )
  {
    const StatusCode sc = pixelCreator()->newPixels();
    if ( sc.isFailure() )
    { return Error( "Problem creating RichRecPixels", sc ); }
    debug() << "No Pixels found : Created "
            << richPixels()->size() << " RichRecPixels" << endmsg;
  }

  // Histogramming
  const RichHistoID hid;

  // Loop over pixels
  for ( LHCb::RichRecPixels::const_iterator iP = richPixels()->begin();
        iP != richPixels()->end(); ++iP )
  {
    // Which detector
    const Rich::DetectorType rich = (*iP)->detector();

    // Associated Cluster
    const Rich::HPDPixelCluster & cluster = (*iP)->associatedCluster();

    richHisto1D( rich, "clusterSize" ) -> fill( cluster.size() );
  }

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode PixelClusterMoni::finalize()
{
  // Execute base class method
  return HistoAlgBase::finalize();
}
