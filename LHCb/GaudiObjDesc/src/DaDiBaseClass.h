// $Id: DaDiBaseClass.h,v 1.4 2003-12-11 15:03:09 mato Exp $
#ifndef DADIBASECLASS_H
#define DADIBASECLASS_H 1

// Include files
#include "xercesc/util/XMLString.hpp"

/** @class DaDiBaseClass DaDiBaseClass.h
 *
 *
 *  @author Stefan Roiser
 *  @date   14/06/2001
 */
class DaDiBaseClass {
public:
  /// Standard constructor
  DaDiBaseClass() :
    m_name(0),
    m_access(0),
    m_virtual(false) {};

  virtual ~DaDiBaseClass();

  const XMLCh* name();
  void setName(const XMLCh* value);

  const XMLCh* access();
  void setAccess(const XMLCh* value);

  bool virtual_();
  void setVirtual_(bool value);

protected:

private:

  XMLCh *m_name;
  XMLCh *m_access;
  bool   m_virtual;

};

inline DaDiBaseClass::~DaDiBaseClass()
{
  delete [] m_name;
  delete [] m_access;
}

inline const XMLCh* DaDiBaseClass::name()
{
  return m_name;
}

inline void DaDiBaseClass::setName(const XMLCh* value)
{
  m_name = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_name, value);
}

inline const XMLCh* DaDiBaseClass::access()
{
  return m_access;
}

inline void DaDiBaseClass::setAccess(const XMLCh* value)
{
  m_access = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_access, value);
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
