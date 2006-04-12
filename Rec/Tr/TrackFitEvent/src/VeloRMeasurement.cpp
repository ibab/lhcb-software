// $Id: VeloRMeasurement.cpp,v 1.12 2006-04-12 14:38:25 mtobin Exp $
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
                                    const Gaudi::TrackVector& refVector )
{
  m_refVector = refVector; // reference trajectory
  this->init( cluster, det, clusPosTool, true );
}

/// Standard constructor, initializes variables
VeloRMeasurement::VeloRMeasurement( const VeloCluster& cluster,
                                    const DeVelo& det, 
				    const IVeloClusterPosition& clusPosTool) 
{
  m_refVector = Gaudi::TrackVector(); // reference trajectory
  this->init( cluster, det, clusPosTool, false );
}

/// Copy constructor
VeloRMeasurement::VeloRMeasurement( const VeloRMeasurement& other )
  : Measurement(other) {
  m_cluster = other.m_cluster;
}

void VeloRMeasurement::init( const VeloCluster& cluster,
                             const DeVelo& det,
                             const IVeloClusterPosition& clusPosTool,
			     bool refIsSet ) 
{
  // Fill the data members
  m_mtype = Measurement::VeloR;
  m_refIsSet  = refIsSet;
  m_cluster = &cluster;
  m_lhcbID = LHCbID( m_cluster->channelID() );
  const DeVeloRType* rDet=det.rSensor( m_cluster->channelID().sensor() );
  m_z = rDet->z();
  m_trajectory = rDet->trajectory( m_lhcbID.veloID(), m_cluster->interStripFraction() );

  IVeloClusterPosition::toolInfo clusInfo = clusPosTool.position( &cluster );
  m_measure = rDet->rOfStrip(clusInfo.strip) + 
    rDet->rPitch(clusInfo.strip.strip()) * clusInfo.fractionalPosition;
  m_errMeasure = rDet->rPitch(clusInfo.strip.strip()) * clusInfo.fractionalError;
}


