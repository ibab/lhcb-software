
//--------------------------------------------------------------------------
/** @file RichTracklessRingIsolationAlg.cpp
 *
 *  Implementation file for algorithm class : RichTracklessRingIsolationAlg
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
DECLARE_ALGORITHM_FACTORY( TracklessRingIsolationAlg )

// Standard constructor, initializes variables
TracklessRingIsolationAlg::
TracklessRingIsolationAlg( const std::string& name,
                           ISvcLocator* pSvcLocator )
  : Rich::Rec::AlgBase ( name, pSvcLocator ),
    m_scale            ( Rich::NRadiatorTypes, 0 )
{
  setProperty( "UseEfficiencyRowFormat", true );

  declareProperty( "InputRings",
                   m_inputRings = LHCb::RichRecRingLocation::MarkovRings+"All" );
  declareProperty( "OutputRings",
                   m_outputRings = LHCb::RichRecRingLocation::MarkovRings+"Isolated" );
  declareProperty( "SizeSepCut",           m_sizesepcut = { 150, 150, 260 } );
  declareProperty( "MaxPixSearchWin",        m_pixelWin = { 200, 100, 200 } );
  declareProperty( "SizeRingWidth",     m_sizeringwidth = { 0.01, 0.01, 0.00944 } );
  declareProperty( "MaxFracOutsideRing", m_sizepixelcut = { 0.20001, 0.20001, 0.20001 } );
  declareProperty( "CKthetaMax",           m_ckThetaMax = { 0.24, 0.052, 0.03 } );
  declareProperty( "SepGMax",                 m_sepGMax = { 342, 75, 130 } );
  declareProperty( "NPhiRegions",         m_nPhiRegions = { 8, 8, 8 } );
  declareProperty( "SizePhiCut",           m_sizephicut = { 0.2, 0.5, 0.5 } );
  declareProperty( "MaxFitVariance",   m_maxFitVariance = { 100, 100, 100 } );
  declareProperty( "AbortEarly",           m_abortEarly = false );
}

// Destructor
TracklessRingIsolationAlg::~TracklessRingIsolationAlg() {}

StatusCode TracklessRingIsolationAlg::initialize()
{
  const StatusCode sc = Rich::Rec::AlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  _ri_debug << "Input Rings                 : " << m_inputRings  << endmsg;
  _ri_debug << "Output Rings                : " << m_outputRings << endmsg;
  _ri_debug << "Pixel Search Window         : " << m_pixelWin << endmsg;
  _ri_debug << "Ring centre cut             : " << m_sizesepcut  << endmsg;
  _ri_debug << "Ring width cut              : " << m_sizeringwidth << endmsg;
  _ri_debug << "Fraction outside ring cut   : " << m_sizepixelcut << endmsg;
  _ri_debug << "# Phi regions               : " << m_nPhiRegions << endmsg;
  _ri_debug << "Max fraction per phi region : " << m_sizephicut << endmsg;
  _ri_debug << "Max Fit Variance            : " << m_maxFitVariance << endmsg;

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
  const auto * inrings = get<LHCb::RichRecRings>(m_inputRings);

  // Create a container for the output rings
  auto * outrings = new LHCb::RichRecRings();
  put ( outrings, m_outputRings );

  // Count Rings per radiator
  typedef Rich::Map<Rich::RadiatorType,unsigned int> RadCount;
  RadCount rCount;
  rCount[Rich::Aerogel]  = 0;
  rCount[Rich::Rich1Gas] = 0;
  rCount[Rich::Rich2Gas] = 0;

  // Loop over the input rings and select the 'best' ones
  for ( const auto ring : *inrings )
  {
    // start of isolated. Prove otherwise...
    bool ringIsIsolated(true);

    // Ring RICH, radiator and panel
    const auto rad   = ring->radiator();
    const auto panel = ring->panel();
    const auto rich  = ring->rich();

    _ri_debug << "Trying ring " << ring->key()
              << " | " << rich
              << " " << Rich::text(rich,panel)
              << " " << rad
              << endmsg;

    // Ring centre
    const auto & RingCentreLocal = ring->centrePointLocal();

    _ri_verbo << " -> Ring centre point " << RingCentreLocal << endmsg;

    // Loop over rings to compare ring centres
    bool OK(true);
    for ( const auto ring2 : *inrings )
    {
      if ( ring == ring2 ) continue; // check not comparing the ring with itself

      // same RICH, radiator and panel ?
      if ( ring->rich()     != ring2->rich()     ||
           ring->radiator() != ring2->radiator() ||
           ring->panel()    != ring2->panel()    ) continue;

      // Ring2 centre
      const auto & RingCentreLocal2 = ring2->centrePointLocal();

      // Compare outer loop segment entry to all other segments and find separation
      const double Xdifsq      = std::pow( fabs(RingCentreLocal.x()-RingCentreLocal2.x()), 2 );
      const double Ydifsq      = std::pow( fabs(RingCentreLocal.y()-RingCentreLocal2.y()), 2 );
      const double centreXYdif = std::sqrt(Xdifsq+Ydifsq);

      _ri_verbo << "  -> Comparing to ring " << ring2->key()
                << " centre point " << RingCentreLocal2
                << " sepDist " << centreXYdif
                << endmsg;

      if ( centreXYdif < m_sizesepcut[rad] )
      {
        OK = false;
        break;
      }

    } // inner ring loop

    // skip if ring is already not isolated
    ringIsIsolated &= testCut( "Ring centre isolation", rad, OK );
    if ( !ringIsIsolated && m_abortEarly ) continue;

    //-------------------------------------------------------------------------------------------------

    // Ring radius
    const auto rRadius = ring->radius();//RADIUS IN RADIANS!

    // Define ring around ring radius (in Ch theta)
    const auto ringmin = rRadius - m_sizeringwidth[rad];
    const auto ringmax = rRadius + m_sizeringwidth[rad];

    int hittotal    = 0;
    int hitsoutside = 0;
    int hitsinside  = 0; //initialise all to zero for each ring

    // Tally vector for number of photons in each phi region
    std::vector<int> regTally(m_nPhiRegions[rad],0);

    // Loop over all pixels in same HPD panel
    const auto pixrange = pixelCreator()->range( rich, panel );
    for ( const auto pixel : pixrange )
    {

      // pixel hit
      const auto & PixelLocal = pixel->localPosition();//should be radiation corrected?!

      // Compare pixel to ring centre
      const auto sep = std::sqrt( std::pow( fabs(RingCentreLocal.x()-PixelLocal.x()), 2 ) +
                                  std::pow( fabs(RingCentreLocal.y()-PixelLocal.y()), 2 ) );
      
      // is pixel inside consideration window for this ring
      if ( sep < m_pixelWin[rad] )
      {
        ++hittotal;

        // Estimated Ch theta for this pixel
        const auto ckThetaEsti = sep*m_scale[rad];

        // Does hit lie 'just' outside the ring region
        if      ( ckThetaEsti > ringmax ) { ++hitsoutside; }
        // is it inside the ring
        else if ( ckThetaEsti < ringmin ) { ++hitsinside;  }

        //--------------------------------------------------------------------------------------------------

        // x,y differences
        const auto diff_x       = RingCentreLocal.x()-PixelLocal.x();
        const auto diff_y       = RingCentreLocal.y()-PixelLocal.y();

        // Estimate CK Phi for pixel
        const auto CherenkovPhi = Gaudi::Units::pi + std::atan2( diff_y, diff_x );

        // Which CK phi region
        const int region = (int)( m_nPhiRegions[rad] * CherenkovPhi / (M_PI*2.0) );

        // count number in each region
        ++regTally[region];

      }

    } // loop over pixels

    ringIsIsolated &= testCut( "Ring has some associated hits", rad, 
                               hittotal>0 && (hittotal-(hitsoutside+hitsinside))>0 );
    if ( !ringIsIsolated && m_abortEarly ) continue;

    const double frachitsout = (double)(hitsinside+hitsoutside) / (double)hittotal;
    OK = frachitsout <= m_sizepixelcut[rad];
    ringIsIsolated &= testCut( "Occupancy outside ring annulus too large", rad, OK );
    if ( !ringIsIsolated && m_abortEarly ) continue;

    OK = true;
    for ( const auto & region : regTally )
    {
      if ( (double)(region)/(double)hittotal > m_sizephicut[rad] )
      {
        OK = false;
        break;
      }
    }
    ringIsIsolated &= testCut( "Too many pixel blobs", rad, OK );
    if ( !ringIsIsolated && m_abortEarly ) continue;

    // refit the ring
    FastRingFitter fitter(*ring);
    fitter.fit();
    // Fit OK ?
    OK = ( fitter.result().Status == 0 &&
           fitter.result().Variance < m_maxFitVariance[rad] );
    if (!OK)
    {
      _ri_verbo << " -> Failed refitting : "
                << fitter.result()
                << endmsg;
    }
    ringIsIsolated &= testCut( "Ring Refitting", rad, OK );
    if ( !ringIsIsolated && m_abortEarly ) continue;

    //----------------------------------------------------------------------
    if ( ringIsIsolated )
    {
      // If we get here, ring is isolated
      // count
      ++rCount[ring->radiator()];
      // clone and save
      outrings->insert( new LHCb::RichRecRing(*ring), ring->key() );
    }

  } // outer ring loop

  _ri_debug << "Selected " << outrings->size() << " isolated rings at " << m_outputRings << endmsg;
  
  for ( const auto & rad : rCount )
  {
    counter(Rich::text(rad.first)+" Selected isolated rings") += rad.second;
  }

  return StatusCode::SUCCESS;
}
