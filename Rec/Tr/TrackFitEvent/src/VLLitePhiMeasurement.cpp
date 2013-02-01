// $Id: VLLitePhiMeasurement.cpp,v 1.1 2009-01-19 11:49:37 dhcroft Exp $
// Include files 

// From Event
#include "Event/VLCluster.h"
#include "Event/VLLiteCluster.h"

// From VLDet
#include "VLDet/DeVL.h"

// local
#include "Event/VLLitePhiMeasurement.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : VLLitePhiMeasurement
//
// Author: David Hutchcroft
// Created: 2008-11-18
//-----------------------------------------------------------------------------

/// Standard constructor, initializes variables
VLLitePhiMeasurement::VLLitePhiMeasurement( const VLLiteCluster& aCluster,
                                        const DeVL& det,
                                        const IVLClusterPosition& clusPosTool,
                                        const LHCb::StateVector& refVector )
  : VLLiteMeasurement(Measurement::VLLitePhi,aCluster)
{
  IVLClusterPosition::toolInfo clusInfo = 
    clusPosTool.position(this->cluster(),refVector.position(),
                         refVector.tx(),refVector.ty()) ;
  this->init( det, clusInfo ) ;
}

/// Standard constructor, initializes variables
VLLitePhiMeasurement::VLLitePhiMeasurement( const VLLiteCluster& aCluster,					
                                        const DeVL& det,
                                        const IVLClusterPosition& clusPosTool) 
  : VLLiteMeasurement(Measurement::VLLitePhi,aCluster)
{
  IVLClusterPosition::toolInfo clusInfo = clusPosTool.position(cluster());
  this->init( det, clusInfo ) ;
}

void VLLitePhiMeasurement::init( const DeVL& det, const IVLClusterPosition::toolInfo& clusInfo)
{
  // Fill the data members
  const DeVLPhiSensor* phiDet = det.phiSensor( channelID() );
  m_detectorElement = phiDet ;
  m_z = phiDet->z();
  //m_origin = phiDet -> globalOrigin();
  m_origin = Gaudi::XYZPoint(phiDet->xCentre(), phiDet->yCentre(), m_z);
  // Store only the 'position', which is the signed distance from strip to
  // the origin.
  m_measure = phiDet -> distToOrigin(  channelID().strip() );
  // fix sign convention of d0 of strip
  if( ! phiDet -> isDownstream() ) {
    m_measure = -m_measure;
  }
  
  m_errMeasure  = clusInfo.fractionalError *
    phiDet -> phiPitchOfStrip( clusInfo.strip.strip() );  
  
  m_trajectory = phiDet -> trajectory( clusInfo.strip, clusInfo.fractionalPosition );
}

double VLLitePhiMeasurement::resolution( const Gaudi::XYZPoint& point,
                                       const Gaudi::XYZVector& /*vec*/) const
{
  // Retrieve the measurement error
  const double radius = (m_origin - point).R();
  return radius * m_errMeasure;
}

double VLLitePhiMeasurement::resolution2( const Gaudi::XYZPoint& point,
                                        const Gaudi::XYZVector& /*vec*/) const
{
  // Retrieve the measurement error
  const double radius = (m_origin - point).R();
  return radius * m_errMeasure * radius * m_errMeasure;
}

const DeVLPhiSensor& VLLitePhiMeasurement::sensor() const{                                                
  return *static_cast<const DeVLPhiSensor *>(detectorElement()); 
}                                   


