// $Header: /afs/cern.ch/project/cvs/reps/lhcb/GaudiObjDesc/src/DaDiAttribute.h,v 1.7 2002-04-30 16:50:24 mato Exp $
#ifndef DADIATTRIBUTE_H 
#define DADIATTRIBUTE_H 1

// Include files
#include "dom/DOMString.hpp"

#include "DaDiBitfield.h"

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

  DOMString array();
  void setArray(DOMString value);
  
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

  bool noCast();
  void setNoCast(bool value);

  bool serialize();
  void setSerialize(bool value);

  bool bitset();
  void setBitset(bool value);

	DaDiBitfield* popDaDiBitfield();
	void pushDaDiBitfield(DaDiBitfield* value);
	int sizeDaDiBitfield();

protected:

private:

  DOMString                m_name, 
                           m_type, 
                           m_desc, 
                           m_array,
                           m_access,
                           m_init,
                           m_setMeth, 
                           m_getMeth;
  bool                     m_static,
                           m_noCast,
                           m_serialize,
                           m_bitset;
  std::list<DaDiBitfield*> m_daDiBitfield;

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

inline DOMString DaDiAttribute::array()
{
  return m_array;
}

inline void DaDiAttribute::setArray(DOMString value)
{
  m_array = value;
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

inline bool DaDiAttribute::noCast()
{
  return m_noCast;
}

inline void DaDiAttribute::setNoCast(bool value)
{
  m_noCast = value;
}

inline bool DaDiAttribute::serialize()
{
  return m_serialize;
}

inline void DaDiAttribute::setSerialize(bool value)
{
  m_serialize = value;
}

inline bool DaDiAttribute::bitset()
{
  return m_bitset;
}

inline void DaDiAttribute::setBitset(bool value)
{
  m_bitset = value;
}

inline DaDiBitfield* DaDiAttribute::popDaDiBitfield()
{
	DaDiBitfield* pt = m_daDiBitfield.front();
	m_daDiBitfield.push_back(pt);
	m_daDiBitfield.pop_front();
	return pt;
}

inline void DaDiAttribute::pushDaDiBitfield(DaDiBitfield* value)
{
	m_daDiBitfield.push_back(value);
}

inline int DaDiAttribute::sizeDaDiBitfield()
{
	return m_daDiBitfield.size();
}

#endif // DADIATTRIBUTE_H
