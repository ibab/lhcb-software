// $Header: /afs/cern.ch/project/cvs/reps/lhcb/GaudiObjDesc/src/DaDiMethReturn.h,v 1.2 2001-11-09 08:37:11 mato Exp $
#ifndef DADIMETHRETURN_H 
#define DADIMETHRETURN_H 1

// Include files
#include "dom/DOMString.hpp"

/** @class DaDiMethReturn DaDiMethReturn.h
 *  
 *
 *  @author Stefan Roiser
 *  @date   14/06/2001
 */
class DaDiMethReturn {
public:
  /// Standard constructor
  DaDiMethReturn() {}; 

  virtual ~DaDiMethReturn() {}; ///< Standard destructor

  DOMString type();
  void setType(DOMString value);

  bool const_();
  void setConst_(bool value);
  
protected:

private:

  DOMString m_type; 
  bool      m_constant;
  
};

inline DOMString DaDiMethReturn::type() 
{
  return m_type;
}

inline void DaDiMethReturn::setType(DOMString value)
{
  m_type = value;
}

inline bool DaDiMethReturn::const_()
{
  return m_constant;
}

inline void DaDiMethReturn::setConst_(bool value)
{
  m_constant = value;
}

#endif // DADIMETHRETURN_H
