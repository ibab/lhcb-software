// $Id: OTMeasurement.cpp,v 1.8 2006-02-16 17:04:02 erodrigu Exp $
// Include files

// local
#include "Event/OTMeasurement.h"

using namespace Gaudi;
using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : OTMeasurement
//
// 2005-04-07 : Jose Hernando, Eduardo Rodrigues
// Author: Rutger van der Eijk
// Created: 07-04-1999
//-----------------------------------------------------------------------------

/// Standard constructor, initializes variables
OTMeasurement::OTMeasurement( const LHCb::OTTime& otTime,
                              const DeOTDetector& geom,
                              int ambiguity,
                              double tu )
{
  m_mtype = Measurement::OT;

  m_time      = &otTime;        // pointer to hit
  m_ambiguity = ambiguity;      // drift ambiguity
  m_tu        = tu;             // reference trajectory

  // set the LHCbID and ambiguity
  setLhcbID( LHCbID( m_time -> channel() ) );

  // some constants...
  double driftVelocity = geom.driftDelay(); // ns/mm
  // double wireVelocity  = geom.propagationDelay();      // ns/mm

  m_measure      = m_time->calibratedTime() * m_ambiguity;
  m_errMeasure   = geom.resolution() * driftVelocity;

  OTChannelID OTChan = m_time->channel();
  DeOTModule* module = geom.module( OTChan );
  XYZPoint wirePos = module->centerOfStraw( OTChan.straw() );

  m_stereoAngle = module->stereoAngle();
  // double mwirePos =  wirePos.x() * cos(stereoAngle) +
  //   wirePos.y() * sin(stereoAngle);
  m_z = wirePos.z();
  //  m_wireLength       = module->wireLength();

}

/// Get the reference vector from a estimated state vector
const TrackVector OTMeasurement::referenceVector( const TrackVector& 
                                                  stateVec ) const 
{
  // Initialize reference vector
  TrackVector refVec = TrackVector();

  // Check if the reference slope tu is set. If it is set: use it
  if ( m_tu > 990.0 ) {
    refVec = stateVec;
  } else {
    double tv = - stateVec(2) * sin( m_stereoAngle )
      + stateVec(3) * cos( m_stereoAngle );
    refVec[0] = stateVec(0);
    refVec[1] = stateVec(1);
    refVec[2] = m_tu * cos( m_stereoAngle ) - tv * sin( m_stereoAngle );
    refVec[3] = m_tu * sin( m_stereoAngle ) + tv * cos( m_stereoAngle );
    refVec[4] = stateVec(4);
  }
  return refVec;
}
