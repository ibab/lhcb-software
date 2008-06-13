
//---------------------------------------------------------------------------
/** @file RichMarkovRingFinderMoni.cpp
 *
 *  Implementation file for algorithm class : RichMarkovRingFinderMoni
 *
 *  $Id: RichMarkovRingFinderMoni.cpp,v 1.30 2008-06-13 13:39:41 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

// local
#include "RichMarkovRingFinderMoni.h"

// namespace
using namespace LHCb;
using namespace Rich::Rec::MC;

//---------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RichMarkovRingFinderMoni );

// Standard constructor, initializes variables
RichMarkovRingFinderMoni::RichMarkovRingFinderMoni( const std::string& name,
                                                    ISvcLocator* pSvcLocator )
  : RichRecHistoAlgBase ( name, pSvcLocator )
{
  declareProperty( "RingLocation", m_ringLoc = LHCb::RichRecRingLocation::MarkovRings );
}

// Destructor
RichMarkovRingFinderMoni::~RichMarkovRingFinderMoni() {};

//  Initialize
StatusCode RichMarkovRingFinderMoni::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  info() << "Monitoring Trackless rings " << m_ringLoc << endreq;

  return sc;
}

// Main execution
StatusCode RichMarkovRingFinderMoni::execute()
{
  debug() << "Execute" << endreq;

  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Retrieve rings
  if ( !exist<LHCb::RichRecRings>(m_ringLoc) ) return StatusCode::SUCCESS;
  const LHCb::RichRecRings * rings = get<LHCb::RichRecRings>( m_ringLoc );

  // Rich Histo ID
  const RichHistoID hid;

  // loop
  for ( LHCb::RichRecRings::const_iterator iR = rings->begin();
        iR != rings->end(); ++iR )
  {
    LHCb::RichRecRing * ring = *iR;

    // Radiator info
    //const Rich::RadiatorType rad = ring->radiator(); 
    // Cannot do yet until RichRecRing is updated so hard code to RICH2 temporarily
    const Rich::RadiatorType rad = Rich::Rich2;

    // Plot centre points of "isolated" rings
    const Gaudi::XYZPoint & RingCentreLocal = (*iR)->centrePointLocal();
    plot2D( RingCentreLocal.x(), RingCentreLocal.y(),  
            hid(rad,"ringCentres"), "Ring centres",
            -2*Gaudi::Units::m, 2*Gaudi::Units::m,
            -2*Gaudi::Units::m, 2*Gaudi::Units::m, 200, 200 );    

    // Ring radius
    //const LHCb::RichRecRing::FloatType ringRadius = ring->radius();//RADIUS IN RADIANS!
    //double ringRadius = 130.0;//set to saturated RICH2 ring size
    //debug() << "Ring radius" << ringRadius<< endmsg;

    // Ring knows if it is associated or not
    RichRecSegment * nearestSeg = ring->richRecSegment();

    // Is MC available
    const bool mcTrackOK = richRecMCTool()->trackToMCPAvailable();
    // Get the Cherenkov angle for the nearest segment and ring radius
    Rich::ParticleIDType mcType = Pion;
    if ( mcTrackOK )
    {
      // True particle type
      mcType = richRecMCTool()->mcParticleType(nearestSeg);
      if ( Rich::Unknown  == mcType ) continue; // skip tracks with unknown MC type
      if ( Rich::Electron == mcType ) continue; // skip electrons which are reconstructed badly.
    }
    else continue; // the rest needs MC info !

    const RichRecSegment::FloatType avChTheta = nearestSeg->averageCKTheta ( mcType);
    const RichRecRing::FloatType rRadius = ring->radius();//RADIUS IN RADIANS!

    // Plot nearest segment Ch theta against ring radius (both in radians)
    plot2D( rRadius, avChTheta, hid(rad,"MCChThetaVRingRadius"),
            "MCChThetaVRingRadius",
            0.015, 0.035,
            0.015, 0.035, 300, 300 );

  }//outer ring loop

  return StatusCode::SUCCESS;
}
