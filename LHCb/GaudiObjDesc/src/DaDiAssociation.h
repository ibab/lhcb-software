// $Id: DaDiAssociation.h,v 1.1 2003-12-17 17:31:17 mato Exp $
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
  m_destination(0),
  m_type(0),
  m_weight(0) {}; 

  virtual ~DaDiAssociation();

  const XMLCh* destination();
  void setDestination(const XMLCh* value);
    
  const XMLCh* type();
  void setType(const XMLCh* value);
  
  const XMLCh* weight();
  void setWeight(const XMLCh* value);

protected:

private:

  XMLCh *m_destination; 
  XMLCh *m_type; 
  XMLCh *m_weight; 
};

inline DaDiAssociation::~DaDiAssociation()
{
  delete [] m_destination;
  delete [] m_type;
  delete [] m_weight;
}

inline const XMLCh* DaDiAssociation::destination()
{
  return m_destination;
}

inline void DaDiAssociation::setDestination(const XMLCh* value)
{
  m_destination = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_destination, value);
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
