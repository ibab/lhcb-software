// $Id: DaDiMethod.h,v 1.8 2003-04-30 12:04:19 mato Exp $
#ifndef DADIMETHOD_H
#define DADIMETHOD_H 1

// Include files
#include "DaDiMethReturn.h"
#include "DaDiMethArgument.h"

#include "xercesc/util/XMLString.hpp"

#include <list>

/** @class DaDiMethod DaDiMethod.h
 *
 *
 *  @author Stefan Roiser
 *  @date   14/06/2001
 */
class DaDiMethod {
public:
  /// Standard constructor
  DaDiMethod() :
    m_name(0),
    m_desc(0),
    m_template(0),
    m_access(0),
    m_virtual(0),
    m_code(0),
    m_const(false),
    m_static(false),
    m_inline(false),
    m_friend(false),
    m_daDiMethReturn(0),
    m_daDiMethArgument(std::list<DaDiMethArgument*>()) {};

  virtual ~DaDiMethod();

  const XMLCh* name();
  void setName(const XMLCh* value);

  const XMLCh* desc();
  void setDesc(const XMLCh* value);

  const XMLCh* template_();
  void setTemplate(const XMLCh* value);

  const XMLCh* access();
  void setAccess(const XMLCh* value);

  //  const XMLCh* argList();
  //  void setArgList(const XMLCh* value);

  //  const XMLCh* argInOut();
  //  void setArgInOut(const XMLCh* value);

  const XMLCh* virtual_();
  void setVirtual_(const XMLCh* value);

  const XMLCh* code();
  void setCode(const XMLCh* value);

  bool const_();
  void setConst_(bool value);

  bool static_();
  void setStatic_(bool value);

  bool inline_();
  void setInline_(bool value);

  bool friend_();
  void setFriend_(bool value);

  DaDiMethReturn* daDiMethReturn();
  void setDaDiMethReturn(DaDiMethReturn* value);

  DaDiMethArgument* popDaDiMethArgument();
  void pushDaDiMethArgument(DaDiMethArgument* value);
  int sizeDaDiMethArgument();

protected:

private:

  XMLCh                        *m_name;
  XMLCh                        *m_desc;
  XMLCh                        *m_template;
  XMLCh                        *m_access;
  //XMLCh                        *m_argList;
  //XMLCh                        *m_argInOut;
  XMLCh                        *m_virtual;
  XMLCh                        *m_code;
  bool                          m_const;
  bool                          m_static;
  bool                          m_inline;
  bool                          m_friend;
  DaDiMethReturn*               m_daDiMethReturn;
  std::list<DaDiMethArgument*>  m_daDiMethArgument;

};

inline DaDiMethod::~DaDiMethod()
{
  xercesc::XMLString::release(&m_name);
  xercesc::XMLString::release(&m_desc);
  xercesc::XMLString::release(&m_template);
  xercesc::XMLString::release(&m_access);
  //  xercesc::XMLString::release(&m_argList);
  //  xercesc::XMLString::release(&m_argInOut);
  xercesc::XMLString::release(&m_virtual);
  xercesc::XMLString::release(&m_code);
  delete m_daDiMethReturn;
  std::list<DaDiMethArgument*>::iterator aIter;
  for (aIter = m_daDiMethArgument.begin(); 
       aIter != m_daDiMethArgument.end(); ++aIter)
  { delete *aIter; }
}

inline const XMLCh* DaDiMethod::name()
{
  return m_name;
}

inline void DaDiMethod::setName(const XMLCh* value)
{
  m_name = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_name, value);
}

inline const XMLCh* DaDiMethod::desc()
{
  return m_desc;
}

inline void DaDiMethod::setDesc(const XMLCh* value)
{
  m_desc = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_desc, value);
}

inline const XMLCh* DaDiMethod::template_()
{
  return m_template;
}

inline void DaDiMethod::setTemplate(const XMLCh* value)
{
  m_template = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_template, value);
}

inline const XMLCh* DaDiMethod::access()
{
  return m_access;
}

inline void DaDiMethod::setAccess(const XMLCh* value)
{
  m_access = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_access, value);
}

/*
  inline const XMLCh* DaDiMethod::argList()
  {
  return m_argList;
  }

  inline void DaDiMethod::setArgList(const XMLCh* value)
  {
  m_argList = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_argList, value);
  }

  inline const XMLCh* DaDiMethod::argInOut()
  {
  return m_argInOut;
  }

  inline void DaDiMethod::setArgInOut(const XMLCh* value)
  {
  m_argInOut = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_argInOut, value);
  }
*/

inline const XMLCh* DaDiMethod::virtual_()
{
  return m_virtual;
}

inline void DaDiMethod::setVirtual_(const XMLCh* value)
{
  m_virtual = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_virtual, value);
}

inline const XMLCh* DaDiMethod::code()
{
  return m_code;
}

inline void DaDiMethod::setCode(const XMLCh* value)
{
  m_code = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_code, value);
}

inline bool DaDiMethod::const_()
{
  return m_const;
}

inline void DaDiMethod::setConst_(bool value)
{
  m_const = value;
}

inline bool DaDiMethod::static_()
{
  return m_static;
}

inline void DaDiMethod::setStatic_(bool value)
{
  m_static = value;
}

inline bool DaDiMethod::inline_()
{
  return m_inline;
}

inline void DaDiMethod::setInline_(bool value)
{
  m_inline = value;
}

inline bool DaDiMethod::friend_()
{
  return m_friend;
}

inline void DaDiMethod::setFriend_(bool value)
{
  m_friend = value;
}

inline DaDiMethReturn* DaDiMethod::daDiMethReturn()
{
  return m_daDiMethReturn;
}

inline void DaDiMethod::setDaDiMethReturn(DaDiMethReturn* value)
{
  m_daDiMethReturn = value;
}

inline DaDiMethArgument* DaDiMethod::popDaDiMethArgument()
{
  DaDiMethArgument* pt = m_daDiMethArgument.front();
  m_daDiMethArgument.push_back(pt);
  m_daDiMethArgument.pop_front();
  return pt;
}

inline void DaDiMethod::pushDaDiMethArgument(DaDiMethArgument* value)
{
  m_daDiMethArgument.push_back(value);
}

inline int DaDiMethod::sizeDaDiMethArgument()
{
  return m_daDiMethArgument.size();
}

#endif // DADIMETHOD_H
