// $Header: /afs/cern.ch/project/cvs/reps/lhcb/GaudiObjDesc/src/DaDiEnum.h,v 1.2 2002-03-13 18:35:47 mato Exp $
#ifndef DADIENUM_H 
#define DADIENUM_H 1

// Include files
#include "dom/DOMString.hpp"

/** @class DaDiAttribute DaDiAttribute.h
 *  
 *
 *  @author Stefan Roiser
 *  @date   14/06/2001
 */
class DaDiEnum {
public:
  /// Standard constructor
  DaDiEnum() {}; 

  virtual ~DaDiEnum() {}; ///< Standard destructor

  DOMString name();
  void setName(DOMString value);
    
  DOMString desc();
  void setDesc(DOMString value);
  
  DOMString value();
  void setValue(DOMString value);

  DOMString access();
  void setAccess(DOMString value);

protected:

private:

  DOMString m_name, 
            m_desc, 
            m_value, 
            m_access;
};

inline DOMString DaDiEnum::name()
{
  return m_name;
}

inline void DaDiEnum::setName(DOMString value)
{
  m_name = value;
}

inline DOMString DaDiEnum::desc()
{
  return m_desc;
}

inline void DaDiEnum::setDesc(DOMString value)
{
  m_desc = value;
}

inline DOMString DaDiEnum::value()
{
  return m_value;
}

inline void DaDiEnum::setValue(DOMString value)
{
  m_value = value;
}

inline DOMString DaDiEnum::access()
{
  return m_access;
}

inline void DaDiEnum::setAccess(DOMString value)
{
  m_access = value;
}

#endif // DADIENUM_H
