// $Id: DaDiMethReturn.h,v 1.4 2003-12-11 15:03:10 mato Exp $
#ifndef DADIMETHRETURN_H
#define DADIMETHRETURN_H 1

// Include files
#include "xercesc/util/XMLString.hpp"

/** @class DaDiMethReturn DaDiMethReturn.h
 *
 *
 *  @author Stefan Roiser
 *  @date   14/06/2001
 */
class DaDiMethReturn {
public:
  /// Standard constructor
  DaDiMethReturn() :
    m_type(0),
    m_constant(false) {};

  virtual ~DaDiMethReturn();

  const XMLCh* type();
  void setType(const XMLCh* value);

  bool const_();
  void setConst_(bool value);

protected:

private:

  XMLCh *m_type;
  bool   m_constant;

};

inline DaDiMethReturn::~DaDiMethReturn()
{
  delete [] m_type;
}

inline const XMLCh* DaDiMethReturn::type()
{
  return m_type;
}

inline void DaDiMethReturn::setType(const XMLCh* value)
{
  m_type = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_type, value);
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
