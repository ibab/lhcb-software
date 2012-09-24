// $Id: VPPatternWord.h,v 1.1.1.1 2010-01-21 13:16:41 marcin Exp $
#ifndef _VPPatternWord_H
#define _VPPatternWord_H 1

#include <iostream>

/** @class VPPatternWord
 *
 *  Class for encapsulating word in RAW data format
 *  for the VP detectors. 
 *
 *  @author Marcin Kucharczyk
 *  @date   27/12/2009
 */

class VPPatternWord {

public:

  VPPatternWord(long pixel,
                     long pattern) {

  m_value = (pixel << pixelBits) +
            (pattern << patternBits);
  
  }

  /** Constructor with int
  @param value 
  */
  explicit VPPatternWord(unsigned int value = 0):
    m_value(value){}

  /** Destructor */
  ~VPPatternWord(){}

  /** Cast to short*/
  operator short() const;

  /** Retrieve value
  @return value
  */
  unsigned int value() const;

  /** Return pixel
  @return pixel
  */
  long pixel() const;

  /** Pattern value 
  @return pattern
  */
  long pattern() const;

  /** Operator overloading for stringoutput */
  friend std::ostream& operator<< (std::ostream& s, 
                                   const VPPatternWord& obj)
  {
    return obj.fillStream(s);
  }

  /** Fill the ASCII output stream */
  std::ostream& fillStream(std::ostream& s) const;

private:

  enum bits {
    pixelBits           =  0,
    patternBits         =  22
  };
  enum mask {
    pixelMask     = 0x3fffffL,
    patternMask   = 0x1c00000L
  };

  unsigned int m_value;

};

inline VPPatternWord::operator short() const
{
  return m_value;
}

inline unsigned int VPPatternWord::value() const
{
  return m_value;
}

inline long VPPatternWord::pixel() const{
  return (m_value & pixelMask) >> pixelBits;
}

inline long VPPatternWord::pattern() const{
  return (m_value & patternMask) >> patternBits;
}

inline std::ostream& VPPatternWord::fillStream(std::ostream& s) const
{
  s << "{ "
    << " pixel:\t" << pixel() << std::endl
    << " pattern:\t" << pattern() << " } ";
  return s;
}

#endif // _VPPatternWord_H
