

//   **************************************************************************
//   *                                                                        *
//   *                      ! ! ! A T T E N T I O N ! ! !                     *
//   *                                                                        *
//   *  This file was created automatically by GaudiObjDesc, please do not    *
//   *  delete it or edit it by hand.                                         *
//   *                                                                        *
//   *  If you want to change this file, first change the corresponding       *
//   *  xml-file and rerun the tools from GaudiObjDesc (or run make if you    *
//   *  are using it from inside a Gaudi-package).                            *
//   *                                                                        *
//   **************************************************************************



#ifndef PhysEvent_PhysEvent_H
#define PhysEvent_PhysEvent_H 1

// Include files
#include "LHCbEvent/CLHEPStreams.h"
#include "LHCbEvent/Classification.h"
#include "GaudiKernel/DataObject.h"


// Class ID definition
  static const CLID& CLID_PhysEvent = 801;

/** @class PhysEvent PhysEvent.h 
 *
 *  Stores information of the physics event
 *
 *  @author Gloria Corti
 *  created Mon Feb  4 20:21:56 2002
 *
 */

class PhysEvent: public DataObject
{

public: 

  /// Default Constructor 
  PhysEvent()  {}

  /// Destructor 
  virtual ~PhysEvent() {}

  /// Retrieve pointer to class definition structure
  virtual const CLID& clID() const; 
  static const CLID& classID(); 

  /// Retrieve Event classification
  const Classification& classification() const; 

  /// Update Event classification
  void setClassification(const Classification& value);

  /// Serialize the object for writing
  virtual StreamBuffer& serialize(StreamBuffer& s) const;

  /// Serialize the object for reading
  virtual StreamBuffer& serialize(StreamBuffer& s);

  /// Fill the ASCII output stream
  virtual std::ostream& fillStream(std::ostream& s) const;


protected: 


private: 

  Classification m_classification; ///<             Event classification

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------


inline const CLID& PhysEvent::clID() const 
{
  return PhysEvent::classID();
}

inline const CLID& PhysEvent::classID()
{
  return CLID_PhysEvent;
}

inline const Classification& PhysEvent::classification() const 
{
  return m_classification;
}

inline void PhysEvent::setClassification(const Classification& value)
{
  m_classification = value; 
}

inline StreamBuffer& PhysEvent::serialize(StreamBuffer& s) const 
{
  s << m_classification;
  return s;
}

inline StreamBuffer& PhysEvent::serialize(StreamBuffer& s)
{
  s >> m_classification;
  return s;
}

inline std::ostream& PhysEvent::fillStream(std::ostream& s) const
{
  s << "{ "
    << " classification:\t" << m_classification << " } ";
  return s;
}


#endif   ///PhysEvent_PhysEvent_H
