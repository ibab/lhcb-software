// $Header: /afs/cern.ch/project/cvs/reps/lhcb/GaudiObjDesc/src/DaDiBaseClass.h,v 1.2 2001-11-09 08:37:10 mato Exp $
#ifndef DADIBASECLASS_H 
#define DADIBASECLASS_H 1

// Include files
#include "dom/DOMString.hpp"

/** @class DaDiBaseClass DaDiBaseClass.h
 *  
 *
 *  @author Stefan Roiser
 *  @date   14/06/2001
 */
class DaDiBaseClass {
public:
  /// Standard constructor
  DaDiBaseClass() {}; 

  virtual ~DaDiBaseClass() {}; ///< Standard destructor

  DOMString name();
  void setName(DOMString value);

  DOMString access();
  void setAccess(DOMString value);

  bool virtual_();
  void setVirtual_(bool value);
  
protected:

private:

  DOMString m_name, 
			      m_access;
  bool      m_virtual;
  
};

inline DOMString DaDiBaseClass::name() 
{
  return m_name;
}

inline void DaDiBaseClass::setName(DOMString value)
{
  m_name = value;
}

inline DOMString DaDiBaseClass::access()
{
  return m_access;
}

inline void DaDiBaseClass::setAccess(DOMString value)
{
  m_access = value;
}

inline bool DaDiBaseClass::virtual_()
{
  return m_virtual;
}

inline void DaDiBaseClass::setVirtual_(bool value)
{
  m_virtual = value;
}

#endif // DADIBASECLASS_H
