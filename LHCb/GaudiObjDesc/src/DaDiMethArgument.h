// $Header: /afs/cern.ch/project/cvs/reps/lhcb/GaudiObjDesc/src/DaDiMethArgument.h,v 1.3 2001-10-19 17:28:31 mato Exp $
#ifndef DADIMETHARGUMENT_H 
#define DADIMETHARGUMENT_H 1

// Include files
#include "dom/DOMString.hpp"

/** @class DaDiMethArgument DaDiMethArgument.h
 *  
 *
 *  @author Stefan Roiser
 *  @date   14/06/2001
 */
class DaDiMethArgument {
public:
  /// Standard constructor
  DaDiMethArgument() {}; 

  virtual ~DaDiMethArgument() {}; ///< Standard destructor

  DOMString type();
  void setType(DOMString value);
  
  DOMString name();
  void setName(DOMString value);
  
  DOMString const_();
  void setConst_(DOMString value);
  
  DOMString inout();
  void setInout(DOMString value);

  bool isPointer();
  void setIsPointer(bool value);

protected:

private:

  DOMString m_type, 
            m_name,
            m_constant, 
            m_inout;
  bool      m_isPointer;
  
};

inline DOMString DaDiMethArgument::type()
{
  return m_type;
}

inline void DaDiMethArgument::setType(DOMString value)
{
  m_type = value;
}

inline DOMString DaDiMethArgument::const_()
{
  return m_constant;
}

inline void DaDiMethArgument::setConst_(DOMString value)
{
  m_constant = value;
}

inline DOMString DaDiMethArgument::name()
{
  return m_name;
}

inline void DaDiMethArgument::setName(DOMString value)
{
  m_name = value;
}

inline DOMString DaDiMethArgument::inout()
{
  return m_inout;
}

inline void DaDiMethArgument::setInout(DOMString value)
{
  m_inout = value;
}

inline bool DaDiMethArgument::isPointer()
{
  return m_isPointer;
}

inline void DaDiMethArgument::setIsPointer(bool value)
{
  m_isPointer = value;
}

#endif // DADIMETHARGUMENT_H
