// $Id: DaDiRelation.h,v 1.6 2003-12-11 15:03:10 mato Exp $
#ifndef DADIRELATION_H
#define DADIRELATION_H 1

// Include files
#include "xercesc/util/XMLString.hpp"

/** @class DaDiRelation DaDiRelation.h
 *
 *
 *  @author Stefan Roiser
 *  @date   14/06/2001
 */
class DaDiRelation {
public:
  /// Standard constructor
  DaDiRelation() :
    m_name(0),
    m_type(0),
    m_desc(0),
    m_access(0),
    m_ratio(0),
    m_setMeth(0),
    m_getMeth(0),
    m_addMeth(0),
    m_remMeth(0),
    m_clrMeth(0),
    m_serialize(false) {};

  virtual ~DaDiRelation();

  const XMLCh* name();
  void setName(const XMLCh* value);

  const XMLCh* type();
  void setType(const XMLCh* value);

  const XMLCh* desc();
  void setDesc(const XMLCh* value);

  const XMLCh* access();
  void setAccess(const XMLCh* value);

  const XMLCh* ratio();
  void setRatio(const XMLCh* value);

  const XMLCh* setMeth();
  void setSetMeth(const XMLCh* value);

  const XMLCh* getMeth();
  void setGetMeth(const XMLCh* value);

  const XMLCh* addMeth();
  void setAddMeth(const XMLCh* value);

  const XMLCh* remMeth();
  void setRemMeth(const XMLCh* value);

  const XMLCh* clrMeth();
  void setClrMeth(const XMLCh* value);

  bool serialize();
  void setSerialize(bool value);

protected:

private:

  XMLCh *m_name;
  XMLCh *m_type;
  XMLCh *m_desc;
  XMLCh *m_access;
  XMLCh *m_ratio;
  XMLCh *m_setMeth;
  XMLCh *m_getMeth;
  XMLCh *m_addMeth;
  XMLCh *m_remMeth;
  XMLCh *m_clrMeth;
  bool   m_serialize;

};

inline DaDiRelation::~DaDiRelation()
{
  delete [] m_name;
  delete [] m_type;
  delete [] m_desc;
  delete [] m_access;
  delete [] m_ratio;
  delete [] m_setMeth;
  delete [] m_getMeth;
  delete [] m_addMeth;
  delete [] m_remMeth;
  delete [] m_clrMeth;
}

inline const XMLCh* DaDiRelation::name()
{
  return m_name;
}

inline void DaDiRelation::setName(const XMLCh* value)
{
  m_name = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_name, value);
}

inline const XMLCh* DaDiRelation::type()
{
  return m_type;
}

inline void DaDiRelation::setType(const XMLCh* value)
{
  m_type = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_type, value);
}

inline const XMLCh* DaDiRelation::desc()
{
  return m_desc;
}

inline void DaDiRelation::setDesc(const XMLCh* value)
{
  m_desc = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_desc, value);
}

inline const XMLCh* DaDiRelation::access()
{
  return m_access;
}

inline void DaDiRelation::setAccess(const XMLCh* value)
{
  m_access = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_access, value);
}

inline const XMLCh* DaDiRelation::ratio()
{
  return m_ratio;
}

inline void DaDiRelation::setRatio(const XMLCh* value)
{
  m_ratio = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_ratio, value);
}

inline const XMLCh* DaDiRelation::setMeth()
{
  return m_setMeth;
}

inline void DaDiRelation::setSetMeth(const XMLCh* value)
{
  m_setMeth = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_setMeth, value);
}

inline const XMLCh* DaDiRelation::getMeth()
{
  return m_getMeth;
}

inline void DaDiRelation::setGetMeth(const XMLCh* value)
{
  m_getMeth = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_getMeth, value);
}

inline const XMLCh* DaDiRelation::addMeth()
{
  return m_addMeth;
}

inline void DaDiRelation::setAddMeth(const XMLCh* value)
{
  m_addMeth = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_addMeth, value);
}

inline const XMLCh* DaDiRelation::remMeth()
{
  return m_remMeth;
}

inline void DaDiRelation::setRemMeth(const XMLCh* value)
{
  m_remMeth = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_remMeth, value);
}

inline const XMLCh* DaDiRelation::clrMeth()
{
  return m_clrMeth;
}

inline void DaDiRelation::setClrMeth(const XMLCh* value)
{
  m_clrMeth = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_clrMeth, value);
}

inline bool DaDiRelation::serialize()
{
  return m_serialize;
}

inline void DaDiRelation::setSerialize(bool value)
{
  m_serialize = value;
}


#endif // DADIRELATION_H
