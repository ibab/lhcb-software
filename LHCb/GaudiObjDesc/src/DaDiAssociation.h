// $Id: DaDiAssociation.h,v 1.2 2003-12-18 14:05:32 mato Exp $
#ifndef DADIASSOCIATION_H 
#define DADIASSOCIATION_H 1

// Include files
#include "xercesc/util/XMLString.hpp"

/** @class DaDiAssociation DaDiAssociation.h
 *  
 *
 *  @author Stefan Roiser
 *  @date   14/06/2001
 */
class DaDiAssociation {
public:
  /// Standard constructor
  DaDiAssociation() :
  m_from(0),
  m_to(0),
  m_type(0),
  m_weight(0) {}; 

  virtual ~DaDiAssociation();

  const XMLCh* from();
  void setFrom(const XMLCh* value);

  const XMLCh* to();
  void setTo(const XMLCh* value);
    
  const XMLCh* type();
  void setType(const XMLCh* value);
  
  const XMLCh* weight();
  void setWeight(const XMLCh* value);

protected:

private:

  XMLCh *m_from;
  XMLCh *m_to; 
  XMLCh *m_type; 
  XMLCh *m_weight; 
};

inline DaDiAssociation::~DaDiAssociation()
{
  delete [] m_from;
  delete [] m_to;
  delete [] m_type;
  delete [] m_weight;
}

inline const XMLCh* DaDiAssociation::from()
{
  return m_from;
}

inline void DaDiAssociation::setFrom(const XMLCh* value)
{
  m_from = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_from, value);
}

inline const XMLCh* DaDiAssociation::to()
{
  return m_to;
}

inline void DaDiAssociation::setTo(const XMLCh* value)
{
  m_to = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_to, value);
}

inline const XMLCh* DaDiAssociation::type()
{
  return m_type;
}

inline void DaDiAssociation::setType(const XMLCh* value)
{
  m_type = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_type, value);
}

inline const XMLCh* DaDiAssociation::weight()
{
  return m_weight;
}

inline void DaDiAssociation::setWeight(const XMLCh* value)
{
  m_weight = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_weight, value);
}

#endif // DADIASSOCIATION_H
