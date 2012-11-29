// Include files 

// from FTDet
#include "FTDet/DeFTDetector.h"

// from Event
#include "Event/FTCluster.h"
#include "Kernel/LineTraj.h"
// local
#include "Event/FTMeasurement.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : FTMeasurement
//
//  2012-11-27 Olivier Callot, from STMeasurement
//-----------------------------------------------------------------------------
void FTMeasurement::init( const DeFTDetector& geom ) {
 
  // Get the corresponding layer
  const DeFTLayer* ftLayer = geom.findLayer( m_cluster.channelID() );
  m_detectorElement = ftLayer;
  DetectorSegment mySeg = ftLayer->createDetSegment( m_cluster.channelID(), m_cluster.fraction() );
  Gaudi::XYZPoint begPoint( mySeg.x( mySeg.yMin() ), mySeg.yMin(), mySeg.z( mySeg.yMin() ) );
  Gaudi::XYZPoint endPoint( mySeg.x( mySeg.yMax() ), mySeg.yMax(), mySeg.z( mySeg.yMax() ) );
  
  m_size = m_cluster.size();
  m_errMeasure = 0.050 + .030 * m_size;
  LHCb::LineTraj* traj = new LHCb::LineTraj( begPoint, endPoint );
  m_trajectory = std::auto_ptr<LHCb::Trajectory>( traj );
  m_z = ftLayer->layerCenterZ();
  m_measure = mySeg.x( 0. );
}
