// $Header: /afs/cern.ch/project/cvs/reps/lhcb/GaudiObjDesc/src/DaDiRelation.h,v 1.2 2001-11-09 08:37:11 mato Exp $
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
  
  bool setMeth();
  void setSetMeth(bool value);
  
  bool getMeth();
  void setGetMeth(bool value);
  
  bool addMeth();
  void setAddMeth(bool value);
  
  bool remMeth();
  void setRemMeth(bool value);
  
  bool clrMeth();
  void setClrMeth(bool value);

protected:

private:

  DOMString m_name, 
            m_type, 
            m_desc, 
            m_access, 
            m_ratio; 
  bool      m_setMeth, 
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

inline bool DaDiRelation::setMeth()
{
  return m_setMeth;
}

inline void DaDiRelation::setSetMeth(bool value)
{
  m_setMeth = value;
}

inline bool DaDiRelation::getMeth()
{
  return m_getMeth;
}

inline void DaDiRelation::setGetMeth(bool value)
{
  m_getMeth = value;
}

inline bool DaDiRelation::addMeth()
{
  return m_addMeth;
}

inline void DaDiRelation::setAddMeth(bool value)
{
  m_addMeth = value;
}

inline bool DaDiRelation::remMeth()
{
  return m_remMeth;
}

inline void DaDiRelation::setRemMeth(bool value)
{
  m_remMeth = value;
}

inline bool DaDiRelation::clrMeth()
{
  return m_clrMeth;
}

inline void DaDiRelation::setClrMeth(bool value)
{
  m_clrMeth = value;
}


#endif // DADIRELATION_H
