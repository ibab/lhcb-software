// $Id: DaDiMethArgument.h,v 1.6 2003-12-11 15:03:10 mato Exp $
#ifndef DADIMETHARGUMENT_H
#define DADIMETHARGUMENT_H 1

// Include files
#include "xercesc/util/XMLString.hpp"

/** @class DaDiMethArgument DaDiMethArgument.h
 *
 *
 *  @author Stefan Roiser
 *  @date   14/06/2001
 */
class DaDiMethArgument {
public:
  /// Standard constructor
  DaDiMethArgument() :
    m_type(0),
    m_name(0),
    m_inout(0),
    m_constant(false),
    m_isPointer(false) {};

  virtual ~DaDiMethArgument();

  const XMLCh* type();
  void setType(const XMLCh* value);

  const XMLCh* name();
  void setName(const XMLCh* value);

  const XMLCh* inout();
  void setInout(const XMLCh* value);

  bool const_();
  void setConst_(bool value);

  bool isPointer();
  void setIsPointer(bool value);

protected:

private:

  XMLCh *m_type;
  XMLCh *m_name;
  XMLCh *m_inout;
  bool   m_constant;
  bool   m_isPointer;

};

inline DaDiMethArgument::~DaDiMethArgument()
{
  delete [] m_type;
  delete [] m_name;
  delete [] m_inout;
}

inline const XMLCh* DaDiMethArgument::type()
{
  return m_type;
}

inline void DaDiMethArgument::setType(const XMLCh* value)
{
  m_type = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_type, value);
}

inline const XMLCh* DaDiMethArgument::name()
{
  return m_name;
}

inline void DaDiMethArgument::setName(const XMLCh* value)
{
  m_name = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_name, value);
}

inline const XMLCh* DaDiMethArgument::inout()
{
  return m_inout;
}

inline void DaDiMethArgument::setInout(const XMLCh* value)
{
  m_inout = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_inout, value);
}

inline bool DaDiMethArgument::const_()
{
  return m_constant;
}

inline void DaDiMethArgument::setConst_(bool value)
{
  m_constant = value;
}

inline bool DaDiMethArgument::isPointer()
{
  return m_isPointer;
}

inline void DaDiMethArgument::setIsPointer(bool value)
{
  m_isPointer = value;
}

#endif // DADIMETHARGUMENT_H
