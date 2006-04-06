// $Id: OTMeasurement.cpp,v 1.12 2006-04-06 06:43:28 ebos Exp $
// Include files

// OTDet
#include "OTDet/DeOTModule.h"

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
                              const Gaudi::TrackVector& refVector)
{
  m_refVector = refVector;                     // reference trajectory
  this->init( otTime, geom, ambiguity, true );
}

/// Standard constructor, without the reference vector
OTMeasurement::OTMeasurement( const LHCb::OTTime& otTime,
                              const DeOTDetector& geom,
                              int ambiguity )
{
  m_refVector = Gaudi::TrackVector(); 
  this->init( otTime, geom, ambiguity, false );
}

/// Copy constructor
OTMeasurement::OTMeasurement( const OTMeasurement& other )
  : Measurement(other) {
  m_time = other.m_time;
  m_ambiguity = other.m_ambiguity;
  m_stereoAngle = other.m_stereoAngle;
}

void OTMeasurement::init( const LHCb::OTTime& otTime,
                          const DeOTDetector& geom,
                          int ambiguity,
                          bool refIsSet ) 
{
  // Fill the data members
  m_mtype     = Measurement::OT;
  m_time      = &otTime;                       // pointer to hit
  m_ambiguity = ambiguity;                     // drift ambiguity
  m_refIsSet  = refIsSet;
  m_lhcbID    = LHCbID( m_time -> channel() ); // set the LHCbID

  // some constants...
  double driftVelocity = geom.driftDelay(); // ns/mm

  // Calculate the drift distance (includes the propagation along wire)
  m_measure      = m_time->calibratedTime() / driftVelocity;
  m_errMeasure   = geom.resolution() ;
  m_trajectory   = geom.trajectory( m_lhcbID );

  // Get the z of the measurement (centre of the wire)
  OTChannelID otChan = m_time->channel();
  DeOTModule* module = geom.findModule( otChan );
  XYZPoint wirePos = module->centerOfStraw( otChan.straw() );
  m_z = wirePos.z();

  // (almost) Obsolete
  m_stereoAngle = module->angle();

}
