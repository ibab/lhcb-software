// $Id: ITMeasurement.cpp,v 1.2 2005-05-13 13:09:08 erodrigu Exp $
// Include files 

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
                              DeSTDetector& geom ) {

  m_cluster = &itCluster; //pointer to ITCluster

  ITChannelID ITChan = m_cluster->channelID();

  // set the LHCbID
  setLhcbID ( LHCbID( ITChan ) );

  const STDetectionLayer* ITLay = geom.layer( ITChan );
  m_measure      = ITLay->U( ITChan ) + m_cluster->distToStripCenter() ;
  m_errMeasure    = m_cluster->distToStripError();
  m_z            = ITLay->centerZ( ITChan );
//  m_stereoAngle  = ITLay->stereoAngle();
//  m_lhcbID = 
}

//=============================================================================
