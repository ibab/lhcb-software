// $Id: DaDiTypeDef.h,v 1.2 2003-04-30 12:04:19 mato Exp $
#ifndef DADITYPEDEF_H
#define DADITYPEDEF_H 1

// Include files
#include "xercesc/util/XMLString.hpp"

/** @class DaDiAttribute DaDiAttribute.h
 *
 *
 *  @author Stefan Roiser
 *  @date   14/06/2001
 */

class DaDiTypeDef {
public:
  /// Standard constructor
  DaDiTypeDef() :
    m_desc(0),
    m_type(0),
    m_def(0),
    m_access(0) {};

  virtual ~DaDiTypeDef();

  const XMLCh* desc();
  void setDesc(const XMLCh* value);

  const XMLCh* type();
  void setType(const XMLCh* value);

  const XMLCh* def();
  void setDef(const XMLCh* value);

  const XMLCh* access();
  void setAccess(const XMLCh* value);

protected:

private:

  XMLCh *m_desc;
  XMLCh *m_type;
  XMLCh *m_def;
  XMLCh *m_access;

};

inline DaDiTypeDef::~DaDiTypeDef()
{
  xercesc::XMLString::release(&m_desc);
  xercesc::XMLString::release(&m_type);
  xercesc::XMLString::release(&m_def);
  xercesc::XMLString::release(&m_access);
}

inline const XMLCh* DaDiTypeDef::desc()
{
  return m_desc;
}

inline void DaDiTypeDef::setDesc(const XMLCh* value)
{
  m_desc = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_desc, value);
}

inline const XMLCh* DaDiTypeDef::type()
{
  return m_type;
}

inline void DaDiTypeDef::setType(const XMLCh* value)
{
  m_type = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_type, value);
}

inline const XMLCh* DaDiTypeDef::def()
{
  return m_def;
}

inline void DaDiTypeDef::setDef(const XMLCh* value)
{
  m_def = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_def, value);
}

inline const XMLCh* DaDiTypeDef::access()
{
  return m_access;
}

inline void DaDiTypeDef::setAccess(const XMLCh* value)
{
  m_access = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_access, value);
}

#endif // DADITYPEDEF_H
