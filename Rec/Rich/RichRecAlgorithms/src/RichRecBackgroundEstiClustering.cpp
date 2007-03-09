
//--------------------------------------------------------------------------
/** @file RichRecBackgroundEstiClustering.cpp
 *
 *  Implementation file for algorithm class : Rich::Rec::BackgroundEstiClustering
 *
 *  CVS Log :-
 *  $Id: RichRecBackgroundEstiClustering.cpp,v 1.1 2007-03-09 22:48:41 jonrob Exp $
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
DECLARE_ALGORITHM_FACTORY( BackgroundEstiClustering );

// Standard constructor, initializes variables
BackgroundEstiClustering::BackgroundEstiClustering( const std::string& name,
                                                    ISvcLocator* pSvcLocator )
  : BackgroundEstiAvHPD ( name, pSvcLocator ) { }

// Destructor
BackgroundEstiClustering::~BackgroundEstiClustering() {}

double BackgroundEstiClustering::backWeight( LHCb::RichRecPixel * pixel ) const
{
  const double clusSize = pixel->associatedCluster().size();
  const double nPhots   = pixel->richRecPhotons().size();
  return clusSize / (nPhots+1) ;
}

StatusCode BackgroundEstiClustering::pixelBackgrounds()
{
  // first loop over all pixels once to calculate normalisation factor for each HPD
  // (sum of each pixels background weight)
  typedef Rich::Map<LHCb::RichSmartID, std::pair<int,double> > HPDNorm;
  HPDNorm hpdNorm;
  {for ( LHCb::RichRecPixels::iterator pixel = richPixels()->begin();
         pixel != richPixels()->end(); ++pixel )
  {
    const LHCb::RichSmartID pd = (*pixel)->hpdPixelCluster().hpd();
    ++hpdNorm[ pd ].first; // count hits in each hpd
    hpdNorm[ pd ].second += backWeight(*pixel); // sum the total weight
  }}

  // loop over pixels
  for ( LHCb::RichRecPixels::iterator pixel = richPixels()->begin();
        pixel != richPixels()->end(); ++pixel )
  {
    const LHCb::RichSmartID   pd = (*pixel)->hpdPixelCluster().hpd();
    const Rich::DetectorType det = (*pixel)->hpdPixelCluster().rich();

    // background for this HPD
    const double hpdTotBackground = (m_obsPDsignals[det])[pd] - (m_expPDsignals[det])[pd];

    // HPD normalisation
    HPDNorm::const_iterator iF = hpdNorm.find(pd);
    const double norm = ( iF == hpdNorm.end() ? 1 : iF->second.second / iF->second.first ); 

    // background weight for this pixel
    const double bckWeight = backWeight(*pixel) / norm;

    // background contribution for this pixel
    const double pixBkg = hpdTotBackground * bckWeight / m_nPixelsPerPD ;
    
    // Save this value in the pixel
    (*pixel)->setCurrentBackground( pixBkg>0 ? pixBkg : 0 );
    
    // Debug printout
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "Pixel " << (*pixel)->hpdPixelCluster()
                << " hpdTotBkg " << hpdTotBackground
                << " bckWeight " << bckWeight
                << " Obs "  << (m_obsPDsignals[det])[pd]
                << " Exp "  << (m_expPDsignals[det])[pd]
                << " bkg "  << (*pixel)->currentBackground()
                << endreq;
    }

  }

  return StatusCode::SUCCESS;
}
