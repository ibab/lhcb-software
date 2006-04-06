// $Id: VeloPhiMeasurement.cpp,v 1.13 2006-04-06 13:57:09 dhcroft Exp $
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
  m_mtype = Measurement::VeloPhi;
  m_refIsSet  = refIsSet;
  m_cluster = &cluster;
  m_lhcbID = LHCbID( m_cluster->channelID() );
  const DeVeloPhiType* phiDet=det.phiSensor( m_cluster->channelID() );
  m_z = phiDet->z();
  m_trajectory = det.trajectory( m_lhcbID, m_cluster->interStripFraction() );

  // TODO: Get this from the VeloDet
  StatusCode sc = phiDet->globalOrigin(m_origin);
  if(!sc){
    throw GaudiException( "The origin of the phi sensor was bad",
			  "VeloPhiMeasurement.cpp",StatusCode::FAILURE );
  }

  // Store only the 'position', which is the signed distance from strip to
  // the origin.
  m_measure = phiDet->distToOrigin(  m_cluster->channelID().strip() );
  // fix sign convention of d0 of strip
  if( ! phiDet->isDownstream() ) {
    m_measure = -m_measure;
  }

  IVeloClusterPosition::toolInfo clusInfo = clusPosTool.position( &cluster );
  m_errMeasure  = clusInfo.fractionalError * 
    phiDet->phiPitch( m_cluster->channelID().strip() );

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
