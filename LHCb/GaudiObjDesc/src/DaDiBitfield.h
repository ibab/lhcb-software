// $Id: DaDiBitfield.h,v 1.4 2003-12-17 17:31:17 mato Exp $
#ifndef DADIBITFIELD_H
#define DADIBITFIELD_H 1

// Include files
#include "xercesc/util/XMLString.hpp"

#include <vector>
#include <string>

#include "DaDiTools.h"

/** @class DaDiBitfield DaDiBitfield.h
 *
 *
 *  @author Stefan Roiser
 *  @date   14/06/2001
 */

class DaDiBitfield {
public:
  /// Standard constructor
  DaDiBitfield() :
    m_name(0),
    m_desc(0),
    m_setMeth(0),
    m_getMeth(0),
    m_length(0),
    m_mask(std::vector<std::string>()) {};

  virtual ~DaDiBitfield();

  const XMLCh* name();
  void setName(const XMLCh* value);

  const XMLCh* desc();
  void setDesc(const XMLCh* value);

  int length();
  void setLength(const XMLCh* value);

  const XMLCh* setMeth();
  void setSetMeth(const XMLCh* value);

  const XMLCh* getMeth();
  void setGetMeth(const XMLCh* value);

  void setMask(std::vector<std::string> value);
  const std::vector<std::string>& mask();
  int sizeOfMask();

protected:

private:

  XMLCh                    *m_name;
  XMLCh                    *m_desc;
  XMLCh                    *m_setMeth;
  XMLCh                    *m_getMeth;
  int                       m_length;
  std::vector<std::string>  m_mask;

};


inline DaDiBitfield::~DaDiBitfield()
{
  delete [] m_name;
  delete [] m_desc;
  delete [] m_setMeth;
  delete [] m_getMeth;
}

inline const XMLCh* DaDiBitfield::name()
{
  return m_name;
}

inline void DaDiBitfield::setName(const XMLCh* value)
{
  m_name = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_name, value);
}

inline const XMLCh* DaDiBitfield::desc()
{
  return m_desc;
}

inline void DaDiBitfield::setDesc(const XMLCh* value)
{
  m_desc = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_desc, value);
}

inline int DaDiBitfield::length()
{
  return m_length;
}

inline void DaDiBitfield::setLength(const XMLCh* value)
{
  try
  {
    m_length = xercesc::XMLString::parseInt(value);
  }
  catch(...)
    //  if (m_length == 0)
  {
    m_length = 0;
    setMask(DaDiTools::findWords(xercesc::XMLString::transcode(value), ","));
  }
}

inline const XMLCh* DaDiBitfield::setMeth()
{
  return m_setMeth;
}

inline void DaDiBitfield::setSetMeth(const XMLCh* value)
{
  m_setMeth = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_setMeth, value);
}

inline const XMLCh* DaDiBitfield::getMeth()
{
  return m_getMeth;
}

inline void DaDiBitfield::setGetMeth(const XMLCh* value)
{
  m_getMeth = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_getMeth, value);
}

inline void DaDiBitfield::setMask(std::vector<std::string> value)
{
  m_mask = value;
}

inline const std::vector<std::string>& DaDiBitfield::mask()
{
  return m_mask;
}

inline int DaDiBitfield::sizeOfMask()
{
  return m_mask.size();
}

#endif // DADIBITFIELD_H
