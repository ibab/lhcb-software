// $Header: /afs/cern.ch/project/cvs/reps/lhcb/GaudiObjDesc/src/DaDiAttribute.h,v 1.5 2002-02-11 09:24:31 mato Exp $
#ifndef DADIATTRIBUTE_H 
#define DADIATTRIBUTE_H 1

// Include files
#include "dom/DOMString.hpp"

/** @class DaDiAttribute DaDiAttribute.h
 *  
 *
 *  @author Stefan Roiser
 *  @date   14/06/2001
 */
class DaDiAttribute {
public:
  /// Standard constructor
  DaDiAttribute() {}; 

  virtual ~DaDiAttribute() {}; ///< Standard destructor

  DOMString name();
  void setName(DOMString value);
  
  DOMString type();
  void setType(DOMString value);
  
  DOMString desc();
  void setDesc(DOMString value);
  
  DOMString access();
  void setAccess(DOMString value);
  
  DOMString init();
  void setInit(DOMString value);

  DOMString setMeth();
  void setSetMeth(DOMString value);
  
  DOMString getMeth();
  void setGetMeth(DOMString value);

  bool static_();
  void setStatic_(bool value);

protected:

private:

  DOMString m_name, 
            m_type, 
            m_desc, 
            m_access,
            m_init,
            m_setMeth, 
            m_getMeth;
  bool      m_static;

};


inline DOMString DaDiAttribute::name()
{
  return m_name;
}

inline void DaDiAttribute::setName(DOMString value)
{
  m_name = value;
}

inline DOMString DaDiAttribute::type()
{
  return m_type;
}

inline void DaDiAttribute::setType(DOMString value)
{
  m_type = value;
}

inline DOMString DaDiAttribute::desc()
{
  return m_desc;
}

inline void DaDiAttribute::setDesc(DOMString value)
{
  m_desc = value;
}

inline DOMString DaDiAttribute::access()
{
  return m_access;
}

inline void DaDiAttribute::setAccess(DOMString value)
{
  m_access = value;
}

inline DOMString DaDiAttribute::init()
{
  return m_init;
}

inline void DaDiAttribute::setInit(DOMString value)
{
  m_init = value;
}

inline DOMString DaDiAttribute::setMeth()
{
  return m_setMeth;
}

inline void DaDiAttribute::setSetMeth(DOMString value)
{
  m_setMeth = value;
}

inline DOMString DaDiAttribute::getMeth()
{
  return m_getMeth;
}

inline void DaDiAttribute::setGetMeth(DOMString value)
{
  m_getMeth = value;
}

inline bool DaDiAttribute::static_()
{
  return m_static;
}

inline void DaDiAttribute::setStatic_(bool value)
{
  m_static = value;
}

#endif // DADIATTRIBUTE_H
