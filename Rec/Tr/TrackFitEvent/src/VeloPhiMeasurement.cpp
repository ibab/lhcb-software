// $Id: VeloPhiMeasurement.cpp,v 1.19 2007-06-06 15:05:06 wouter Exp $
// Include files 

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
                                        const Gaudi::TrackVector& refVector )
{
  m_refVector = refVector; // reference trajectory
  this->init( cluster, det, clusPosTool, true );
}

/// Standard constructor, initializes variables
VeloPhiMeasurement::VeloPhiMeasurement( const VeloCluster& cluster,					
                                        const DeVelo& det,
                                        const IVeloClusterPosition& clusPosTool)
{
  m_refVector = Gaudi::TrackVector(); // reference trajectory
  this->init( cluster, det, clusPosTool, false );
}

/// Copy constructor
VeloPhiMeasurement::VeloPhiMeasurement( const VeloPhiMeasurement& other )
  : Measurement(other) {
  m_cluster = other.m_cluster;
  m_origin = other.m_origin;
}

void VeloPhiMeasurement::init( const VeloCluster& cluster,
                               const DeVelo& det,
                               const IVeloClusterPosition& clusPosTool,
                               bool refIsSet ) 
{
  // Fill the data members
  m_mtype    = Measurement::VeloPhi;
  m_refIsSet = refIsSet;
  m_cluster  = &cluster;
  m_lhcbID   = LHCbID( m_cluster->channelID() );

  const DeVeloPhiType* phiDet = det.phiSensor( m_cluster->channelID() );
  m_z = phiDet->z();
  m_origin = phiDet -> globalOrigin();
  
  // Store only the 'position', which is the signed distance from strip to
  // the origin.
  m_measure = phiDet -> distToOrigin(  m_cluster->channelID().strip() );
  // fix sign convention of d0 of strip
  if( ! phiDet -> isDownstream() ) {
    m_measure = -m_measure;
  }

  IVeloClusterPosition::toolInfo clusInfo ;
  if(!refIsSet) {
    clusInfo = clusPosTool.position( &cluster );
  } else {
    Gaudi::XYZPoint point( m_refVector[0], m_refVector[1], m_z ) ;
    clusInfo = clusPosTool.position(&cluster,point,std::pair<double,double>(m_refVector[2],m_refVector[3]) );
  }
  m_errMeasure  = clusInfo.fractionalError *
    phiDet -> phiPitch( clusInfo.strip.strip() );  

  m_trajectory = phiDet -> trajectory( clusInfo.strip, 
				       clusInfo.fractionalPosition );

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
