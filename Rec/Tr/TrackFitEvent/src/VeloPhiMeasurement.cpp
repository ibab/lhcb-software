// $Id: VeloPhiMeasurement.cpp,v 1.23 2008-11-14 10:33:37 mneedham Exp $
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
VeloPhiMeasurement::VeloPhiMeasurement( const VeloCluster& aCluster,
                                        const DeVelo& det,
                                        const IVeloClusterPosition& clusPosTool,
                                        const LHCb::StateVector& refVector )
  : VeloMeasurement(Measurement::VeloPhi,aCluster)
{
  IVeloClusterPosition::toolInfo clusInfo =
    clusPosTool.position(this->cluster(),refVector.position(),
                         std::pair<double,double>(refVector.tx(),refVector.ty())) ;
  this->init( det, clusInfo ) ;
}

/// Standard constructor, initializes variables
VeloPhiMeasurement::VeloPhiMeasurement( const VeloCluster& aCluster,					
                                        const DeVelo& det,
                                        const IVeloClusterPosition& clusPosTool)
  : VeloMeasurement(Measurement::VeloPhi,aCluster)
{
  IVeloClusterPosition::toolInfo clusInfo = clusPosTool.position(cluster());
  this->init( det, clusInfo ) ;
}

void VeloPhiMeasurement::init( const DeVelo& det, const IVeloClusterPosition::toolInfo& clusInfo)
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

double VeloPhiMeasurement::resolution( const Gaudi::XYZPoint& point,
                                       const Gaudi::XYZVector& /*vec*/) const
{
  // Retrieve the measurement error
  const double radius = (m_origin - point).R();
  return radius * m_errMeasure;
}

double VeloPhiMeasurement::resolution2( const Gaudi::XYZPoint& point,
                                        const Gaudi::XYZVector& /*vec*/) const
{
  // Retrieve the measurement error
  const double radius = (m_origin - point).R();
  return radius * m_errMeasure * radius * m_errMeasure;
}

const DeVeloPhiType& VeloPhiMeasurement::sensor() const{
  return *static_cast<const DeVeloPhiType *>(detectorElement());
}


