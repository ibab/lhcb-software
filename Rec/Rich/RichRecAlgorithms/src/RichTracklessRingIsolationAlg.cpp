
//--------------------------------------------------------------------------
/** @file RichTracklessRingIsolationAlg.cpp
 *
 *  Implementation file for algorithm class : RichTracklessRingIsolationAlg
 *
 *  CVS Log :-
 *  $Id: RichTracklessRingIsolationAlg.cpp,v 1.5 2008-09-12 15:45:12 jonrob Exp $
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
    m_ckThetaMax     ( Rich::NRadiatorTypes, 0 ),
    m_sepGMax        ( Rich::NRadiatorTypes, 0 ),
    m_scale          ( Rich::NRadiatorTypes, 0 )
{
  using namespace boost::assign;
  declareProperty( "InputRings",
                   m_inputRings = LHCb::RichRecRingLocation::MarkovRings+"All" );
  declareProperty( "OutputRings",
                   m_outputRings = LHCb::RichRecRingLocation::MarkovRings+"Isolated" );
  declareProperty( "SizeSepCut", m_sizesepcut = list_of(260)(260)(260) );
  declareProperty( "SizeRingWidth", m_sizeringwidth = list_of(0.0)(0.0)(0.00944) );
  declareProperty( "SizePixelCut", m_sizepixelcut = list_of(0.2)(0.2)(0.2) );
  declareProperty( "CKthetaMax", m_ckThetaMax = list_of  (0.24)(0.052) (0.03) );
  declareProperty( "SepGMax", m_sepGMax = list_of  (342)(75)(130) );
  declareProperty( "NPhiRegions", m_nPhiRegions = list_of  (8)(8)(8)   );
  declareProperty( "SizePhiCut", m_sizephicut = list_of  (0.2125)(0.2125)(0.2125)   );
}

// Destructor
TracklessRingIsolationAlg::~TracklessRingIsolationAlg() {}

StatusCode TracklessRingIsolationAlg::initialize()
{
  const StatusCode sc = RichRecAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  info() << "Input Rings            : " << m_inputRings  << endreq;
  info() << "Output Rings           : " << m_outputRings << endreq;
  info() << "Ring centre cut        : " << m_sizesepcut  << endreq;

  // loop over radiators
  for ( int radi = 0; radi < Rich::NRadiatorTypes; ++radi )
  {
    //numbers for hit pixel calculations
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

  unsigned int noofrings = 0;

  // Loop over the input rings and select the 'best' ones
  for ( LHCb::RichRecRings::const_iterator iRing = inrings->begin();
        iRing != inrings->end(); ++iRing )
  {
    bool ringIsIsolated(true);

    // Ring radiator
    const Rich::RadiatorType RingRadiator = (*iRing)->radiator();

    // Ring centre
    const Gaudi::XYZPoint & RingCentreLocal = (*iRing)->centrePointLocal();

    // Loop over rings to compare ring centres
    for ( LHCb::RichRecRings::const_iterator iR2 = inrings->begin();
          iR2 != inrings->end(); ++iR2 )
    {
      if ( (*iRing) == (*iR2) ) continue; // check not comparing the ring with itself

      // Ring2 centre
      const Gaudi::XYZPoint & RingCentreLocal2 = (*iR2)->centrePointLocal();

      // Compare outer loop segment entry to all other segments and find separation
      const double Xdifsq      = std::pow( fabs(RingCentreLocal.x()-RingCentreLocal2.x()), 2 );
      const double Ydifsq      = std::pow( fabs(RingCentreLocal.y()-RingCentreLocal2.y()), 2 );
      const double centreXYdif = std::sqrt(Xdifsq+Ydifsq);

      if ( centreXYdif < m_sizesepcut[RingRadiator]   )
      {
        //debug() << "Too close to another ring !" << endmsg;
        ringIsIsolated = false;
        break;
      }


    } // inner ring loop

    //-----------------------------------------------------------------------------------------------------------------------------

    // Ring radius
    const LHCb::RichRecRing::FloatType rRadius = (*iRing)->radius();//RADIUS IN RADIANS!

    // Define ring around ring radius (in Ch theta)
    const double ringmin = rRadius-m_sizeringwidth[RingRadiator];
    const double ringmax = rRadius+m_sizeringwidth[RingRadiator];

    int hittotal    = 0;
    int hitsoutside = 0;
    int hitsinside  = 0; //initialise all to zero for each ring

    // Tally vector for number of photons in each phi region
    std::vector<int> regTally(m_nPhiRegions[RingRadiator],0);

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

      //Estimated Ch theta for this pixel
      const double ckThetaEsti = sep*m_scale[RingRadiator];

      // Does hit lie outside ring region?
      if (ckThetaEsti>ringmax) { ++hitsoutside; }
      if (ckThetaEsti<ringmin) { ++hitsinside;  }

      //---------------------------------------------------------------------------------------------------------

      //Estimate Ch Phi for pixel
      // x,y differences
      const float diff_x = RingCentreLocal.x()-PixelLocal.x();
      const float diff_y = RingCentreLocal.y()-PixelLocal.y();

      const float CherenkovPhi = Gaudi::Units::pi + std::atan2( diff_y, diff_x );

      // Which Ch phi region
      const int region = (int)( m_nPhiRegions[RingRadiator] * CherenkovPhi / (M_PI*2.0) );

      //debug() << "Phi region: " << region  << endmsg;

      // count number in each region
      ++regTally[region];


    }//loop over pixels

    if (hittotal==0) { hittotal = 1; } // To prevent dividing by zero, although if a Markov ring exists it should have hits
    const double frachitsout = (double)(hitsinside+hitsoutside) / (double)hittotal;


    if ( frachitsout > m_sizepixelcut[RingRadiator] )
    {
      //debug() << "Too many pixel hits outside ring!" << endmsg;
      ringIsIsolated = false;
    }

    ++noofrings;

    for ( std::vector<int>::const_iterator iRegion = regTally.begin();
          iRegion != regTally.end(); ++iRegion )
    {
      if ( (double)(*iRegion)/(double)hittotal > m_sizephicut[RingRadiator] )
      {
        //debug() << "Too many blobs!" << endmsg;
        ringIsIsolated = false;
      }
    }



    //---------------------------------------------------------------------------------------------------------------------------
    // is ring isolated
    if ( ringIsIsolated )
    {
      // clone and save
      outrings->insert( new LHCb::RichRecRing(**iRing), (*iRing)->key() );
    }

  } // outer ring loop
  //debug() << "No. of Markov rings past sep cut " << noofrings << endmsg;

  return StatusCode::SUCCESS;
}
