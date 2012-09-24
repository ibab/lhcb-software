// $Id: VPClusterWord.h,v 1.1.1.1 2010-01-21 13:16:41 marcin Exp $
#ifndef _VPClusterWord_H
#define _VPClusterWord_H 1

#include <iostream>

/** @class VPClusterWord
 *
 *  Class for encapsulating word in RAW data format
 *  for the VP detectors. 
 *
 *  @author Marcin Kucharczyk
 *  @date   27/12/2009
 */

class VPClusterWord {

public:

  VPClusterWord(long pixel,
                     unsigned int totValue,
                     unsigned int xFract,
                     unsigned int yFract,
                     bool isLong) {

  unsigned int iLong = (unsigned int)(isLong);
  m_value = (pixel << pixelBits) +
            (totValue << totValueBits) +
            (xFract  << xFractBits) +
            (yFract  << yFractBits) +
            (iLong << isLongBits);
  
  }

  /** Constructor with int
  @param value 
  */
  explicit VPClusterWord(unsigned int value = 0):
    m_value(value){}

  /** Destructor */
  ~VPClusterWord(){}

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

  /** Cluster totValue 
  @return totValue
  */
  unsigned int totValue() const;

  /** xFract, yFract 
  @return fractions of pixel
  */
  unsigned int xFract() const;
  unsigned int yFract() const;

  /** isLong
  @return isLong
  */
  bool hasIsLong() const;

  /** Operator overloading for stringoutput */
  friend std::ostream& operator<< (std::ostream& s, 
                                   const VPClusterWord& obj)
  {
    return obj.fillStream(s);
  }

  /** Fill the ASCII output stream */
  std::ostream& fillStream(std::ostream& s) const;

private:

  enum bits {
    pixelBits           =  0,
    totValueBits        =  22,
    xFractBits          =  25,
    yFractBits          =  28,
    isLongBits          =  31 
  };
  enum mask {
    pixelMask     = 0x3fffff,
    totValueMask  = 0x1c00000,
    xFractMask    = 0xe000000,
    yFractMask    = 0x70000000,
    isLongMask    = 0x80000000
  };

  unsigned int m_value;

};

inline VPClusterWord::operator short() const
{
  return m_value;
}

inline unsigned int VPClusterWord::value() const
{
  return m_value;
}

inline long VPClusterWord::pixel() const{
  return (m_value & pixelMask) >> pixelBits;
}

inline unsigned int VPClusterWord::totValue() const{
  return (m_value & totValueMask) >> totValueBits;
}

inline unsigned int VPClusterWord::xFract() const{
  return (m_value & xFractMask) >> xFractBits;
}

inline unsigned int VPClusterWord::yFract() const{
  return (m_value & yFractMask) >> yFractBits;
}

inline bool VPClusterWord::hasIsLong() const{
  return (((m_value & isLongMask) >> isLongBits != 0) ? true : false);
}

inline std::ostream& VPClusterWord::fillStream(std::ostream& s) const
{
  s << "{ "
    << " pixel:\t" << pixel() << std::endl
    << " totValue:\t" << totValue() << std::endl 
    << " xFract :\t" << xFract() << std::endl 
    << " xFract :\t" << xFract() << std::endl 
    << " isLong:\t" << hasIsLong() << " } ";
  return s;
}

#endif // _VPClusterWord_H
