
//--------------------------------------------------------------------------
/** @file RichRecRingPeakSearch.cpp
 *
 *  Implementation file for algorithm class : Rich::Rec::RingPeakSearch
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */
//--------------------------------------------------------------------------

// local
#include "RichRecRingPeakSearch.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RingPeakSearch )

// Standard constructor, initializes variables
RingPeakSearch::RingPeakSearch( const std::string& name,
                                ISvcLocator* pSvcLocator )
  : HistoAlgBase ( name, pSvcLocator )
{
  using namespace boost::assign;

  setProperty( "NBins1DHistos", 50 );

  declareProperty( "RingLocation", m_ringLoc = "Rec/Rich/Markov/RingsIsolated",
                   "The TES location of RichRecRings" );
  declareProperty( "MaxFitVariance",
                   m_maxFitVariance = std::vector<double>(3, 100) );

}

// Destructor
RingPeakSearch::~RingPeakSearch() {}

StatusCode RingPeakSearch::prebookHistograms()
{
  richHisto1D( Rich::HistogramID("normRadii",Rich::Rich1), 
               "Hit Separation to Ring Centre", 0, 10, nBins1D() );
  return StatusCode::SUCCESS;
}

StatusCode RingPeakSearch::execute()
{
  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Load the rings
  const auto * rings = getIfExists<LHCb::RichRecRings>( m_ringLoc );
  if ( !rings ) return StatusCode::SUCCESS;

  // loop
  Rich::Map<Rich::RadiatorType,unsigned int> ringsPerRad;
  for ( const auto & ring : *rings )
  {
    // Radiator info
    const Rich::RadiatorType rad = ring->radiator();

    // only bother with RICH1 gas rings (searching for aerogel) at this time
    if ( rad != Rich::Rich1Gas ) continue;

    // Find the ring centre point on the HPD panel
    const Gaudi::XYZPoint & centreP = ring->centrePointLocal();

    // refit the ring ...
    FastRingFitter fitter(*ring);
    // run the fit
    fitter.fit();
    const bool refitOK = ( fitter.result().Status == 0 &&
                           fitter.result().Variance < m_maxFitVariance[rad] );

    if ( refitOK )
    {

      // Loop over hits in the same rich HPD panel
      const auto pixels = pixelCreator()->range(ring->rich(),ring->panel());
      for ( const auto * pixel : pixels )
      {
        // Get the centre - hit separation
        double sep = std::sqrt( std::pow( centreP.X() - pixel->localPosition().X(), 2 ) +
                                std::pow( centreP.Y() - pixel->localPosition().Y(), 2 ) );

        // normalise to ring radius on HPD plane
        sep /= fitter.result().Radius;

        // plot the result
        richHisto1D( Rich::HistogramID("normRadii",ring->rich()) )->fill(sep);

      } // loop over pixels

    } // ring refit OK

  } // loop over rings

  return StatusCode::SUCCESS;
}
