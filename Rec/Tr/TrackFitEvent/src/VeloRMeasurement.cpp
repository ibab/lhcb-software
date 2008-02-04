// $Id: VeloRMeasurement.cpp,v 1.17 2008-02-04 09:47:50 wouter Exp $
// Include files

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
VeloRMeasurement::VeloRMeasurement( const VeloCluster& cluster,
                                    const DeVelo& det,
                                    const IVeloClusterPosition& clusPosTool,
                                    const LHCb::StateVector& refVector )
  : Measurement(Measurement::VeloR,cluster.channelID()),m_cluster(&cluster)

{
  IVeloClusterPosition::toolInfo clusInfo = 
    clusPosTool.position(m_cluster,refVector.position(),
                         std::pair<double,double>(refVector.tx(),refVector.ty())) ;
  this->init( det, clusInfo ) ;
}

/// Standard constructor, initializes variables
VeloRMeasurement::VeloRMeasurement( const VeloCluster& cluster,
                                    const DeVelo& det, 
                                    const IVeloClusterPosition& clusPosTool) 
  : Measurement(Measurement::VeloR,cluster.channelID()),m_cluster(&cluster)
{
  IVeloClusterPosition::toolInfo clusInfo = clusPosTool.position(m_cluster);
  this->init( det, clusInfo ) ;
}

void VeloRMeasurement::init( const DeVelo& det, const IVeloClusterPosition::toolInfo& clusInfo)
{
  // Fill the data members
  const DeVeloRType* rDet=det.rSensor( m_cluster->channelID().sensor() );
  m_detectorElement = rDet ;
  m_z = rDet -> z();
  
  m_measure = rDet -> rOfStrip( clusInfo.strip.strip() ) +
    rDet -> rPitch( clusInfo.strip.strip() ) * clusInfo.fractionalPosition;
  m_errMeasure = rDet -> rPitch( clusInfo.strip.strip() )
    * clusInfo.fractionalError;

  m_trajectory = rDet -> trajectory( clusInfo.strip, clusInfo.fractionalPosition );
}
