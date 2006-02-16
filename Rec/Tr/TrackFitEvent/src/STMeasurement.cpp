// $Id: STMeasurement.cpp,v 1.3 2006-02-16 17:04:02 erodrigu Exp $
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
                              ISTClusterPosition& stClusPosTool) {

  m_cluster = &stCluster; //pointer to STCluster

  STChannelID stChan = m_cluster -> channelID();

  m_mtype = ( stChan.isTT() ? Measurement::TT : Measurement::IT );

  // set the LHCbID
  setLhcbID ( LHCbID( stChan ) );

  const DeSTSector* stLay = geom.sectors()[stChan.strip()];

  ISTClusterPosition::Measurement measVal =
    stClusPosTool.estimate( &stCluster );
  m_measure    = stLay -> localU( stChan.strip() )
                 + ( measVal.first.second * stLay -> pitch() );
  m_errMeasure = measVal.second;

/// ERROR!!!
  m_z            = 0.;
//  m_stereoAngle  = stLay->stereoAngle();

//  std::cout << "- stereo angle = " << stLay->stereoAngle() << std::endl;

}
