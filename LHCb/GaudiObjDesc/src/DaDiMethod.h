// $Header: /afs/cern.ch/project/cvs/reps/lhcb/GaudiObjDesc/src/DaDiMethod.h,v 1.5 2002-01-30 20:29:05 mato Exp $
#ifndef DADIMETHOD_H 
#define DADIMETHOD_H 1

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
class DaDiMethod {
public:
  /// Standard constructor
  DaDiMethod() {}; 

  virtual ~DaDiMethod() {}; ///< Standard destructor

  DOMString name();
  void setName(DOMString value);

  DOMString desc();
  void setDesc(DOMString value);

  DOMString access();
  void setAccess(DOMString value);

  DOMString argList();
  void setArgList(DOMString value);

  DOMString argInOut();
  void setArgInOut(DOMString value);

  DOMString virtual_();
  void setVirtual_(DOMString value);

  DOMString code();
  void setCode(DOMString value);

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

  DOMString                    m_name, 
                               m_desc, 
                               m_access, 
                               m_argList,
                               m_argInOut,
                               m_virtual, 
                               m_code;
  bool                         m_const, 
                               m_static, 
							                 m_inline,
                               m_friend;
  DaDiMethReturn*              m_daDiMethReturn;
  std::list<DaDiMethArgument*> m_daDiMethArgument;

};

inline DOMString DaDiMethod::name()
{
  return m_name;
}

inline void DaDiMethod::setName(DOMString value)
{
  m_name = value;
}

inline DOMString DaDiMethod::desc()
{
  return m_desc;
}

inline void DaDiMethod::setDesc(DOMString value)
{
  m_desc = value;
}

inline DOMString DaDiMethod::access()
{
  return m_access;
}

inline void DaDiMethod::setAccess(DOMString value)
{
  m_access = value;
}

inline DOMString DaDiMethod::argList()
{
  return m_argList;
}

inline void DaDiMethod::setArgList(DOMString value)
{
  m_argList = value;
}

inline DOMString DaDiMethod::argInOut()
{
  return m_argInOut;
}

inline void DaDiMethod::setArgInOut(DOMString value)
{
  m_argInOut = value;
}

inline DOMString DaDiMethod::virtual_()
{
  return m_virtual;
}

inline void DaDiMethod::setVirtual_(DOMString value)
{
  m_virtual = value;
}

inline DOMString DaDiMethod::code()
{
  return m_code;
}

inline void DaDiMethod::setCode(DOMString value)
{
  m_code = value;
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
