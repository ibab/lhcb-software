// $Header: /afs/cern.ch/project/cvs/reps/lhcb/GaudiObjDesc/src/DaDiTypeDef.h,v 1.1 2002-02-04 10:07:00 mato Exp $
#ifndef DADITYPEDEF_H 
#define DADITYPEDEF_H 1

// Include files
#include "dom/DOMString.hpp"

/** @class DaDiAttribute DaDiAttribute.h
 *  
 *
 *  @author Stefan Roiser
 *  @date   14/06/2001
 */

class DaDiTypeDef {
public:
  /// Standard constructor
  DaDiTypeDef() {}; 

  virtual ~DaDiTypeDef() {}; ///< Standard destructor

  DOMString desc();
  void setDesc(DOMString value);
    
  DOMString type();
  void setType(DOMString value);
  
  DOMString def();
  void setDef(DOMString value);

  DOMString access();
  void setAccess(DOMString value);
  
protected:

private:

  DOMString m_desc, 
            m_type, 
            m_def, 
            m_access;

};


inline DOMString DaDiTypeDef::desc()
{
  return m_desc;
}

inline void DaDiTypeDef::setDesc(DOMString value)
{
  m_desc = value;
}

inline DOMString DaDiTypeDef::type()
{
  return m_type;
}

inline void DaDiTypeDef::setType(DOMString value)
{
  m_type = value;
}

inline DOMString DaDiTypeDef::def()
{
  return m_def;
}

inline void DaDiTypeDef::setDef(DOMString value)
{
  m_def = value;
}

inline DOMString DaDiTypeDef::access()
{
  return m_access;
}

inline void DaDiTypeDef::setAccess(DOMString value)
{
  m_access = value;
}

#endif // DADITYPEDEF_H
