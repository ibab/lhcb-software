
//---------------------------------------------------------------------------
/** @file RichMarkovRingFinderMoni.cpp
 *
 *  Implementation file for algorithm class : RichMarkovRingFinderMoni
 *
 *  $Id: RichMarkovRingFinderMoni.cpp,v 1.26 2008-06-10 23:11:38 jonrob Exp $
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
  : RichRecHistoAlgBase ( name, pSvcLocator ),
    m_rayTrace          ( NULL ),
    m_idTool            ( NULL ),
    m_trSelector        ( NULL )
{
  declareProperty( "RingLocation", m_ringLoc = LHCb::RichRecRingLocation::MarkovRings+"All" );
  declareProperty( "SizeSepCut",   m_sizesepcut                =  260); // ring centre separation
  declareProperty( "SizeSegRingAssocregion",  m_regionscale    =  0.5); // this * ring radius gives area to consider segments for association - see A. Buckley pg 85

}

// Destructor
RichMarkovRingFinderMoni::~RichMarkovRingFinderMoni() {};

//  Initialize
StatusCode RichMarkovRingFinderMoni::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRayTracing",       m_rayTrace          );
  acquireTool( "RichSmartIDTool",      m_idTool,   0, true );
  acquireTool( "TrackSelector",        m_trSelector, this  );

  return sc;
}

// Main execution
StatusCode RichMarkovRingFinderMoni::execute()
{
  debug() << "Execute" << endreq;

  bool trackIsIsolated(true);


  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Make sure all tracks and segments have been formed
  if ( richTracks()->empty() )
  {
    if ( !trackCreator()->newTracks() ) return StatusCode::FAILURE;
    debug() << "No tracks found : Created " << richTracks()->size()
            << " RichRecTracks " << richSegments()->size()
            << " RichRecSegments" << endreq;
  }

  //   // Iterate over segments - NOT ASSOC WITH FITTED RINGS
  //   for ( RichRecSegments::const_iterator iSeg = richSegments()->begin();
  //         iSeg != richSegments()->end(); ++iSeg )
  //   {
  //     RichRecSegment * segment = *iSeg;

  //   } // end segment loop


  // Retrieve rings and loop
  const LHCb::RichRecRings * rings = get<LHCb::RichRecRings>( m_ringLoc );
  for ( LHCb::RichRecRings::const_iterator iR = rings->begin();
        iR != rings->end(); ++iR )
  {

    LHCb::RichRecRing * ring = *iR;

    const Gaudi::XYZPoint &  RingCentreLocal = ring->centrePointLocal();
    //double  RingCentreLocalX = ring->centrePointLocal().x();
    //double  RingCentreLocalY = ring->centrePointLocal().y();

    // Loop over rings to compare ring centres
    for ( LHCb::RichRecRings::const_iterator iR2 = rings->begin();
          iR2 != rings->end(); ++iR2 )
    {
      LHCb::RichRecRing * ring2 = *iR2;

      if ( ring == ring2 ) continue;//check not comparing the ring with itself

      // Ring centre
      const Gaudi::XYZPoint &  RingCentreLocal2 = ring2->centrePointLocal();

      // Compare outer loop segment entry to all other segments and find separation
      const double Xdifsq      = std::pow( fabs(RingCentreLocal.x()-RingCentreLocal2.x()), 2 );
      const double Ydifsq      = std::pow( fabs(RingCentreLocal.y()-RingCentreLocal2.y()), 2 );
      const double centreXYdif = std::sqrt(Xdifsq+Ydifsq);

      if (centreXYdif < m_sizesepcut){
        debug() << "Too close to another ring!" << endmsg;
        trackIsIsolated = false;
      }

    }//inner ring loop

    // Plot centre points of "isolated" rings
    if(trackIsIsolated){ plot2D( RingCentreLocal.x(), RingCentreLocal.y(), "Ring centres",
                                 -2*Gaudi::Units::m, 2*Gaudi::Units::m,
                                 -2*Gaudi::Units::m, 2*Gaudi::Units::m, 200, 200 );
    }//isolated

    // Ring radius
    //const LHCb::RichRecRing::FloatType ringRadius = ring->radius();//RADIUS IN RADIANS!
    double ringRadius = 130.0;//set to saturated RICH2 ring size
    //debug() << "Ring radius" << ringRadius<< endmsg;

    typedef std::map<const double, RichRecSegment*> ssmap;
    ssmap SegSepMap;

    //finding nearest segment
    for ( RichRecSegments::const_iterator iSeg = richSegments()->begin();
          iSeg != richSegments()->end(); ++iSeg )
    {
      RichRecSegment * segment = *iSeg;

      // Track hit point (if it were a photon) on detector plane, in local coords (i.e. centre of Ch ring)
      const Gaudi::XYZPoint & tkPtnLocal = segment->pdPanelHitPointLocal();

      const double Xsegringsep      = std::pow( fabs(RingCentreLocal.x()-tkPtnLocal.x()), 2 );
      const double Ysegringsep      = std::pow( fabs(RingCentreLocal.y()-tkPtnLocal.y()), 2 );
      const double segringXYdif     = std::sqrt(Xsegringsep+Ysegringsep);

      //info() << "Separation: " << segringXYdif << endmsg;


      // Consider segments within a certain region around ring centre
      double regionRadius = m_regionscale * ringRadius;

      //info() << "Region radius" << regionRadius << endmsg;

      if (segringXYdif > regionRadius){
        continue;// Skip segments too far away from the ring centre to be associated with it
      }
      else{
        debug() << "Segment lies within region around ring centre" << endmsg;
        // Populate map
        SegSepMap.insert(ssmap::value_type(segringXYdif, segment));
      }

    } //end segment loop

    // For debug write separations from map
    //  ssmap::iterator itM;
    //   int x=0;
    //   for (itM=SegSepMap.begin(); itM != SegSepMap.end(); itM++){
    //       info() << "Map separation element " << x++ << ":" << (*itM).first << endmsg;
    //   }


    // Find smallest separation (check there's something in the map first!)
    if (SegSepMap.size()==0) continue; //skip to next ring, no segments near enough for association
    ssmap::iterator minitM = SegSepMap.begin();//map is sorted so first element should have smallest key, i.e. smallest separation
    //info() << "The smallest element is " << (*minitM).first << endmsg;
    RichRecSegment * nearestSeg = (*minitM).second;

    // Is MC available
    const bool mcTrackOK = richRecMCTool()->trackToMCPAvailable();
    // Get the Cherenkov angle for the nearest segment and ring radius
    Rich::ParticleIDType mcType = Pion;
    if ( mcTrackOK ){
      // True particle type
      mcType = richRecMCTool()->mcParticleType(nearestSeg);
      if ( Rich::Unknown  == mcType ) continue; // skip tracks with unknown MC type
      if ( Rich::Electron == mcType ) continue; // skip electrons which are reconstructed badly.
    }
    else continue;//need the MC info!

    const RichRecSegment::FloatType avChTheta = nearestSeg->averageCKTheta ( mcType);
    const RichRecRing::FloatType rRadius = ring->radius();//RADIUS IN RADIANS!

    // Plot nearest segment Ch theta against ring radius (both in radians)
    plot2D( rRadius, avChTheta, "MCChThetaVRingRadius",
            0.015, 0.035,
            0.015, 0.035, 300, 300 );

  }//outer ring loop

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichMarkovRingFinderMoni::finalize()
{
  // Execute base class method
  return RichRecHistoAlgBase::finalize();
}
