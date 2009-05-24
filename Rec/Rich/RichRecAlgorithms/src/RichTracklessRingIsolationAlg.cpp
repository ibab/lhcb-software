
//--------------------------------------------------------------------------
/** @file RichTracklessRingIsolationAlg.cpp
 *
 *  Implementation file for algorithm class : RichTracklessRingIsolationAlg
 *
 *  CVS Log :-
 *  $Id: RichTracklessRingIsolationAlg.cpp,v 1.11 2009-05-24 16:18:25 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */
//--------------------------------------------------------------------------

// local
#include "RichTracklessRingIsolationAlg.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TracklessRingIsolationAlg );

// Standard constructor, initializes variables
TracklessRingIsolationAlg::
TracklessRingIsolationAlg( const std::string& name,
                           ISvcLocator* pSvcLocator )
  : Rich::Rec::AlgBase ( name, pSvcLocator ),
    m_scale            ( Rich::NRadiatorTypes, 0 )
{
  setProperty( "UseEfficiencyRowFormat", true );
  using namespace boost::assign;
  declareProperty( "InputRings",
                   m_inputRings = LHCb::RichRecRingLocation::MarkovRings+"All" );
  declareProperty( "OutputRings",
                   m_outputRings = LHCb::RichRecRingLocation::MarkovRings+"Isolated" );
  declareProperty( "SizeSepCut",         m_sizesepcut = list_of (260)(150)(260) );
  declareProperty( "SizeRingWidth",   m_sizeringwidth = list_of (0.01)(0.01)(0.00944) );
  declareProperty( "SizePixelCut",     m_sizepixelcut = list_of (0.2)(0.3)(0.2) );
  declareProperty( "CKthetaMax",         m_ckThetaMax = list_of (0.24)(0.052)(0.03) );
  declareProperty( "SepGMax",               m_sepGMax = list_of (342)(75)(130) );
  declareProperty( "NPhiRegions",       m_nPhiRegions = list_of (8)(8)(8)   );
  declareProperty( "SizePhiCut",         m_sizephicut = list_of (0.2)(0.2125)(0.2125)   );
  declareProperty( "MaxFitVariance", m_maxFitVariance = list_of (200)(200)(200) );
  declareProperty( "AbortEarly", m_abortEarly = true );
}

// Destructor
TracklessRingIsolationAlg::~TracklessRingIsolationAlg() {}

StatusCode TracklessRingIsolationAlg::initialize()
{
  const StatusCode sc = RichRecAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  info() << "Input Rings                 : " << m_inputRings  << endmsg;
  info() << "Output Rings                : " << m_outputRings << endmsg;
  info() << "Ring centre cut             : " << m_sizesepcut  << endmsg;
  info() << "Ring width cut              : " << m_sizeringwidth << endmsg;
  info() << "Fraction outside ring cut   : " << m_sizepixelcut << endmsg;
  info() << "# Phi regions               : " << m_nPhiRegions << endmsg;
  info() << "Max fraction per phi region : " << m_sizephicut << endmsg;
  info() << "Max Fit Variance            : " << m_maxFitVariance << endmsg;

  // loop over radiators
  for ( int radi = 0; radi < Rich::NRadiatorTypes; ++radi )
  {
    // numbers for hit pixel calculations
    m_scale[radi] = (m_ckThetaMax[radi]/m_sepGMax[radi]);
  }

  return sc;
}

StatusCode TracklessRingIsolationAlg::execute()
{
  // Event Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Load the input rings
  const LHCb::RichRecRings * inrings = get<LHCb::RichRecRings>(m_inputRings);

  // Create a container for the output rings
  LHCb::RichRecRings * outrings = new LHCb::RichRecRings();
  put ( outrings, m_outputRings );

  // Count Rings per radiator
  typedef Rich::Map<Rich::RadiatorType,unsigned int> RadCount;
  RadCount rCount;
  //rCount[Rich::Aerogel]  = 0;
  rCount[Rich::Rich1Gas] = 0;
  rCount[Rich::Rich2Gas] = 0;

  // Loop over the input rings and select the 'best' ones
  for ( LHCb::RichRecRings::const_iterator iRing = inrings->begin();
        iRing != inrings->end(); ++iRing )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Trying ring " << (*iRing)->key() << endmsg;

    bool ringIsIsolated(true);

    // Ring radiator
    const Rich::RadiatorType rad = (*iRing)->radiator();

    // Ring centre
    const Gaudi::XYZPoint & RingCentreLocal = (*iRing)->centrePointLocal();

    // Loop over rings to compare ring centres
    bool OK(true);
    for ( LHCb::RichRecRings::const_iterator iR2 = inrings->begin();
          iR2 != inrings->end(); ++iR2 )
    {
      if ( (*iRing) == (*iR2) ) continue; // check not comparing the ring with itself

      // same RICH and pane ?
      if ( (*iRing)->rich()  != (*iR2)->rich()  ||
           (*iRing)->panel() != (*iR2)->panel() ) continue;

      // Ring2 centre
      const Gaudi::XYZPoint & RingCentreLocal2 = (*iR2)->centrePointLocal();

      // Compare outer loop segment entry to all other segments and find separation
      const double Xdifsq      = std::pow( fabs(RingCentreLocal.x()-RingCentreLocal2.x()), 2 );
      const double Ydifsq      = std::pow( fabs(RingCentreLocal.y()-RingCentreLocal2.y()), 2 );
      const double centreXYdif = std::sqrt(Xdifsq+Ydifsq);

      if ( centreXYdif < m_sizesepcut[rad]   )
      {
        if ( msgLevel(MSG::DEBUG) )
          debug() << " -> Too close to another ring !" << endmsg;
        OK = false;
        break;
      }

    } // inner ring loop

    // skip if ring is already not isolated
    ringIsIsolated &= testCut( "Ring centre isolation", rad, OK );
    if ( !ringIsIsolated && m_abortEarly ) continue;

    //-------------------------------------------------------------------------------------------------

    // Ring radius
    const LHCb::RichRecRing::FloatType rRadius = (*iRing)->radius();//RADIUS IN RADIANS!

    // Define ring around ring radius (in Ch theta)
    const double ringmin = rRadius - m_sizeringwidth[rad];
    const double ringmax = rRadius + m_sizeringwidth[rad];

    int hittotal    = 0;
    int hitsoutside = 0;
    int hitsinside  = 0; //initialise all to zero for each ring

    // Tally vector for number of photons in each phi region
    std::vector<int> regTally(m_nPhiRegions[rad],0);

    //Loop over all hit pixels associated to ring - already <260mm from ring centre by default
    for ( LHCb::RichRecPixelOnRing::Vector::iterator iP = (*iRing)->richRecPixels().begin();
          iP != (*iRing)->richRecPixels().end(); ++iP )
    {
      LHCb::RichRecPixel * pixel = (*iP).pixel();//get pixel from pixelOnRing

      // pixel hit
      const Gaudi::XYZPoint & PixelLocal = pixel->localPosition();//should be radiation corrected?!

      // Compare pixel to ring centre
      const double Xdifsq      = std::pow( fabs(RingCentreLocal.x()-PixelLocal.x()), 2 );
      const double Ydifsq      = std::pow( fabs(RingCentreLocal.y()-PixelLocal.y()), 2 );
      const double sep = std::sqrt(Xdifsq+Ydifsq);

      ++hittotal; // Counting total hits assoc with this ring

      // Estimated Ch theta for this pixel
      const double ckThetaEsti = sep*m_scale[rad];

      // Does hit lie outside ring region?
      if (ckThetaEsti>ringmax) { ++hitsoutside; }
      if (ckThetaEsti<ringmin) { ++hitsinside;  }

      //--------------------------------------------------------------------------------------------------

      // x,y differences
      const float diff_x       = RingCentreLocal.x()-PixelLocal.x();
      const float diff_y       = RingCentreLocal.y()-PixelLocal.y();
      // Estimate Ch Phi for pixel
      const float CherenkovPhi = Gaudi::Units::pi + std::atan2( diff_y, diff_x );

      // Which Ch phi region
      const int region = (int)( m_nPhiRegions[rad] * CherenkovPhi / (M_PI*2.0) );

      // count number in each region
      ++regTally[region];

    }//loop over pixels

    // To prevent dividing by zero, although if a Markov ring exists it should have hits
    if (hittotal==0) { hittotal = 1; }

    const double frachitsout = (double)(hitsinside+hitsoutside) / (double)hittotal;

    OK = frachitsout < m_sizepixelcut[rad];
    ringIsIsolated &= testCut( "Occupancy outside ring", rad, OK );
    if ( !OK && msgLevel(MSG::VERBOSE) )
    {
      verbose() << "Too many pixel hits outside ring!" << endmsg;
    }
    if ( !ringIsIsolated && m_abortEarly ) continue;

    OK = true;
    for ( std::vector<int>::const_iterator iRegion = regTally.begin();
          iRegion != regTally.end(); ++iRegion )
    {
      if ( (double)(*iRegion)/(double)hittotal > m_sizephicut[rad] )
      {
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << "Ring circularity" << endmsg;
        OK = false;
        break;
      }
    }
    ringIsIsolated &= testCut( "Too many pixel blobs", rad, OK );
    if ( !ringIsIsolated && m_abortEarly ) continue;

    // refit the ring
    FastRingFitter fitter(**iRing);
    fitter.fit();
    // Fit OK ?
    OK = true;
    if ( fitter.result().Status != 0 ||
         fitter.result().Variance > m_maxFitVariance[rad] )
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << " -> Failed refitting : " 
                  << fitter.result()
                  << endmsg;
      OK = false;
    }
    ringIsIsolated &= testCut( "Ring Refitting", rad, OK );
    if ( !ringIsIsolated && m_abortEarly ) continue;

    //----------------------------------------------------------------------
    if ( ringIsIsolated )
    {
      // If we get here, ring is isolated
      // count
      ++rCount[(*iRing)->radiator()];
      // clone and save
      outrings->insert( new LHCb::RichRecRing(**iRing), (*iRing)->key() );
    }

  } // outer ring loop

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Selected " << outrings->size() << " isolated rings at " << m_outputRings << endmsg;

  for ( RadCount::const_iterator rad = rCount.begin(); rad != rCount.end(); ++rad )
  {
    counter(Rich::text(rad->first)+" Selected isolated rings") += rad->second;
  }
 
  return StatusCode::SUCCESS;
}
