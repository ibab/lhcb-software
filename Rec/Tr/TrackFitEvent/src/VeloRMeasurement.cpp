// $Id: VeloRMeasurement.cpp,v 1.19 2008-11-14 10:33:37 mneedham Exp $
// Include files

// From VeloDet
#include "VeloDet/DeVelo.h"

// From Event
#include "Event/VeloCluster.h"

// local
#include "Event/VeloRMeasurement.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : VeloRMeasurement
//
// 2005-04-07 : Jose Hernando, Eduardo Rodrigues
// Author: Rutger van der Eijk
// Created: 27-06-2000
//-----------------------------------------------------------------------------

/// Standard constructor, initializes variables
VeloRMeasurement::VeloRMeasurement( const VeloCluster& aCluster,
                                    const DeVelo& det,
                                    const IVeloClusterPosition& clusPosTool,
                                    const LHCb::StateVector& refVector )
  : VeloMeasurement(Measurement::VeloR,aCluster)

{
  IVeloClusterPosition::toolInfo clusInfo =
    clusPosTool.position(this->cluster(),refVector.position(),
                         std::pair<double,double>(refVector.tx(),refVector.ty())) ;
  this->init( det, clusInfo ) ;
}

/// Standard constructor, initializes variables
VeloRMeasurement::VeloRMeasurement( const VeloCluster& aCluster,
                                    const DeVelo& det,
                                    const IVeloClusterPosition& clusPosTool)
  : VeloMeasurement(Measurement::VeloR,aCluster)
{
  IVeloClusterPosition::toolInfo clusInfo = clusPosTool.position(this->cluster());
  this->init( det, clusInfo ) ;
}

void VeloRMeasurement::init( const DeVelo& det, const IVeloClusterPosition::toolInfo& clusInfo)
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


const DeVeloRType& VeloRMeasurement::sensor() const{                                                return *static_cast<const DeVeloRType *>(detectorElement());
}

