// $Id: DaDiDestructor.h,v 1.4 2003-12-11 15:03:09 mato Exp $
#ifndef DADIDESTRUCTOR_H
#define DADIDESTRUCTOR_H 1

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
class DaDiDestructor {
public:
  /// Standard constructor
  DaDiDestructor() :
    m_desc(0),
    m_code(0),
    m_daDiMethArgument(std::list<DaDiMethArgument*>()) {};

  virtual ~DaDiDestructor();

  const XMLCh* desc();
  void setDesc(const XMLCh* value);

  //  const XMLCh* argList();
  //  void setArgList(const XMLCh* value);

  //  const XMLCh* argInOut();
  //  void setArgInOut(const XMLCh* value);

  const XMLCh* code();
  void setCode(const XMLCh* value);

  DaDiMethArgument* popDaDiMethArgument();
  void pushDaDiMethArgument(DaDiMethArgument* value);
  int sizeDaDiMethArgument();

protected:

private:

  XMLCh                        *m_desc;
  //XMLCh                        *m_argList;
  //XMLCh                        *m_argInOut;
  XMLCh                        *m_code;
  std::list<DaDiMethArgument*>  m_daDiMethArgument;

};

inline DaDiDestructor::~DaDiDestructor()
{
  delete [] m_desc;
  //  delete [] m_argList;
  //  delete [] m_argInOut;
  delete [] m_code;
  std::list<DaDiMethArgument*>::iterator aIter;
  for (aIter = m_daDiMethArgument.begin();
       aIter != m_daDiMethArgument.end();
       ++aIter)
  { delete *aIter; }

}

inline const XMLCh* DaDiDestructor::desc()
{
  return m_desc;
}

inline void DaDiDestructor::setDesc(const XMLCh* value)
{
  m_desc = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_desc, value);
}

/*
  inline const XMLCh* DaDiDestructor::argList()
  {
  return m_argList;
  }

  inline void DaDiDestructor::setArgList(const XMLCh* value)
  {
  m_argList = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_argList, value);
  }

  inline const XMLCh* DaDiDestructor::argInOut()
  {
  return m_argInOut;
  }

  inline void DaDiDestructor::setArgInOut(const XMLCh* value)
  {
  m_argInOut = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_argInOut, value);
  }
*/

inline const XMLCh* DaDiDestructor::code()
{
  return m_code;
}

inline void DaDiDestructor::setCode(const XMLCh* value)
{
  m_code = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_code, value);
}

inline DaDiMethArgument* DaDiDestructor::popDaDiMethArgument()
{
  DaDiMethArgument* pt = m_daDiMethArgument.front();
  m_daDiMethArgument.push_back(pt);
  m_daDiMethArgument.pop_front();
  return pt;
}

inline void DaDiDestructor::pushDaDiMethArgument(DaDiMethArgument* value)
{
  m_daDiMethArgument.push_back(value);
}

inline int DaDiDestructor::sizeDaDiMethArgument()
{
  return m_daDiMethArgument.size();
}

#endif // DADIDESTRUCTOR_H
