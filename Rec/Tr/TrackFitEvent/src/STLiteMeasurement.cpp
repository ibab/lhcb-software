// $Id: STLiteMeasurement.cpp,v 1.2 2008-11-06 12:41:26 mneedham Exp $
// Include files

// from STDet
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"
#include "STDet/DeSTSensor.h"

// from Event
#include "Event/STLiteCluster.h"

// from Kernel
#include "TrackInterfaces/ISTClusterPosition.h"

// local
#include "Event/STLiteMeasurement.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : STLiteMeasurement
//
// 2005-04-07 : Jose Hernando, Eduardo Rodrigues
// Author: Rutger van der Eijk
// Created: 07-04-1999
//-----------------------------------------------------------------------------

/// Standard constructor, initializes variables
STLiteMeasurement::STLiteMeasurement( const STLiteCluster& stCluster,
                              const DeSTDetector& geom,
                              const ISTClusterPosition& stClusPosTool,
                              const LHCb::StateVector& /*refVector*/)
  : Measurement(Measurement::ITLite, LHCbID(stCluster.channelID()), 0), m_cluster(stCluster)
{
  this->init( geom, stClusPosTool );
}

void STLiteMeasurement::init( const DeSTDetector& geom,
                          const ISTClusterPosition& stClusPosTool)
{
  // Fill the data members
  m_mtype = ( m_cluster.isTT() ? Measurement::TTLite :
              m_cluster.isIT() ? Measurement::ITLite :
              Measurement::UTLite );

  // Get the corresponding sensor
  const DeSTSector* stSector = geom.findSector( m_cluster.channelID() );
  m_detectorElement = stSector ;

  m_errMeasure = stClusPosTool.error(m_cluster.pseudoSize())*stSector->pitch();
  m_trajectory = stSector->trajectory( m_cluster.channelID(), m_cluster.interStripFraction()) ;
  m_z = stSector->globalCentre().z();

  // get the best sensor to and go local
  // this is the only way to ensure we get inside a
  // sensor, and not into a bondgap
  m_measure = stSector->middleSensor()->localU( m_cluster.strip() )
              + ( m_cluster.interStripFraction() * stSector -> pitch() );

}
