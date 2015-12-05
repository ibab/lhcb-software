// $Id: VeloLitePhiMeasurement.cpp,v 1.1 2009-01-19 11:49:37 dhcroft Exp $
// Include files

// From Event
#include "Event/VeloCluster.h"

// From VeloDet
#include "VeloDet/DeVelo.h"

// local
#include "Event/VeloLitePhiMeasurement.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : VeloLitePhiMeasurement
//
// Author: David Hutchcroft
// Created: 2008-11-18
//-----------------------------------------------------------------------------

/// Standard constructor, initializes variables
VeloLitePhiMeasurement::VeloLitePhiMeasurement( const VeloLiteCluster& aCluster,
                                        const DeVelo& det,
                                        const IVeloClusterPosition& clusPosTool,
                                        const LHCb::StateVector& refVector )
  : VeloLiteMeasurement(Measurement::VeloLitePhi,aCluster)
{
  IVeloClusterPosition::toolInfo clusInfo =
    clusPosTool.position(this->cluster(),refVector.position(),
                         std::pair<double,double>(refVector.tx(),refVector.ty())) ;
  this->init( det, clusInfo ) ;
}

/// Standard constructor, initializes variables
VeloLitePhiMeasurement::VeloLitePhiMeasurement( const VeloLiteCluster& aCluster,					
                                        const DeVelo& det,
                                        const IVeloClusterPosition& clusPosTool)
  : VeloLiteMeasurement(Measurement::VeloLitePhi,aCluster)
{
  IVeloClusterPosition::toolInfo clusInfo = clusPosTool.position(cluster());
  this->init( det, clusInfo ) ;
}

void VeloLitePhiMeasurement::init( const DeVelo& det, const IVeloClusterPosition::toolInfo& clusInfo)
{
  // Fill the data members
  const DeVeloPhiType* phiDet = det.phiSensor( channelID() );
  m_detectorElement = phiDet ;
  m_z = phiDet->z();
  m_origin = phiDet -> globalOrigin();

  // Store only the 'position', which is the signed distance from strip to
  // the origin.
  m_measure = phiDet -> distToOrigin(  channelID().strip() );
  // fix sign convention of d0 of strip
  if( ! phiDet -> isDownstream() ) {
    m_measure = -m_measure;
  }

  m_errMeasure  = clusInfo.fractionalError *
    phiDet -> phiPitch( clusInfo.strip.strip() );

  m_trajectory = phiDet -> trajectory( clusInfo.strip, clusInfo.fractionalPosition );
}

double VeloLitePhiMeasurement::resolution( const Gaudi::XYZPoint& point,
                                       const Gaudi::XYZVector& /*vec*/) const
{
  // Retrieve the measurement error
  const double radius = (m_origin - point).R();
  return radius * m_errMeasure;
}

double VeloLitePhiMeasurement::resolution2( const Gaudi::XYZPoint& point,
                                        const Gaudi::XYZVector& /*vec*/) const
{
  // Retrieve the measurement error
  const double radius = (m_origin - point).R();
  return radius * m_errMeasure * radius * m_errMeasure;
}

const DeVeloPhiType& VeloLitePhiMeasurement::sensor() const{
  return *static_cast<const DeVeloPhiType *>(detectorElement());
}


