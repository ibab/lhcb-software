
//-----------------------------------------------------------------------------
/** @file RichPhotonRecoTestAlg.cpp
 *
 *  Implementation file for algorithm class : RichPhotonRecoTestAlg
 *
 *  $Id: RichPhotonRecoTestAlg.cpp,v 1.1 2006-03-02 15:26:29 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   21/02/2006
 */
//-----------------------------------------------------------------------------

#include <fstream>

// local
#include "RichPhotonRecoTestAlg.h"

// namespace
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichPhotonRecoTestAlg>          s_factory ;
const        IAlgFactory& RichPhotonRecoTestAlgFactory = s_factory ;


// Standard constructor, initializes variables
RichPhotonRecoTestAlg::RichPhotonRecoTestAlg( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : RichRecHistoAlgBase   ( name, pSvcLocator ),
    m_photonReco          ( 0 ),
    m_photonRecoName      ( "RichDetPhotonReco" )
{
  declareProperty( "PhotonRecoTool", m_photonRecoName );
}

// Destructor
RichPhotonRecoTestAlg::~RichPhotonRecoTestAlg() {};

//  Initialize
StatusCode RichPhotonRecoTestAlg::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( m_photonRecoName, m_photonReco, this );

  return sc;
}

// Main execution
StatusCode RichPhotonRecoTestAlg::execute()
{
  debug() << "Execute" << endreq;

  // Perform some fixed reconstructions
  // ==================================

  typedef Gaudi::XYZPoint    Point;
  typedef std::vector<Point> Points;
  typedef Gaudi::XYZVector   Vector;
  typedef std::vector<RichTrackSegment> TrackSegments;

  // Rich1 data points
  Points dataPoints;
  dataPoints.push_back( Point(-8.56583,1187.12,1508.36) );
  dataPoints.push_back( Point(3.67107,1186.59,1513.6)   );
  dataPoints.push_back( Point(23.2501,1191.52,1513.41)  );

  // Rich2 data points
  dataPoints.push_back( Point(-3936.09,73.6291,10702.3) );
  dataPoints.push_back( Point(4020.39,183.208,10563.8)  );
  dataPoints.push_back( Point(4017.67,175.881,10559.5)  );

  // null errors ( not needed for these tests )
  const RichTrackSegment::StateErrors nullErrs;

  // Aerogel segments
  TrackSegments Segs;
  Segs.push_back( RichTrackSegment( RichTrackSegment::UseAllStateVectors(),
                                    Point(1.24572,99.0629,1110),
                                    Vector(8.27581,284.16,3423.45),
                                    Point(1.37191,103.213,1160),
                                    Vector(9.00563,284.158,3423.45),
                                    Rich::Aerogel, Rich::Rich1,
                                    nullErrs, nullErrs ) );
  Segs.push_back( RichTrackSegment( RichTrackSegment::UseAllStateVectors(),
                                    Point(-279.149,-45.9607,1110),
                                    Vector(-534.221,-86.3879,2269.82),
                                    Point(-290.927,-47.8635,1160),
                                    Vector(-535.06,-86.3626,2269.62),
                                    Rich::Aerogel, Rich::Rich1,
                                    nullErrs, nullErrs ) );

  // C4f10 segments
  Segs.push_back( RichTrackSegment( RichTrackSegment::UseAllStateVectors(),
                                    Point(12.1593,43.7352,990),
                                    Vector(200.427,725.766,17990.2),
                                    Point(17.183,62.2349,1448.56),
                                    Vector(193.824,725.782,17990.2),
                                    Point(22.1956,80.5884,1907.12),
                                    Vector(190.165,723.361,17986.1),
                                    Rich::C4F10, Rich::Rich1,
                                    nullErrs, nullErrs, nullErrs ) );
  Segs.push_back( RichTrackSegment( RichTrackSegment::UseAllStateVectors(),
                                    Point(-37.1749,-8.16514,990),
                                    Vector(-658.922,-144.142,19349),
                                    Point(-52.3554,-11.5027,1438.01),
                                    Vector(-652.395,-144.149,19349.2),
                                    Point(-67.6491,-14.2807,1886.02),
                                    Vector(-649.358,-132.183,19353.4),
                                    Rich::C4F10, Rich::Rich1,
                                    nullErrs, nullErrs, nullErrs ) );

  // Cf4 segments
  Segs.push_back( RichTrackSegment( RichTrackSegment::UseAllStateVectors(),
                                    Point(-178.735,385.802,9501.4),
                                    Vector(-1021.86,719.726,17948.8),
                                    Point(-231.545,422.75,10422.8),
                                    Vector(-1033.78,719.67,17948.1),
                                    Point(-284.777,459.695,11344.3),
                                    Vector(-1038.67,719.252,17940.1),
                                    Rich::CF4, Rich::Rich2,
                                    nullErrs, nullErrs, nullErrs ) );
  Segs.push_back( RichTrackSegment( RichTrackSegment::UseAllStateVectors(),
                                    Point(160.598,47.8836,9500.18),
                                    Vector(1278.51,154.37,35339.7),
                                    Point(193.967,51.8904,10417.4),
                                    Vector(1278.51,154.37,35339.7),
                                    Point(227.559,55.8973,11334.7),
                                    Vector(1295.61,154.249,35312.6),
                                    Rich::CF4, Rich::Rich2,
                                    nullErrs, nullErrs, nullErrs ) );

  // test photon reconstruction
  for ( TrackSegments::const_iterator iS = Segs.begin(); iS != Segs.end(); ++iS )
  {
    for ( Points::const_iterator iD = dataPoints.begin(); iD != dataPoints.end(); ++iD )
    {
      if ( ( (*iD).z() < 5000 && (*iS).entryPoint().z() < 5000 ) ||
           ( (*iD).z() > 5000 && (*iS).entryPoint().z() > 5000 ) )
      {
        RichGeomPhoton photon;
        debug() << "Data Point " << *iD << endreq;
        debug() << "Track Segment " << *iS << endreq;
        const StatusCode sc = m_photonReco->reconstructPhoton( *iS, *iD, photon );
        debug() << "Status " << sc << " Photon " << photon << endreq;
      }
    }
  }


  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichPhotonRecoTestAlg::finalize()
{
  // Execute base class method
  return RichRecHistoAlgBase::finalize();
}
