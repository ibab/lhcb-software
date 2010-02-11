
//-----------------------------------------------------------------------------
/** @file RichPixelClusterMoni.cpp
 *
 *  Implementation file for algorithm class : Rich::Rec::PixelClusterMoni
 *
 *  $Id: RichPixelClusterMoni.cpp,v 1.4 2010-02-11 20:01:30 jonrob Exp $
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
                                    ISvcLocator* pSvcLocator )
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

  // Keep in case we need some initisation

  return sc;
}

StatusCode PixelClusterMoni::prebookHistograms()
{

  // Pre book detector level histograms
  for ( Rich::Detectors::const_iterator rich = Rich::detectors().begin();
        rich != Rich::detectors().end(); ++rich )
  { 
    richHisto1D( Rich::HistogramID( "clusterSize", *rich ), 
                 "Pixel Cluster Sizes",
                 -0.5, 100.5, 101 );
  }

  return StatusCode::SUCCESS;
}

// Main execution
StatusCode PixelClusterMoni::execute()
{

  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Histogramming
  const Rich::HistoID hid;

  // Loop over pixels
  for ( LHCb::RichRecPixels::const_iterator iP = richPixels()->begin();
        iP != richPixels()->end(); ++iP )
  {

    // Which detector
    const Rich::DetectorType rich = (*iP)->detector();

    // Associated Cluster
    const Rich::HPDPixelCluster & cluster = (*iP)->associatedCluster();
    if ( 0 == cluster.size() ) { Warning("Empty cluster !").ignore(); continue; }

    // cluster size histogram
    const double weight = 1.0 / (double)cluster.size(); // since will be filled size() times ...
    richHisto1D( Rich::HistogramID("clusterSize",rich) ) -> fill( cluster.size(), weight );
    
  }

  return StatusCode::SUCCESS;
}
