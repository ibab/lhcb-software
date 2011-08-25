
//--------------------------------------------------------------------------
/** @file RichRecBackgroundEstiClustering.cpp
 *
 *  Implementation file for algorithm class : Rich::Rec::BackgroundEstiClustering
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */
//--------------------------------------------------------------------------

// local
#include "RichRecBackgroundEstiClustering.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( BackgroundEstiClustering )

// Standard constructor, initializes variables
BackgroundEstiClustering::BackgroundEstiClustering( const std::string& type,
                                                    const std::string& name,
                                                    const IInterface* parent )
  : BackgroundEstiAvHPD ( type, name, parent ) { }

// Destructor
BackgroundEstiClustering::~BackgroundEstiClustering() { }

double BackgroundEstiClustering::backWeight( LHCb::RichRecPixel * pixel ) const
{
  const double clusSize = (double) pixel->associatedCluster().size();
  const double nPhots   = (double) pixel->richRecPhotons().size();
  return ( clusSize / ( nPhots + 1.0 ) );
}

void BackgroundEstiClustering::pixelBackgrounds() const
{
  // first loop over all pixels once to calculate normalisation factor for each HPD
  // (sum of each pixels background weight)
  typedef Rich::Map<LHCb::RichSmartID, std::pair<int,double> > HPDNorm;
  HPDNorm hpdNorm;
  {for ( LHCb::RichRecPixels::const_iterator pixel = richPixels()->begin();
         pixel != richPixels()->end(); ++pixel )
  {
    const LHCb::RichSmartID pd = (*pixel)->hpdPixelCluster().hpd();
    ++hpdNorm[ pd ].first; // count hits in each hpd
    hpdNorm[ pd ].second += backWeight(*pixel); // sum the total weight
  }}

  // loop over pixels
  for ( LHCb::RichRecPixels::const_iterator pixel = richPixels()->begin();
        pixel != richPixels()->end(); ++pixel )
  {
    const LHCb::RichSmartID&   pd = (*pixel)->hpdPixelCluster().hpd();
    const Rich::DetectorType& det = (*pixel)->hpdPixelCluster().rich();

    // background for this HPD
    const double hpdTotBackground = (m_obsPDsignals[det])[pd] - (m_expPDsignals[det])[pd];

    // HPD normalisation
    HPDNorm::const_iterator iF = hpdNorm.find(pd);
    const double norm = ( iF == hpdNorm.end() ? 1.0 : iF->second.second / iF->second.first ); 

    // background weight for this pixel
    const double bckWeight = backWeight(*pixel) / norm;

    // background contribution for this pixel
    const double pixBkg = hpdTotBackground * bckWeight / m_nPixelsPerPD ;
    
    // Save this value in the pixel
    (*pixel)->setCurrentBackground( pixBkg > 0 ? (LHCb::RichRecRing::FloatType)(pixBkg) : 0.0f );
    
    // printout
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "Pixel " << (*pixel)->hpdPixelCluster()
                << " hpdTotBkg " << hpdTotBackground
                << " bckWeight " << bckWeight
                << " Obs "  << (m_obsPDsignals[det])[pd]
                << " Exp "  << (m_expPDsignals[det])[pd]
                << " bkg "  << (*pixel)->currentBackground()
                << endmsg;
    }

  }

}
