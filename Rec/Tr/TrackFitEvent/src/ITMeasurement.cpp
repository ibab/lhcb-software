// $Id: ITMeasurement.cpp,v 1.5 2005-12-01 17:13:20 erodrigu Exp $
// Include files 

// from STDet
#include "STDet/STDetectionLayer.h"

// local
#include "Event/ITMeasurement.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ITMeasurement
//
// 2005-04-07 : Jose Hernando, Eduardo Rodrigues
// Author: Rutger van der Eijk
// Created: 07-04-1999
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ITMeasurement::ITMeasurement( ITCluster& itCluster,
                              DeSTDetector& geom,
                              ISTClusterPosition& itClusPosTool) {

  m_mtype = Measurement::ST;
  
  m_cluster = &itCluster; //pointer to ITCluster

  ITChannelID itChan = m_cluster->channelID();

  // set the LHCbID
  setLhcbID ( LHCbID( itChan ) );

  const STDetectionLayer* itLay = geom.layer( itChan );

  ISTClusterPosition::Measurement measVal =
    itClusPosTool.estimate( &itCluster );
  m_measure    = itLay -> U( itChan )
                 + ( measVal.first.second * itLay -> pitch() );
  m_errMeasure = measVal.second;

  m_z            = itLay->centerZ( itChan );
//  m_stereoAngle  = itLay->stereoAngle();

//  std::cout << "- stereo angle = " << itLay->stereoAngle() << std::endl;

}

//=============================================================================
