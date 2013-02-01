// $Id: VLLiteRMeasurement.cpp,v 1.1 2009-01-19 11:49:37 dhcroft Exp $
// Include files

// From VLDet
#include "VLDet/DeVL.h"

// From Event
#include "Event/VLCluster.h"
#include "Event/VLLiteCluster.h"

// local
#include "Event/VLLiteRMeasurement.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : VLLiteRMeasurement
//
// Author: David Hutchcroft
// Created: 2008-11-18
//-----------------------------------------------------------------------------

/// Standard constructor, initializes variables
VLLiteRMeasurement::VLLiteRMeasurement( const VLLiteCluster& aCluster,
                                    const DeVL& det,
                                    const IVLClusterPosition& clusPosTool,
                                    const LHCb::StateVector& refVector )
  : VLLiteMeasurement(Measurement::VLLiteR,aCluster)

{
  IVLClusterPosition::toolInfo clusInfo = 
    clusPosTool.position(this->cluster(),refVector.position(),
                         refVector.tx(),refVector.ty()) ;
  this->init( det, clusInfo ) ;
}

/// Standard constructor, initializes variables
VLLiteRMeasurement::VLLiteRMeasurement( const VLLiteCluster& aCluster,
                                    const DeVL& det, 
                                    const IVLClusterPosition& clusPosTool) 
  : VLLiteMeasurement(Measurement::VLLiteR,aCluster)
{
  IVLClusterPosition::toolInfo clusInfo = clusPosTool.position(this->cluster());
  this->init( det, clusInfo ) ;
}

void VLLiteRMeasurement::init( const DeVL& det, const IVLClusterPosition::toolInfo& clusInfo)
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


const DeVLRSensor& VLLiteRMeasurement::sensor() const{
  return *static_cast<const DeVLRSensor *>(detectorElement());
}

