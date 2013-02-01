// $Id: VLRMeasurement.cpp,v 1.19 2008-11-14 10:33:37 mneedham Exp $
// Include files

// From VLDet
#include "VLDet/DeVL.h"

// From Event
#include "Event/VLCluster.h"

// local
#include "Event/VLRMeasurement.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : VLRMeasurement
//
// 2005-04-07 : Jose Hernando, Eduardo Rodrigues
// Author: Rutger van der Eijk
// Created: 27-06-2000
//-----------------------------------------------------------------------------

/// Standard constructor, initializes variables
VLRMeasurement::VLRMeasurement( const VLCluster& aCluster,
                                const DeVL& det,
                                const IVLClusterPosition& clusPosTool,
                                const LHCb::StateVector& refVector )
  : VLMeasurement(Measurement::VLR,aCluster)

{
  IVLClusterPosition::toolInfo clusInfo = 
    clusPosTool.position(this->cluster(),refVector.position(),
                         refVector.tx(),refVector.ty()) ;
  this->init( det, clusInfo ) ;
}

/// Standard constructor, initializes variables
VLRMeasurement::VLRMeasurement( const VLCluster& aCluster,
                                const DeVL& det, 
                                const IVLClusterPosition& clusPosTool) 
  : VLMeasurement(Measurement::VLR,aCluster)
{
  IVLClusterPosition::toolInfo clusInfo = clusPosTool.position(this->cluster());
  this->init( det, clusInfo ) ;
}

void VLRMeasurement::init( const DeVL& det, const IVLClusterPosition::toolInfo& clusInfo)
{
  // Fill the data members
  const DeVLRSensor* rDet=det.rSensor( channelID().sensor() );
  m_detectorElement = rDet ;
  m_z = rDet -> z();
  
  m_measure = rDet -> rOfStrip( clusInfo.strip.strip() ) +
    rDet -> rPitchOfStrip( clusInfo.strip.strip() ) * clusInfo.fractionalPosition;
  m_errMeasure = rDet -> rPitchOfStrip( clusInfo.strip.strip() )
    * clusInfo.fractionalError;

  m_trajectory = rDet -> trajectory( clusInfo.strip, clusInfo.fractionalPosition );
}


const DeVLRSensor& VLRMeasurement::sensor() const{
  return *static_cast<const DeVLRSensor *>(detectorElement());
}

