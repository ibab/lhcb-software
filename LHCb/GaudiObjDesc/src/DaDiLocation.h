// $Header: /afs/cern.ch/project/cvs/reps/lhcb/GaudiObjDesc/src/DaDiLocation.h,v 1.1 2002-01-30 20:29:05 mato Exp $
#ifndef DADILOCATION_H 
#define DADILOCATION_H 1

// Include files
#include "dom/DOMString.hpp"

/** @class DaDiAttribute DaDiAttribute.h
 *  
 *
 *  @author Stefan Roiser
 *  @date   14/06/2001
 */
class DaDiLocation {
public:
  /// Standard constructor
  DaDiLocation() {}; 

  virtual ~DaDiLocation() {}; ///< Standard destructor

  DOMString name();
  void setName(DOMString value);
    
  DOMString place();
  void setPlace(DOMString value);
  
protected:

private:

  DOMString m_name, 
            m_place;
};


inline DOMString DaDiLocation::name()
{
  return m_name;
}

inline void DaDiLocation::setName(DOMString value)
{
  m_name = value;
}

inline DOMString DaDiLocation::place()
{
  return m_place;
}

inline void DaDiLocation::setPlace(DOMString value)
{
  m_place = value;
}


#endif // DADILOCATION_H
