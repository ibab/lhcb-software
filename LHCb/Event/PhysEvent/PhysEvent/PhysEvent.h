// $Id: PhysEvent.h,v 1.1.1.1 2001-07-09 09:23:58 gcorti Exp $
#ifndef PHYSEVENT_PHYSEVENT_H
#define PHYSEVENT_PHYSEVENT_H 1


// Include files
#include <iostream>
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/StreamBuffer.h"
#include "LHCbEvent/Classification.h"
#include "LHCbEvent/Definitions.h"

// ClassID
static const CLID& CLID_PhysEvent = 801;


/** @class PhysEvent PhysEvent.h PhysEvent/PhysEvent.h
 *
 *  Stores information of the physics event
 *  It can be identified by "/Event/Phys"
 *
 * @version 1
 * @author  G.Corti
 * @date    19/05/2001 
 */

class PhysEvent : public DataObject {

public:
  /// Constructors
  PhysEvent(const char* name = "PhysEvent")
    : DataObject(name) { }
  /// Destructor
  virtual ~PhysEvent() { }

  /// Retrieve reference to class definition structure
  virtual const CLID& clID() const { return PhysEvent::classID(); }
  static const CLID& classID() { return CLID_PhysEvent; }

  /// Retrieve classificiation
  const Classification& classification() const {
    return m_classification;
  }
  /// Update classification
  void setClassification (const Classification& value) {
    m_classification = value;
  }

  /// Serialize the object for writing
  virtual StreamBuffer& serialize( StreamBuffer& s ) const;
  /// Serialize the object for reading
  virtual StreamBuffer& serialize( StreamBuffer& s );

  /// Output operator (ASCII)
  friend std::ostream& operator<< ( std::ostream& s, const PhysEvent& obj )    {
    return obj.fillStream(s);
  }
  /// Fill the output stream (ASCII)
  virtual std::ostream& fillStream( std::ostream& s ) const;

private:
  /// Event classification
  Classification    m_classification;

};


//
// Inline code must be outside the class definition
//


/// Serialize the object for writing
inline StreamBuffer& PhysEvent::serialize( StreamBuffer& s ) const             {
  DataObject::serialize(s);
  return s << m_classification;
}


/// Serialize the object for reading
inline StreamBuffer& PhysEvent::serialize( StreamBuffer& s )                   {
  DataObject::serialize(s);
  return s >> m_classification;
}


/// Fill the output stream (ASCII)
inline std::ostream& PhysEvent::fillStream( std::ostream& s ) const            {
  return s
    << "class PhysEvent :"
    << "\n    Event classification = "
    << LHCbEventField( LHCbEvent::field12 )
    << m_classification;
}


#endif      // PHYSEVENT_PHYSEVENT_H
