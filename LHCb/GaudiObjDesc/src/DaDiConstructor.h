// $ID
#ifndef DADICONSTRUCTOR_H 
#define DADICONSTRUCTOR_H 1

// Include files
#include <list>

#include "dom/DOMString.hpp"

#include "DaDiMethReturn.h"
#include "DaDiMethArgument.h"

/** @class DaDiMethod DaDiMethod.h
 *  
 *
 *  @author Stefan Roiser
 *  @date   14/06/2001
 */
class DaDiConstructor {
public:
  /// Standard constructor
  DaDiConstructor() {}; 

  virtual ~DaDiConstructor() {}; ///< Standard destructor

  DOMString desc();
  void setDesc(DOMString value);

  DOMString argList();
  void setArgList(DOMString value);

  DOMString argInOut();
  void setArgInOut(DOMString value);

  DOMString initList();
  void setInitList(DOMString value);

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
                               m_initList,
                               m_code;
  std::list<DaDiMethArgument*> m_daDiMethArgument;

};

inline DOMString DaDiConstructor::desc()
{
  return m_desc;
}

inline void DaDiConstructor::setDesc(DOMString value)
{
  m_desc = value;
}

inline DOMString DaDiConstructor::argList()
{
  return m_argList;
}

inline void DaDiConstructor::setArgList(DOMString value)
{
  m_argList = value;
}

inline DOMString DaDiConstructor::argInOut()
{
  return m_argInOut;
}

inline void DaDiConstructor::setArgInOut(DOMString value)
{
  m_argInOut = value;
}

inline DOMString DaDiConstructor::initList()
{
  return m_initList;
}

inline void DaDiConstructor::setInitList(DOMString value)
{
  m_initList = value;
}

inline DOMString DaDiConstructor::code()
{
  return m_code;
}

inline void DaDiConstructor::setCode(DOMString value)
{
  m_code = value;
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
