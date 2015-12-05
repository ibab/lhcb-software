// $Id: VeloLiteRMeasurement.cpp,v 1.1 2009-01-19 11:49:37 dhcroft Exp $
// Include files

// From VeloDet
#include "VeloDet/DeVelo.h"

// From Event
#include "Event/VeloCluster.h"

// local
#include "Event/VeloLiteRMeasurement.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : VeloLiteRMeasurement
//
// Author: David Hutchcroft
// Created: 2008-11-18
//-----------------------------------------------------------------------------

/// Standard constructor, initializes variables
VeloLiteRMeasurement::VeloLiteRMeasurement( const VeloLiteCluster& aCluster,
                                    const DeVelo& det,
                                    const IVeloClusterPosition& clusPosTool,
                                    const LHCb::StateVector& refVector )
  : VeloLiteMeasurement(Measurement::VeloLiteR,aCluster)

{
  IVeloClusterPosition::toolInfo clusInfo =
    clusPosTool.position(this->cluster(),refVector.position(),
                         std::pair<double,double>(refVector.tx(),refVector.ty())) ;
  this->init( det, clusInfo ) ;
}

/// Standard constructor, initializes variables
VeloLiteRMeasurement::VeloLiteRMeasurement( const VeloLiteCluster& aCluster,
                                    const DeVelo& det,
                                    const IVeloClusterPosition& clusPosTool)
  : VeloLiteMeasurement(Measurement::VeloLiteR,aCluster)
{
  IVeloClusterPosition::toolInfo clusInfo = clusPosTool.position(this->cluster());
  this->init( det, clusInfo ) ;
}

void VeloLiteRMeasurement::init( const DeVelo& det, const IVeloClusterPosition::toolInfo& clusInfo)
{
  // Fill the data members
  const DeVeloRType* rDet=det.rSensor( channelID().sensor() );
  m_detectorElement = rDet ;
  m_z = rDet -> z();

  m_measure = rDet -> rOfStrip( clusInfo.strip.strip() ) +
    rDet -> rPitch( clusInfo.strip.strip() ) * clusInfo.fractionalPosition;
  m_errMeasure = rDet -> rPitch( clusInfo.strip.strip() )
    * clusInfo.fractionalError;

  m_trajectory = rDet -> trajectory( clusInfo.strip, clusInfo.fractionalPosition );
}


const DeVeloRType& VeloLiteRMeasurement::sensor() const{
  return *static_cast<const DeVeloRType *>(detectorElement());
}

