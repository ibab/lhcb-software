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

  const DeFTFibreMat* ftMat = geom.findFibreMat( m_cluster.channelID() );
  m_detectorElement = ftMat;
  DetectorSegment mySeg = ftMat->createDetSegment( m_cluster.channelID(), m_cluster.fraction() );
  Gaudi::XYZPoint begPoint( mySeg.x( mySeg.yMin() ), mySeg.yMin(), mySeg.z( mySeg.yMin() ) );
  Gaudi::XYZPoint endPoint( mySeg.x( mySeg.yMax() ), mySeg.yMax(), mySeg.z( mySeg.yMax() ) );

  m_size = m_cluster.size();
  m_errMeasure = 0.050 + .030 * m_size;
  m_trajectory.reset( new LHCb::LineTraj( begPoint, endPoint ) );
  m_z = ftMat->layerCenterZ();
  m_measure = mySeg.x( 0. );

}

