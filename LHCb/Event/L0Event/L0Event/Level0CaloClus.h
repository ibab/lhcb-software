#ifndef LHCBEVENT_LEVEL0CALOCLUS_H
#define LHCBEVENT_LEVEL0CALOCLUS_H 1

// Include files
#include <iostream>
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/SmartRefVector.h"
#include "CLHEP/Geometry/Point3D.h"

// Externals 
extern const CLID& CLID_Level0CaloClus;

//------------------------------------------------------------------------------
//
// ClassName:   Level0CaloClus
//
// Description: This class represents the calorimeter cluster candidates for L0
//
// Author:      Bruce Hay
//
//------------------------------------------------------------------------------
class Level0CaloClus : public ContainedObject {

public:
  /// Constructor
  Level0CaloClus()                                 { 
  }
  /// Destructor
  virtual ~Level0CaloClus()                        {
  }
  /// Retrieve pointer to class definition structure
  virtual const CLID& clID() const                 {
    return Level0CaloClus::classID();
  }
  static const CLID& classID()                     {
    return CLID_Level0CaloClus;
  }

  /// Transverse energy
  double et() const                                { return m_et; }
  /// Update transverse energy
  void setEt(double et)                            { m_et = et; }

  /// Position
  const HepPoint3D& position() const               { return m_position; }
  /// Update position
  void setPosition(HepPoint3D& position)           { m_position = position; }
  void setPosition(double x, double y, double z)   { 
    m_position.setX(x);
    m_position.setY(y);
    m_position.setZ(z);
  }

  /// Tolerance on x/y position = 1/2 cell size
  double posTol() const                            { return m_posTol; }
  /// Update tolerance on x/y position = 1/2 cell size
  void setPosTol(double posTol)                    { m_posTol = posTol; }

  /// Serialize the object for writing
  virtual StreamBuffer& serialize( StreamBuffer& s ) const ;
  /// Serialize the object for reading
  virtual StreamBuffer& serialize( StreamBuffer& s );

private:
  /// Transverse energy
  double      m_et;
  /// Position
  HepPoint3D  m_position;
  /// Tolerance on x/y position = 1/2 cell size
  double      m_posTol;

};

#include "LHCbEvent/CLHEPStreams.h"
//
// Inline code must be outside the class definition
//

/// Serialize the object for reading
inline StreamBuffer& Level0CaloClus::serialize(StreamBuffer& s) {
  ContainedObject::serialize(s) >> m_et >> m_position >> m_posTol;
  return s;
}

/// Serialize the object for writing
inline StreamBuffer& Level0CaloClus::serialize(StreamBuffer& s) const {
  ContainedObject::serialize(s) << m_et << m_position << m_posTol;
  return s;
}

#endif // LHCBEVENT_LEVEL0CALOCLUS_H
