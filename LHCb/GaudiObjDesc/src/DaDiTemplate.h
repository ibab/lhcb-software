// $Id: DaDiTemplate.h,v 1.1 2003-12-17 17:31:19 mato Exp $
#ifndef DADITEMPLATE_H 
#define DADITEMPLATE_H 1

// Include files
#include "xercesc/util/XMLString.hpp"

/** @class DaDiTemplate DaDiTemplate.h
 *  
 *
 *  @author Stefan Roiser
 *  @date   25/11/2003
 */
class DaDiTemplate {
public:
  /// Standard constructor
  DaDiTemplate() :
  m_name(0),
  m_t1(0),
  m_t2(0),
  m_t3(0),
  m_t4(0) {}; 

  virtual ~DaDiTemplate();

  const XMLCh* name();
  void setName(const XMLCh* value);
    
  const XMLCh* t1();
  void setT1(const XMLCh* value);
  
  const XMLCh* t2();
  void setT2(const XMLCh* value);
  
  const XMLCh* t3();
  void setT3(const XMLCh* value);
  
  const XMLCh* t4();
  void setT4(const XMLCh* value);
  
protected:

private:

  XMLCh *m_name; 
  XMLCh *m_t1; 
  XMLCh *m_t2; 
  XMLCh *m_t3;
  XMLCh *m_t4;
};

inline DaDiTemplate::~DaDiTemplate()
{
  xercesc::XMLString::release(&m_name);
  xercesc::XMLString::release(&m_t1);
  xercesc::XMLString::release(&m_t2);
  xercesc::XMLString::release(&m_t3);
  xercesc::XMLString::release(&m_t4);
}

inline const XMLCh* DaDiTemplate::name()
{
  return m_name;
}

inline void DaDiTemplate::setName(const XMLCh* value)
{
  m_name = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_name, value);
}

inline const XMLCh* DaDiTemplate::t1()
{
  return m_t1;
}

inline void DaDiTemplate::setT1(const XMLCh* value)
{
  m_t1 = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_t1, value);
}

inline const XMLCh* DaDiTemplate::t2()
{
  return m_t2;
}

inline void DaDiTemplate::setT2(const XMLCh* value)
{
  m_t2 = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_t2, value);
}

inline const XMLCh* DaDiTemplate::t3()
{
  return m_t3;
}

inline void DaDiTemplate::setT3(const XMLCh* value)
{
  m_t3 = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_t3, value);
}

inline const XMLCh* DaDiTemplate::t4()
{
  return m_t4;
}

inline void DaDiTemplate::setT4(const XMLCh* value)
{
  m_t4 = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_t4, value);
}

#endif // DADITEMPLATE_H
