
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

#ifndef Event_EventHeader_H
#define Event_EventHeader_H 1

// Include files
#include "GaudiKernel/TimePoint.h"
#include "GaudiKernel/DataObject.h"
#include <ostream>


// Forward declarations

// Class ID definition
static const CLID& CLID_EventHeader = 101;

// Namespace for locations in TDS
namespace EventHeaderLocation {
  static const std::string& Default = "Header";
}


/** @class EventHeader EventHeader.h
 *
 * Global event header, created when event is first created 
 *
 * @author Marco Cattaneo
 * created Wed Aug 17 09:58:33 2005
 * 
 */

class EventHeader: public DataObject
{
public:

  friend class EventHeader_dict;

  /// Default Constructor
  EventHeader() : m_evtNum(0),
                  m_runNum(0),
                  m_evtTime(0) {}

  /// Default Destructor
  virtual ~EventHeader() {}

  // Retrieve pointer to class definition structure
  virtual const CLID& clID() const;
  static const CLID& classID();

  /// Operator overloading for stringoutput
  friend std::ostream& operator<< (std::ostream& str,
                                   const EventHeader& obj);

  /// Fill the ASCII output stream
  virtual std::ostream& fillStream(std::ostream& s) const;

  /// Retrieve const  Event number
  long evtNum() const;

  /// Update  Event number
  void setEvtNum(long value);

  /// Retrieve const  Run number
  long runNum() const;

  /// Update  Run number
  void setRunNum(long value);

protected:

private:

  long      m_evtNum;  ///< Event number
  long      m_runNum;  ///< Run number
  TimePoint m_evtTime; ///< Reserved for time when event was created: DAQ or event generator

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

// Including forward declarations

inline const CLID& EventHeader::clID() const
{
  return EventHeader::classID();
}

inline const CLID& EventHeader::classID()
{
  return CLID_EventHeader;
}

inline std::ostream& operator<< (std::ostream& str,
                                 const EventHeader& obj)
{
  return obj.fillStream(str);
}

inline std::ostream& EventHeader::fillStream(std::ostream& s) const
{
  s << "{ " << "evtNum :	" << m_evtNum << std::endl
            << "runNum :	" << m_runNum << std::endl
            << "evtTime :	" << m_evtTime << std::endl << " }";
  return s;
}


inline long EventHeader::evtNum() const 
{
  return m_evtNum;
}

inline void EventHeader::setEvtNum(long value) 
{
  m_evtNum = value;
}

inline long EventHeader::runNum() const 
{
  return m_runNum;
}

inline void EventHeader::setRunNum(long value) 
{
  m_runNum = value;
}


#endif ///Event_EventHeader_H
