// $Id: VLPhiMeasurement.cpp,v 1.23 2008-11-14 10:33:37 mneedham Exp $
// Include files 

// From Event
#include "Event/VLCluster.h"

// From VLDet
#include "VLDet/DeVL.h"

// local
#include "Event/VLPhiMeasurement.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : VLPhiMeasurement
//
// 2005-04-07 : Jose Hernando, Eduardo Rodrigues
// Author: Rutger van der Eijk
// Created: 13-07-2000
//-----------------------------------------------------------------------------

/// Standard constructor, initializes variables
VLPhiMeasurement::VLPhiMeasurement( const VLCluster& aCluster,
                                        const DeVL& det,
                                        const IVLClusterPosition& clusPosTool,
                                        const LHCb::StateVector& refVector )
  : VLMeasurement(Measurement::VLPhi,aCluster)
{
  IVLClusterPosition::toolInfo clusInfo = 
    clusPosTool.position(this->cluster(),refVector.position(),
                         refVector.tx(),refVector.ty()) ;
  this->init( det, clusInfo ) ;
}

/// Standard constructor, initializes variables
VLPhiMeasurement::VLPhiMeasurement( const VLCluster& aCluster,					
                                        const DeVL& det,
                                        const IVLClusterPosition& clusPosTool) 
  : VLMeasurement(Measurement::VLPhi,aCluster)
{
  IVLClusterPosition::toolInfo clusInfo = clusPosTool.position(cluster());
  this->init( det, clusInfo ) ;
}

void VLPhiMeasurement::init( const DeVL& det, const IVLClusterPosition::toolInfo& clusInfo)
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

double VLPhiMeasurement::resolution( const Gaudi::XYZPoint& point,
                                       const Gaudi::XYZVector& /*vec*/) const
{
  // Retrieve the measurement error
  const double radius = (m_origin - point).R();
  return radius * m_errMeasure;
}

double VLPhiMeasurement::resolution2( const Gaudi::XYZPoint& point,
                                        const Gaudi::XYZVector& /*vec*/) const
{
  // Retrieve the measurement error
  const double radius = (m_origin - point).R();
  return radius * m_errMeasure * radius * m_errMeasure;
}

const DeVLPhiSensor& VLPhiMeasurement::sensor() const{                                                
  return *static_cast<const DeVLPhiSensor *>(detectorElement()); 
}                                   


