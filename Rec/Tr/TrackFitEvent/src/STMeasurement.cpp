// $Id: STMeasurement.cpp,v 1.16 2009-01-19 11:23:56 mneedham Exp $
// Include files

// from STDet
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"
#include "STDet/DeSTSensor.h"

// from Event
#include "Event/STCluster.h"

// from Kernel
#include "TrackInterfaces/ISTClusterPosition.h"

// local
#include "Event/STMeasurement.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : STMeasurement
//
// 2005-04-07 : Jose Hernando, Eduardo Rodrigues
// Author: Rutger van der Eijk
// Created: 07-04-1999
//-----------------------------------------------------------------------------

/// Standard constructor, initializes variables
STMeasurement::STMeasurement( const STCluster& stCluster,
                              const DeSTDetector& geom,
                              const ISTClusterPosition& stClusPosTool,
                             const LHCb::StateVector& /*refVector*/)
  : Measurement( Measurement::IT, LHCbID(stCluster.channelID()), 0), m_cluster(&stCluster)
{
 this->init( geom, stClusPosTool );
}

void STMeasurement::init( const DeSTDetector& geom,
                          const ISTClusterPosition& stClusPosTool)
{
  // Fill the data members
  m_mtype = ( m_cluster->isTT() ? Measurement::TT :
	      (m_cluster->isUT() ? Measurement::UT : Measurement::IT ) ) ;

  // Get the corresponding sensor
  const DeSTSector* stSector = geom.findSector( m_cluster->channelID() );
  m_detectorElement = stSector ;

  // Get the centre of gravity and the measurement error
  ISTClusterPosition::Info measVal =
    stClusPosTool.estimate( m_cluster );

  m_errMeasure = measVal.fractionalError*stSector -> pitch();
  m_size = measVal.clusterSize;
  m_trajectory = stSector->trajectory( measVal.strip, measVal.fractionalPosition) ;
  m_z = stSector->globalCentre().z();

  // get the best sensor to and go local
  // this is the only way to ensure we get inside a
  // sensor, and not into a bondgap
  m_measure = stSector->middleSensor()->localU( m_cluster->strip() )
              + ( measVal.fractionalPosition* stSector -> pitch() );

}
