// $ID
#ifndef DADIDESTRUCTOR_H 
#define DADIDESTRUCTOR_H 1

// Include files
#include "DaDiMethReturn.h"
#include "DaDiMethArgument.h"

#include "dom/DOMString.hpp"

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
  DaDiDestructor() {}; 

  virtual ~DaDiDestructor() {}; ///< Standard destructor

  DOMString desc();
  void setDesc(DOMString value);

  DOMString argList();
  void setArgList(DOMString value);

  DOMString argInOut();
  void setArgInOut(DOMString value);

  DOMString code();
  void setCode(DOMString value);

  DaDiMethArgument* popDaDiMethArgument();
  void pushDaDiMethArgument(DaDiMethArgument* value);
  int sizeDaDiMethArgument();
  
protected:

private:

  DOMString                    m_desc, 
                               m_argList,
                               m_argInOut,
                               m_code;
  std::list<DaDiMethArgument*> m_daDiMethArgument;

};

inline DOMString DaDiDestructor::desc()
{
  return m_desc;
}

inline void DaDiDestructor::setDesc(DOMString value)
{
  m_desc = value;
}

inline DOMString DaDiDestructor::argList()
{
  return m_argList;
}

inline void DaDiDestructor::setArgList(DOMString value)
{
  m_argList = value;
}

inline DOMString DaDiDestructor::argInOut()
{
  return m_argInOut;
}

inline void DaDiDestructor::setArgInOut(DOMString value)
{
  m_argInOut = value;
}

inline DOMString DaDiDestructor::code()
{
  return m_code;
}

inline void DaDiDestructor::setCode(DOMString value)
{
  m_code = value;
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
