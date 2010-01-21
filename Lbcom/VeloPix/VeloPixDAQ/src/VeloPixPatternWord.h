// $Id: VeloPixPatternWord.h,v 1.1.1.1 2010-01-21 13:16:41 marcin Exp $
#ifndef _VeloPixPatternWord_H
#define _VeloPixPatternWord_H 1

#include <iostream>

/** @class VeloPixPatternWord
 *
 *  Class for encapsulating word in RAW data format
 *  for the VeloPix detectors. 
 *
 *  @author Marcin Kucharczyk
 *  @date   27/12/2009
 */

class VeloPixPatternWord {

public:

  VeloPixPatternWord(long pixel,
                     long pattern) {

  m_value = (pixel << pixelBits) +
            (pattern << patternBits);
  
  }

  /** Constructor with int
  @param value 
  */
  explicit VeloPixPatternWord(unsigned int value = 0):
    m_value(value){}

  /** Destructor */
  ~VeloPixPatternWord(){}

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
                                   const VeloPixPatternWord& obj)
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

inline VeloPixPatternWord::operator short() const
{
  return m_value;
}

inline unsigned int VeloPixPatternWord::value() const
{
  return m_value;
}

inline long VeloPixPatternWord::pixel() const{
  return (m_value & pixelMask) >> pixelBits;
}

inline long VeloPixPatternWord::pattern() const{
  return (m_value & patternMask) >> patternBits;
}

inline std::ostream& VeloPixPatternWord::fillStream(std::ostream& s) const
{
  s << "{ "
    << " pixel:\t" << pixel() << std::endl
    << " pattern:\t" << pattern() << " } ";
  return s;
}

#endif // _VeloPixPatternWord_H
