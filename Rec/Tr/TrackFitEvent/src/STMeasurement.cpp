// $Id: STMeasurement.cpp,v 1.4 2006-02-27 19:54:02 jvantilb Exp $
// Include files 

// from STDet
#include "STDet/DeSTSector.h"

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
                              ISTClusterPosition& stClusPosTool,
                              const Gaudi::TrackVector& refVector)
{
  m_refVector = refVector; // reference trajectory
  this->init( stCluster, geom, stClusPosTool, true );
}

/// Standard constructor, without the reference vector
STMeasurement::STMeasurement( const STCluster& stCluster,
                              const DeSTDetector& geom,
                              ISTClusterPosition& stClusPosTool )
{
  m_refVector = Gaudi::TrackVector(); // reference trajectory
  this->init(  stCluster, geom, stClusPosTool, false );
}

void STMeasurement::init( const STCluster& stCluster,
                          const DeSTDetector& geom,
                          ISTClusterPosition& stClusPosTool,
                          bool refIsSet ) 
{
  // Fill the data members
  m_cluster = &stCluster; //pointer to STCluster
  STChannelID stChan = m_cluster -> channelID();
  m_mtype = ( stChan.isTT() ? Measurement::TT : Measurement::IT );
  m_lhcbID = LHCbID( stChan ) ;
  m_refIsSet  = refIsSet;

  // Get the corresponding sensor
  // TODO: Add const functions in STDet
  DeSTDetector* tmpGeom = const_cast<DeSTDetector*>(&geom);
  DeSTSector* stSector = tmpGeom->findSector( stChan );

  // Get the centre of gravity and the measurement error
  ISTClusterPosition::Measurement measVal =
    stClusPosTool.estimate( &stCluster );
  m_measure    = stSector -> localU( stChan.strip() )
                 + ( measVal.first.second * stSector -> pitch() );
  m_errMeasure = measVal.second;
  m_trajectory = tmpGeom->trajectory( m_lhcbID, measVal.first.second ) ;

  // Use the z of the centre of the strip
  m_z = m_trajectory->position(0.0).z();

}
