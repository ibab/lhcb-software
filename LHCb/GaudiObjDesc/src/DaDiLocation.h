// $Id: DaDiLocation.h,v 1.3 2003-04-30 12:04:19 mato Exp $
#ifndef DADILOCATION_H
#define DADILOCATION_H 1

// Include files
#include "xercesc/util/XMLString.hpp"

/** @class DaDiAttribute DaDiAttribute.h
 *
 *
 *  @author Stefan Roiser
 *  @date   14/06/2001
 */
class DaDiLocation {
public:
  /// Standard constructor
  DaDiLocation() :
    m_name(0),
    m_place(0),
    m_noQuote(false) {};

  virtual ~DaDiLocation();

  const XMLCh* name();
  void setName(const XMLCh* value);

  const XMLCh* place();
  void setPlace(const XMLCh* value);

  bool noQuote();
  void setNoQuote(bool value);

protected:

private:

  XMLCh *m_name;
  XMLCh *m_place;
  bool   m_noQuote;
};

inline DaDiLocation::~DaDiLocation()
{
  xercesc::XMLString::release(&m_name);
  xercesc::XMLString::release(&m_place);
}

inline const XMLCh* DaDiLocation::name()
{
  return m_name;
}

inline void DaDiLocation::setName(const XMLCh* value)
{
  m_name = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_name, value);
}

inline const XMLCh* DaDiLocation::place()
{
  return m_place;
}

inline void DaDiLocation::setPlace(const XMLCh* value)
{
  m_place = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_place, value);
}

inline bool DaDiLocation::noQuote()
{
  return m_noQuote;
}

inline void DaDiLocation::setNoQuote(bool value)
{
  m_noQuote = value;
}


#endif // DADILOCATION_H
