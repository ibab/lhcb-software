// $Id: STMeasurement.cpp,v 1.13 2008-04-30 14:58:41 mneedham Exp $
// Include files 

// from STDet
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"
#include "STDet/DeSTSensor.h"

// from Event
#include "Event/STCluster.h"

// from Kernel
#include "Kernel/ISTClusterPosition.h"

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

/// Standard constructor, without the reference vector
STMeasurement::STMeasurement( const STCluster& stCluster,
                              const DeSTDetector& geom,
                              const ISTClusterPosition& stClusPosTool )
  : Measurement( Measurement::IT, LHCbID(stCluster.channelID()), 0), m_cluster(&stCluster)
{
  this->init( geom, stClusPosTool );
}


void STMeasurement::init( const DeSTDetector& geom,
                          const ISTClusterPosition& stClusPosTool) 
{
  // Fill the data members
  STChannelID stChan = m_cluster -> channelID();
  m_mtype = ( stChan.isTT() ? Measurement::TT : Measurement::IT );
 

  // Get the corresponding sensor
  // TODO: Add const functions in STDet
  DeSTDetector* tmpGeom = const_cast<DeSTDetector*>(&geom);
  const DeSTSector* stSector = tmpGeom->findSector( stChan );
  m_detectorElement = stSector ;

  // Get the centre of gravity and the measurement error
  ISTClusterPosition::Info measVal =
    stClusPosTool.estimate( m_cluster );
 
  m_errMeasure = measVal.fractionalError*stSector -> pitch();
  m_trajectory = stSector->trajectory( measVal.strip, measVal.fractionalPosition) ;

  Gaudi::XYZPoint point = m_trajectory->position( 0.5*( m_trajectory->beginRange()+m_trajectory->endRange())) ;

  m_z = point.z();

  // get the best sensor to and go local
  // this is the only way to ensure we get inside a 
  // sensor, and not into a bondgap
  const DeSTSector::Sensors& sensors = stSector->sensors();
  unsigned int sensorNum = sensors.size()/2u;
  DeSTSensor* sensor = sensors[sensorNum];

  m_measure = sensor->localU( stChan.strip() )
              + ( measVal.fractionalPosition* stSector -> pitch() );
 
}
