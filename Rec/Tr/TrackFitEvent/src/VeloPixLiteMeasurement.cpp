// $Id: VeloPixLiteMeasurement.cpp,v 1.1 2010-04-13 08:58:10 cocov Exp $
// Include files 

// local
#include "Event/VeloPixLiteMeasurement.h"
#include "VeloPixDet/DeVeloPix.h"
#include "Kernel/LineTraj.h"
#include "LHCbMath/LHCbMath.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : VeloPixLiteMeasurement
//
// Author: Victor Coco, based on solution used in MuonMeasurement by Jose Hernando, 
// Miriam Gandelman, Erica Polycarpo, Silvia Pozzi
// Created: 12-02-2010
//-----------------------------------------------------------------------------


/// Constructor out of the VeloPixLite cluster, dx, dy are the error on the cluster...
VeloPixLiteMeasurement::VeloPixLiteMeasurement( const DeVeloPix& velopixdet,const VeloPixLiteCluster& acluster,
                                                const IVeloPixClusterPosition& clusPosTool, 
                                                VeloPixLiteMeasurement::VeloPixLiteMeasurementType& XY)
  : Measurement(Measurement::VeloPixLite,acluster.channelID(),0),
    m_veloPixProjection(XY),
    m_cluster(&acluster)
{
  IVeloPixClusterPosition::toolInfo clusInfo = 
    clusPosTool.position(this->cluster()) ; 
  this->init( velopixdet, clusInfo ) ;
  
}
/// Constructor out of the VeloPixLite cluster knowing the ref track state, dx, dy are the error on the cluster...
VeloPixLiteMeasurement::VeloPixLiteMeasurement( const DeVeloPix& velopixdet,const VeloPixLiteCluster& acluster,
                                                const IVeloPixClusterPosition& clusPosTool, 
                                                VeloPixLiteMeasurement::VeloPixLiteMeasurementType& XY,
                                                const LHCb::StateVector& refVector)
  : Measurement(Measurement::VeloPixLite,acluster.channelID(),0),
  m_veloPixProjection(XY),
    m_cluster(&acluster)
{
  IVeloPixClusterPosition::toolInfo clusInfo = 
    clusPosTool.position(this->cluster(),refVector.position(),
                         std::pair<double,double>(refVector.tx(),refVector.ty())) ; 
  this->init( velopixdet, clusInfo ) ;  
}

void VeloPixLiteMeasurement::init( const DeVeloPix& det, const IVeloPixClusterPosition::toolInfo& clusInfo )
{
  m_detectorElement = det.squareSensor(clusInfo.pixel.sensor() );
  const DeVeloPixSquareType& sqDet=this->sensor();
  //std::pair<double,double> thePixPoint = sqDet.globalXYZ(clusInfo.pixel.pixel(),clusInfo.fractionalPosition) ;
  Gaudi::XYZPoint position =  sqDet.globalXYZ(clusInfo.pixel.pixel(),clusInfo.fractionalPosition) ;

  std::pair<double,double> pixSize = sqDet.PixelSize(clusInfo.pixel.pixel());

  //// CHEATED..................................
  if(position.x()>0 && clusInfo.pixel.chip()%2!=0 )position.SetX(position.x()+pixSize.first);
  else if(position.x()<0 && clusInfo.pixel.chip()%2!=0 ) position.SetX(position.x()-pixSize.first);
  //// CHEATED..................................END

  m_position = position;
  double dx = pixSize.first*clusInfo.fractionalError.first ;
  //// CHEATED..................................
  if (sqDet.isLong(clusInfo.pixel.pixel())) dx = 0.1 ;
  //// CHEATED..................................END
  double dy = pixSize.second*clusInfo.fractionalError.second;
  setZ(position.z());
  // Case of the X projection
  if(m_veloPixProjection == X){
    setMeasure(position.x());
    setErrMeasure(2.*dx);
    Gaudi::XYZVector y_dir(0,1,0); 
    std::pair<double,double> y_range(-dy,dy);
    m_trajectory = std::auto_ptr<Trajectory>(new LineTraj(position, y_dir, y_range, true));

  }
  // Case of the Y projection
  else{
    setMeasure(position.y());
    setErrMeasure(2.*dy);
    Gaudi::XYZVector x_dir(1,0,0); 
    std::pair<double,double> x_range(-dx,dx);
    m_trajectory = std::auto_ptr<Trajectory>(new LineTraj(position, x_dir, x_range, true));  

  }
}

/// Copy constructor
VeloPixLiteMeasurement::VeloPixLiteMeasurement( const VeloPixLiteMeasurement& other ) 
  : Measurement(other),m_veloPixProjection(other.m_veloPixProjection), m_cluster(other.m_cluster) {
}


const DeVeloPixSquareType& VeloPixLiteMeasurement::sensor() const{
  return *static_cast<const DeVeloPixSquareType *>(detectorElement());
}
