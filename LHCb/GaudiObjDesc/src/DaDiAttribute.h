// $Id: DaDiAttribute.h,v 1.11 2003-12-17 17:31:17 mato Exp $
#ifndef DADIATTRIBUTE_H
#define DADIATTRIBUTE_H 1

// Include files
#include "xercesc/util/XMLString.hpp"

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
  DaDiAttribute() :
    m_name(0),
    m_type(0),
    m_desc(0),
    m_array(0),
    m_access(0),
    m_init(0),
    m_dictalias(0),
    m_setMeth(0),
    m_getMeth(0),
    m_static(false),
    m_compression(false),
    m_serialize(false),
    m_transient(false),
    m_bitset(0),
    m_daDiBitfield(std::list<DaDiBitfield*>()) {};

  virtual ~DaDiAttribute();

  const XMLCh* name();
  void setName(const XMLCh* value);

  const XMLCh* type();
  void setType(const XMLCh* value);

  const XMLCh* desc();
  void setDesc(const XMLCh* value);

  const XMLCh* array();
  void setArray(const XMLCh* value);

  const XMLCh* access();
  void setAccess(const XMLCh* value);

  const XMLCh* init();
  void setInit(const XMLCh* value);

  const XMLCh* dictalias();
  void setDictalias(const XMLCh* value);

  const XMLCh* setMeth();
  void setSetMeth(const XMLCh* value);

  const XMLCh* getMeth();
  void setGetMeth(const XMLCh* value);

  bool static_();
  void setStatic_(bool value);

  bool compression();
  void setCompression(bool value);

  bool serialize();
  void setSerialize(bool value);

  bool transient();
  void setTransient(bool value);  

  int bitset();
  void setBitset(int value);

  DaDiBitfield* popDaDiBitfield();
  void pushDaDiBitfield(DaDiBitfield* value);
  int sizeDaDiBitfield();

protected:

private:

  XMLCh                    *m_name;
  XMLCh                    *m_type;
  XMLCh                    *m_desc;
  XMLCh                    *m_array;
  XMLCh                    *m_access;
  XMLCh                    *m_init;
  XMLCh                    *m_dictalias;
  XMLCh                    *m_setMeth;
  XMLCh                    *m_getMeth;
  bool                      m_static;
  bool                      m_compression;
  bool                      m_serialize;
  bool                      m_transient;
  int                       m_bitset;
  std::list<DaDiBitfield*>  m_daDiBitfield;

};

inline DaDiAttribute::~DaDiAttribute()
{
  delete [] m_name;
  delete [] m_type;
  delete [] m_desc;
  delete [] m_array;
  delete [] m_access;
  delete [] m_init;
  delete [] m_setMeth;
  delete [] m_getMeth;
  delete [] m_dictalias;
  std::list<DaDiBitfield*>::iterator bIter;
  for (bIter = m_daDiBitfield.begin(); bIter != m_daDiBitfield.end(); ++bIter)
  { delete *bIter; }
}

inline const XMLCh* DaDiAttribute::name()
{
  return m_name;
}

inline void DaDiAttribute::setName(const XMLCh* value)
{
  m_name = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_name, value);
}

inline const XMLCh* DaDiAttribute::type()
{
  return m_type;
}

inline void DaDiAttribute::setType(const XMLCh* value)
{
  m_type = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_type, value);
}

inline const XMLCh* DaDiAttribute::desc()
{
  return m_desc;
}

inline void DaDiAttribute::setDesc(const XMLCh* value)
{
  m_desc = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_desc, value);
}

inline const XMLCh* DaDiAttribute::array()
{
  return m_array;
}

inline void DaDiAttribute::setArray(const XMLCh* value)
{
  m_array = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_array, value);
}

inline const XMLCh* DaDiAttribute::access()
{
  return m_access;
}

inline void DaDiAttribute::setAccess(const XMLCh* value)
{
  m_access = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_access, value);
}

inline const XMLCh* DaDiAttribute::init()
{
  return m_init;
}

inline void DaDiAttribute::setInit(const XMLCh* value)
{
  m_init = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_init, value);
}

inline const XMLCh* DaDiAttribute::dictalias()
{
  return m_dictalias;
}

inline void DaDiAttribute::setDictalias(const XMLCh* value)
{
  m_dictalias = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_dictalias, value);
}

inline const XMLCh* DaDiAttribute::setMeth()
{
  return m_setMeth;
}

inline void DaDiAttribute::setSetMeth(const XMLCh* value)
{
  m_setMeth = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_setMeth, value);
}

inline const XMLCh* DaDiAttribute::getMeth()
{
  return m_getMeth;
}

inline void DaDiAttribute::setGetMeth(const XMLCh* value)
{
  m_getMeth = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_getMeth, value);
}

inline bool DaDiAttribute::static_()
{
  return m_static;
}

inline void DaDiAttribute::setStatic_(bool value)
{
  m_static = value;
}

inline bool DaDiAttribute::compression()
{
  return m_compression;
}

inline void DaDiAttribute::setCompression(bool value)
{
  m_compression = value;
}

inline bool DaDiAttribute::serialize()
{
  return m_serialize;
}

inline void DaDiAttribute::setSerialize(bool value)
{
  m_serialize = value;
}

inline bool DaDiAttribute::transient()
{
  return m_transient;
}

inline void DaDiAttribute::setTransient(bool value)
{
  m_transient = value;
}

inline int DaDiAttribute::bitset()
{
  return m_bitset;
}

inline void DaDiAttribute::setBitset(int value)
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
