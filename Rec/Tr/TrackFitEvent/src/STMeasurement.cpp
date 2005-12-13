// $Id: STMeasurement.cpp,v 1.1 2005-12-13 19:04:52 erodrigu Exp $
// Include files 

// from STDet
#include "STDet/STDetectionLayer.h"

// local
#include "Event/STMeasurement.h"

//-----------------------------------------------------------------------------
// Implementation file for class : STMeasurement
//
// 2005-04-07 : Jose Hernando, Eduardo Rodrigues
// Author: Rutger van der Eijk
// Created: 07-04-1999
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
STMeasurement::STMeasurement( ITCluster& itCluster,
                              DeSTDetector& geom,
                              ISTClusterPosition& itClusPosTool) {

  m_cluster = &itCluster; //pointer to ITCluster

  ITChannelID itChan = m_cluster -> channelID();

  m_mtype = ( itChan.isTT() ? Measurement::TT : Measurement::IT );

  // set the LHCbID
  setLhcbID ( LHCbID( itChan ) );

  const STDetectionLayer* itLay = geom.layer( itChan );

  ISTClusterPosition::Measurement measVal =
    itClusPosTool.estimate( &itCluster );
  m_measure    = itLay -> U( itChan )
                 + ( measVal.first.second * itLay -> pitch() );
  m_errMeasure = measVal.second;

  m_z            = itLay -> centerZ( itChan );
//  m_stereoAngle  = itLay->stereoAngle();

//  std::cout << "- stereo angle = " << itLay->stereoAngle() << std::endl;

}

//=============================================================================
