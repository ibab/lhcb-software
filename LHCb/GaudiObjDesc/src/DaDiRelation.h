// $Header: /afs/cern.ch/project/cvs/reps/lhcb/GaudiObjDesc/src/DaDiRelation.h,v 1.1.1.1 2001-10-03 16:39:17 mato Exp $
#ifndef DADIRELATION_H 
#define DADIRELATION_H 1

// Include files
#include "dom/DOMString.hpp"

/** @class DaDiRelation DaDiRelation.h
 *  
 *
 *  @author Stefan Roiser
 *  @date   14/06/2001
 */
class DaDiRelation {
public:
  /// Standard constructor
  DaDiRelation() {}; 

  virtual ~DaDiRelation() {}; ///< Standard destructor

  DOMString name();
  void setName(DOMString value);
  
  DOMString type();
  void setType(DOMString value);
  
  DOMString desc();
  void setDesc(DOMString value);
  
  DOMString access();
  void setAccess(DOMString value);
  
  DOMString ratio();
  void setRatio(DOMString value);
  
  DOMString setMeth();
  void setSetMeth(DOMString value);
  
  DOMString getMeth();
  void setGetMeth(DOMString value);
  
  DOMString addMeth();
  void setAddMeth(DOMString value);
  
  DOMString remMeth();
  void setRemMeth(DOMString value);
  
  DOMString clrMeth();
  void setClrMeth(DOMString value);

protected:

private:

  DOMString m_name, 
            m_type, 
            m_desc, 
            m_access, 
            m_ratio, 
            m_setMeth, 
            m_getMeth, 
            m_addMeth, 
            m_remMeth, 
            m_clrMeth;

};


inline DOMString DaDiRelation::name()
{
  return m_name;
}

inline void DaDiRelation::setName(DOMString value)
{
  m_name = value;
}

inline DOMString DaDiRelation::type()
{
  return m_type;
}

inline void DaDiRelation::setType(DOMString value)
{
  m_type = value;
}

inline DOMString DaDiRelation::desc()
{
  return m_desc;
}

inline void DaDiRelation::setDesc(DOMString value)
{
  m_desc = value;
}

inline DOMString DaDiRelation::access()
{
  return m_access;
}

inline void DaDiRelation::setAccess(DOMString value)
{
  m_access = value;
}

inline DOMString DaDiRelation::ratio()
{
  return m_ratio;
}

inline void DaDiRelation::setRatio(DOMString value)
{
  m_ratio = value;
}

inline DOMString DaDiRelation::setMeth()
{
  return m_setMeth;
}

inline void DaDiRelation::setSetMeth(DOMString value)
{
  m_setMeth = value;
}

inline DOMString DaDiRelation::getMeth()
{
  return m_getMeth;
}

inline void DaDiRelation::setGetMeth(DOMString value)
{
  m_getMeth = value;
}

inline DOMString DaDiRelation::addMeth()
{
  return m_addMeth;
}

inline void DaDiRelation::setAddMeth(DOMString value)
{
  m_addMeth = value;
}

inline DOMString DaDiRelation::remMeth()
{
  return m_remMeth;
}

inline void DaDiRelation::setRemMeth(DOMString value)
{
  m_remMeth = value;
}

inline DOMString DaDiRelation::clrMeth()
{
  return m_clrMeth;
}

inline void DaDiRelation::setClrMeth(DOMString value)
{
  m_clrMeth = value;
}


#endif // DADIRELATION_H
