// $Id: DaDiConstructor.h,v 1.5 2003-12-11 15:03:09 mato Exp $
#ifndef DADICONSTRUCTOR_H
#define DADICONSTRUCTOR_H 1

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
class DaDiConstructor {
public:
  /// Standard constructor
  DaDiConstructor() :
    m_desc(0),
    m_initList(0),
    m_code(0),
    m_daDiMethArgument(std::list<DaDiMethArgument*>()) {};

  virtual ~DaDiConstructor();

  const XMLCh* desc();
  void setDesc(const XMLCh* value);

  //  const XMLCh* argList();
  //  void setArgList(const XMLCh* value);

  //  const XMLCh* argInOut();
  //  void setArgInOut(const XMLCh* value);

  const XMLCh* initList();
  void setInitList(const XMLCh* value);

  const XMLCh* code();
  void setCode(const XMLCh* value);

  DaDiMethArgument* popDaDiMethArgument();
  void pushDaDiMethArgument(DaDiMethArgument* value);
  int sizeDaDiMethArgument();

protected:

private:

  XMLCh                        *m_desc;
  //XMLCh                         *m_argList;
  //XMLCh                         *m_argInOut;
  XMLCh                        *m_initList;
  XMLCh                        *m_code;
  std::list<DaDiMethArgument*>  m_daDiMethArgument;

};

inline DaDiConstructor::~DaDiConstructor()
{
  delete [] m_desc;
  //  delete [] m_argList;
  //  delete [] m_argInOut;
  delete [] m_initList;
  delete [] m_code;
  std::list<DaDiMethArgument*>::iterator aIter;
  for (aIter = m_daDiMethArgument.begin(); aIter != m_daDiMethArgument.end();
       ++aIter)
  { delete *aIter; }
}

inline const XMLCh* DaDiConstructor::desc()
{
  return m_desc;
}

inline void DaDiConstructor::setDesc(const XMLCh* value)
{
  m_desc = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_desc, value);
}

/*
  inline const XMLCh* DaDiConstructor::argList()
  {
  return m_argList;
  }

  inline void DaDiConstructor::setArgList(const XMLCh* value)
  {
  m_argList = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_argList, value);
  }

  inline const XMLCh* DaDiConstructor::argInOut()
  {
  return m_argInOut;
  }

  inline void DaDiConstructor::setArgInOut(const XMLCh* value)
  {
  m_argInOut = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_argInOut, value);
  }
*/

inline const XMLCh* DaDiConstructor::initList()
{
  return m_initList;
}

inline void DaDiConstructor::setInitList(const XMLCh* value)
{
  m_initList = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_initList, value);
}

inline const XMLCh* DaDiConstructor::code()
{
  return m_code;
}

inline void DaDiConstructor::setCode(const XMLCh* value)
{
  m_code = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_code, value);
}

inline DaDiMethArgument* DaDiConstructor::popDaDiMethArgument()
{
  DaDiMethArgument* pt = m_daDiMethArgument.front();
  m_daDiMethArgument.push_back(pt);
  m_daDiMethArgument.pop_front();
  return pt;
}

inline void DaDiConstructor::pushDaDiMethArgument(DaDiMethArgument* value)
{
  m_daDiMethArgument.push_back(value);
}

inline int DaDiConstructor::sizeDaDiMethArgument()
{
  return m_daDiMethArgument.size();
}

#endif // DADICONSTRUCTOR_H
