
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
  if ( !exist<LHCb::RichRecRings>(m_ringLoc) ) return StatusCode::SUCCESS;
  const LHCb::RichRecRings * rings = get<LHCb::RichRecRings>( m_ringLoc );

  // loop
  Rich::Map<Rich::RadiatorType,unsigned int> ringsPerRad;
  for ( LHCb::RichRecRings::const_iterator iR = rings->begin();
        iR != rings->end(); ++iR )
  {
    // Radiator info
    const Rich::RadiatorType rad = (*iR)->radiator();

    // only bother with RICH1 gas rings (searching for aerogel) at this time
    if ( rad != Rich::Rich1Gas ) continue;

    // Find the ring centre point on the HPD panel
    const Gaudi::XYZPoint & centreP = (*iR)->centrePointLocal();

    // refit the ring ...
    FastRingFitter fitter(**iR);
    // run the fit
    fitter.fit();
    const bool refitOK = ( fitter.result().Status == 0 &&
                           fitter.result().Variance < m_maxFitVariance[rad] );

    if ( refitOK )
    {

      // Loop over hits in the same rich HPD panel
      const IPixelCreator::PixelRange pixels = pixelCreator()->range((*iR)->rich(),(*iR)->panel());
      for ( IPixelCreator::PixelRange::const_iterator iPix = pixels.begin();
            iPix != pixels.end(); ++iPix )
      {
        // Get the centre - hit separation
        double sep = std::sqrt( std::pow( centreP.X() - (*iPix)->localPosition().X(), 2 ) +
                                std::pow( centreP.Y() - (*iPix)->localPosition().Y(), 2 ) );

        // normalise to ring radius on HPD plane
        sep /= fitter.result().Radius;

        // plot the result
        richHisto1D( Rich::HistogramID("normRadii",(*iR)->rich()) )->fill(sep);

      } // loop over pixels

    } // ring refit OK

  } // loop over rings

  return StatusCode::SUCCESS;
}
