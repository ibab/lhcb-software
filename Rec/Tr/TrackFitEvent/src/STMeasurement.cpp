// $Id: STMeasurement.cpp,v 1.11 2008-02-07 16:13:26 cattanem Exp $
// Include files 

// from STDet
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"

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
  m_measure    = stSector -> localU( stChan.strip() )
                 + ( measVal.fractionalPosition* stSector -> pitch() );
  m_errMeasure = measVal.fractionalError*stSector -> pitch();
  m_trajectory = tmpGeom->trajectory( LHCbID(measVal.strip), measVal.fractionalPosition) ;

  // Use the z of the centre of the strip
  m_z = m_trajectory->position(0.0).z();

}
