// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Event/VeloEvent/VeloEvent/VeloCluster.h,v 1.3 2001-07-10 07:47:58 frankm Exp $
#ifndef LHCBEVENT_VELOCLUSTER_H
#define LHCBEVENT_VELOCLUSTER_H 1

// Include files
#include <iostream>
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/SmartRefVector.h"
#include "LHCbEvent/Definitions.h"

class RawVeloHit;

// Externals 
static const CLID& CLID_VeloCluster = 611;


//------------------------------------------------------------------------------
//
// ClassName:   VeloCluster
//  
// Description: This class represents a simple data object
//              containing a VELO r/phi cluster representation
//
// Author:      Bruce Hay
//
// Changes:     
//
//------------------------------------------------------------------------------
class VeloCluster : virtual public ContainedObject {

 public:
  /// Constructors
  VeloCluster()
    : m_station(0),
      m_coordinate(0.0),
      m_detector(0),
      m_sector(0),
      m_centerWeight(0.0),
      m_centerGeom(0.0),
      m_pulseHeight(0.0),
      m_width(0.0),
      m_zPosition(0.0) 	                          { }

  VeloCluster( unsigned char station,
               double coordinate,
               unsigned char detector,
               unsigned char sector,
               double centerWeight,
               double centerGeom,
               double pulseHeight,
               double width,
               double zPosition )
    : m_station(station),
      m_coordinate(coordinate),
      m_detector(detector),
      m_sector(sector),
      m_centerWeight(centerWeight),
      m_centerGeom(centerGeom),
      m_pulseHeight(pulseHeight),
      m_width(width),
      m_zPosition(zPosition)                      { }

  /// Copy constructor not implemented
  VeloCluster( const VeloCluster& /*copy */ )     { }

  /// Destructor
  virtual ~VeloCluster()                          { }

  // Retrieve pointer to class definition structure for a persistent store
  virtual const CLID& clID() const                { return VeloCluster::classID(); }
  static const CLID& classID()                    { return CLID_VeloCluster; }

  /// Retrieve station number
  unsigned char station() const                   { return m_station; }
  /// Update station number
  void setStation( unsigned char new_station )    { m_station = new_station; }

  /// Retrieve coordinate
  double coordinate() const                       { return m_coordinate; }
  /// Update coordinate
  void setCoordinate( double new_coordinate )     { m_coordinate = new_coordinate; }

  /// Retrieve detector number (Phi sector 1-6)
  unsigned char detector() const                  { return m_detector; }
  /// Update detector number
  void setDetector( unsigned char new_detector )  { m_detector = new_detector; }

  /// Retrieve sector number 
  // R-clusters:   (inner-lesser phi, inner-greater phi, middle, outer = 1,2,3,4 )
  // Phi-clusters: (inner, outer = 1,2 )
  unsigned char sector() const                    { return m_sector; }
  /// Update sector number
  void setSector( unsigned char new_sector )      { m_sector = new_sector; }

  /// Retrieve weighted center of cluster
  double centerWeight() const                     { return m_centerWeight; }
  /// Update weighted center of cluster
  void setCenterWeight( double new_centerWeight ) { m_centerWeight = new_centerWeight; }

  /// Retrieve geometry center of cluster
  double centerGeom() const                       { return m_centerGeom; }
  /// Update geometry center of cluster
  void setCenterGeom( double new_centerGeom )     { m_centerGeom = new_centerGeom; }

  /// Retrieve cluster pulse height
  double pulseHeight() const                      { return m_pulseHeight;}
  /// Update cluster pulse height
  void setPulseHeight( double new_pulseHeight )   { m_pulseHeight = new_pulseHeight; }

  /// Retrieve width of cluster
  double width() const                            { return m_width;}
  /// Update width of cluster
  void setWidth( double new_width )               { m_width = new_width; }

  /// Retrieve Z coordinate
  double zPosition() const                        { return m_zPosition; }
  /// Update Z coordinate
  void setZPosition( double new_zPosition )       { m_zPosition = new_zPosition; }

  /// Access to raw hit references
  SmartRefVector<RawVeloHit>& hits();
  /// Access to raw hit references
  const SmartRefVector<RawVeloHit>& hits() const;

  /// Serialize the object for writing
  virtual StreamBuffer& serialize( StreamBuffer& s ) const ;
  /// Serialize the object for reading
  virtual StreamBuffer& serialize( StreamBuffer& s );
	/// Fill the ASCII output stream
	virtual std::ostream& fillStream( std::ostream& s ) const;

private:
  /// Station number
  unsigned char  m_station;
  /// coordinate of cluster
  double     m_coordinate;
  /// Detector number (Phi sector 1-6)
  unsigned char  m_detector;
  /// Sector number
  // R-clusters:   (inner-lesser phi, inner-greater phi, middle, outer = 1,2,3,4 )
  // Phi-clusters: (inner, outer = 1,2 )
  unsigned char  m_sector;
  /// Weighted center of cluster
  double     m_centerWeight;
  /// Geometric center of cluster
  double     m_centerGeom;
  /// cluster pulse height
  double     m_pulseHeight;
  /// cluster width
  double     m_width;
  /// Z position
  double     m_zPosition;
  /// References to raw hits
  SmartRefVector<RawVeloHit>  m_hits;
};

//
// Inline code must be outside the class definition
//
#include "VeloEvent/RawVeloHit.h"

/// Access to raw hit references
inline SmartRefVector<RawVeloHit>& VeloCluster::hits()       { 
  return m_hits;  
}

/// Access to raw hit references
inline const SmartRefVector<RawVeloHit>& VeloCluster::hits() const { 
  return m_hits;  
}

/// Serialize the object for writing
inline StreamBuffer& VeloCluster::serialize( StreamBuffer& s ) const       {
  ContainedObject::serialize(s)
    << m_station
    << m_coordinate
    << m_detector
    << m_sector
    << m_centerWeight
    << m_centerGeom
    << m_pulseHeight
    << m_width
    << m_zPosition
    << m_hits(this);
  return s;
}

/// Serialize the object for reading
inline StreamBuffer& VeloCluster::serialize( StreamBuffer& s )             {
  ContainedObject::serialize(s)
    >> m_station
    >> m_coordinate
    >> m_detector
    >> m_sector
    >> m_centerWeight
    >> m_centerGeom
    >> m_pulseHeight
    >> m_width
    >> m_zPosition
    >> m_hits(this);
  return s;
}

/// Fill the ASCII output stream
inline std::ostream& VeloCluster::fillStream ( std::ostream & s ) const    {
  return s
    << "class VeloCluster :"
    << "\n    Station number          = "
    << int(m_station)
    << "\n    coordinate              = "
    << LHCbEventFloatFormat( LHCbEvent::width, LHCbEvent::precision )
    << m_coordinate
    << "\n    Detector number         = "
    << int(m_detector)
    << "\n    Sector number           = "
    << int(m_sector)
    << "\n    Weighted center         = "
    << LHCbEventFloatFormat( LHCbEvent::width, LHCbEvent::precision )
    << m_centerWeight
    << "\n    Geometric center        = "
    << LHCbEventFloatFormat( LHCbEvent::width, LHCbEvent::precision )
    << m_centerGeom
    << "\n    cluster pulse height    = "
    << LHCbEventFloatFormat( LHCbEvent::width, LHCbEvent::precision )
    << m_pulseHeight
    << "\n    cluster width           = "
    << LHCbEventFloatFormat( LHCbEvent::width, LHCbEvent::precision )
    << m_width
    << "\n    Z position              = "
    << LHCbEventFloatFormat( LHCbEvent::width, LHCbEvent::precision )
    << m_zPosition;
}
#endif // LHCBEVENT_VELOCLUSTER_H
