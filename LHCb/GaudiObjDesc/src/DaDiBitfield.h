// $Header: /afs/cern.ch/project/cvs/reps/lhcb/GaudiObjDesc/src/DaDiBitfield.h,v 1.1 2002-04-30 16:52:49 mato Exp $
#ifndef DADIBITFIELD_H 
#define DADIBITFIELD_H 1

// Include files
#include "dom/DOMString.hpp"

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
  DaDiBitfield() {}; 

  virtual ~DaDiBitfield() {}; ///< Standard destructor

  DOMString name();
  void setName(DOMString value);
  
  DOMString desc();
  void setDesc(DOMString value);
  
  int length();
  void setLength(DOMString value);
  
  DOMString setMeth();
  void setSetMeth(DOMString value);
  
  DOMString getMeth();
  void setGetMeth(DOMString value);

  bool startAtOne();
  void setStartAtOne(bool value);

  void setMask(std::vector<std::string> value);
  const std::vector<std::string>& mask();
  int sizeOfMaks();

protected:

private:

  DOMString                m_name,
                           m_desc,
                           m_setMeth, 
                           m_getMeth;
  int                      m_length;
  bool                     m_startAtOne;
  std::vector<std::string> m_mask;

};


inline DOMString DaDiBitfield::name()
{
  return m_name;
}

inline void DaDiBitfield::setName(DOMString value)
{
  m_name = value;
}

inline DOMString DaDiBitfield::desc()
{
  return m_desc;
}

inline void DaDiBitfield::setDesc(DOMString value)
{
  m_desc = value;
}

inline int DaDiBitfield::length()
{
  return m_length;
}

inline void DaDiBitfield::setLength(DOMString value)
{
  m_length = atoi(value.transcode());
  if (m_length == 0)
  {
    setMask(DaDiTools::findWords(value.transcode(), ","));
  }
}

inline DOMString DaDiBitfield::setMeth()
{
  return m_setMeth;
}

inline void DaDiBitfield::setSetMeth(DOMString value)
{
  m_setMeth = value;
}

inline DOMString DaDiBitfield::getMeth()
{
  return m_getMeth;
}

inline void DaDiBitfield::setGetMeth(DOMString value)
{
  m_getMeth = value;
}

inline bool DaDiBitfield::startAtOne()
{
  return m_startAtOne;
}

inline void DaDiBitfield::setStartAtOne(bool value)
{
  m_startAtOne = value;
}

inline void DaDiBitfield::setMask(std::vector<std::string> value)
{
  m_mask = value;
}

inline const std::vector<std::string>& DaDiBitfield::mask()
{
  return m_mask;
}

inline int DaDiBitfield::sizeOfMaks()
{
  return m_mask.size();
}

#endif // DADIBITFIELD_H
