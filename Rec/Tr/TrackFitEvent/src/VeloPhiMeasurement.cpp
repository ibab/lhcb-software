// $Id: VeloPhiMeasurement.cpp,v 1.22 2008-02-07 16:13:26 cattanem Exp $
// Include files 

// From Event
#include "Event/VeloCluster.h"

// From VeloDet
#include "VeloDet/DeVelo.h"

// local
#include "Event/VeloPhiMeasurement.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : VeloPhiMeasurement
//
// 2005-04-07 : Jose Hernando, Eduardo Rodrigues
// Author: Rutger van der Eijk
// Created: 13-07-2000
//-----------------------------------------------------------------------------

/// Standard constructor, initializes variables
VeloPhiMeasurement::VeloPhiMeasurement( const VeloCluster& cluster,
                                        const DeVelo& det,
                                        const IVeloClusterPosition& clusPosTool,
                                        const LHCb::StateVector& refVector )
  : Measurement(Measurement::VeloPhi,cluster.channelID()),m_cluster(&cluster)
{
  IVeloClusterPosition::toolInfo clusInfo = 
    clusPosTool.position(m_cluster,refVector.position(),
                         std::pair<double,double>(refVector.tx(),refVector.ty())) ;
  this->init( det, clusInfo ) ;
}

/// Standard constructor, initializes variables
VeloPhiMeasurement::VeloPhiMeasurement( const VeloCluster& cluster,					
                                        const DeVelo& det,
                                        const IVeloClusterPosition& clusPosTool) 
  : Measurement(Measurement::VeloPhi,cluster.channelID()),m_cluster(&cluster)
{
  IVeloClusterPosition::toolInfo clusInfo = clusPosTool.position(m_cluster);
  this->init( det, clusInfo ) ;
}

void VeloPhiMeasurement::init( const DeVelo& det, const IVeloClusterPosition::toolInfo& clusInfo)
{
  // Fill the data members
  const DeVeloPhiType* phiDet = det.phiSensor( m_cluster->channelID() );
  m_detectorElement = phiDet ;
  m_z = phiDet->z();
  m_origin = phiDet -> globalOrigin();
  
  // Store only the 'position', which is the signed distance from strip to
  // the origin.
  m_measure = phiDet -> distToOrigin(  m_cluster->channelID().strip() );
  // fix sign convention of d0 of strip
  if( ! phiDet -> isDownstream() ) {
    m_measure = -m_measure;
  }
  
  m_errMeasure  = clusInfo.fractionalError *
    phiDet -> phiPitch( clusInfo.strip.strip() );  
  
  m_trajectory = phiDet -> trajectory( clusInfo.strip, clusInfo.fractionalPosition );
}

double VeloPhiMeasurement::resolution( const Gaudi::XYZPoint& point,
                                       const Gaudi::XYZVector& /*vec*/) const
{
  // Retrieve the measurement error
  double radius = (m_origin - point).R();
  return radius * m_errMeasure;
}

double VeloPhiMeasurement::resolution2( const Gaudi::XYZPoint& point,
                                        const Gaudi::XYZVector& /*vec*/) const
{
  // Retrieve the measurement error
  double radius = (m_origin - point).R();
  return radius * m_errMeasure * radius * m_errMeasure;
}
