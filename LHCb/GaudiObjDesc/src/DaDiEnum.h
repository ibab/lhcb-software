// $Id: DaDiEnum.h,v 1.3 2003-04-30 12:04:17 mato Exp $
#ifndef DADIENUM_H 
#define DADIENUM_H 1

// Include files
#include "xercesc/util/XMLString.hpp"

/** @class DaDiAttribute DaDiAttribute.h
 *  
 *
 *  @author Stefan Roiser
 *  @date   14/06/2001
 */
class DaDiEnum {
public:
  /// Standard constructor
  DaDiEnum() :
  m_name(0),
  m_desc(0),
  m_value(0),
  m_access(0) {}; 

  virtual ~DaDiEnum();

  const XMLCh* name();
  void setName(const XMLCh* value);
    
  const XMLCh* desc();
  void setDesc(const XMLCh* value);
  
  const XMLCh* value();
  void setValue(const XMLCh* value);

  const XMLCh* access();
  void setAccess(const XMLCh* value);

protected:

private:

  XMLCh *m_name; 
  XMLCh *m_desc; 
  XMLCh *m_value; 
  XMLCh *m_access;
};

inline DaDiEnum::~DaDiEnum()
{
  xercesc::XMLString::release(&m_name);
  xercesc::XMLString::release(&m_desc);
  xercesc::XMLString::release(&m_value);
  xercesc::XMLString::release(&m_access);
}

inline const XMLCh* DaDiEnum::name()
{
  return m_name;
}

inline void DaDiEnum::setName(const XMLCh* value)
{
  m_name = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_name, value);
}

inline const XMLCh* DaDiEnum::desc()
{
  return m_desc;
}

inline void DaDiEnum::setDesc(const XMLCh* value)
{
  m_desc = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_desc, value);
}

inline const XMLCh* DaDiEnum::value()
{
  return m_value;
}

inline void DaDiEnum::setValue(const XMLCh* value)
{
  m_value = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_value, value);
}

inline const XMLCh* DaDiEnum::access()
{
  return m_access;
}

inline void DaDiEnum::setAccess(const XMLCh* value)
{
  m_access = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_access, value);
}

#endif // DADIENUM_H
