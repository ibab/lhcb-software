// $Id: VeloPixClusterWord.h,v 1.1.1.1 2010-01-21 13:16:41 marcin Exp $
#ifndef _VeloPixClusterWord_H
#define _VeloPixClusterWord_H 1

#include <iostream>

/** @class VeloPixClusterWord
 *
 *  Class for encapsulating word in RAW data format
 *  for the VeloPix detectors. 
 *
 *  @author Marcin Kucharczyk
 *  @date   27/12/2009
 */

class VeloPixClusterWord {

public:

  VeloPixClusterWord(long pixel,
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
  explicit VeloPixClusterWord(unsigned int value = 0):
    m_value(value){}

  /** Destructor */
  ~VeloPixClusterWord(){}

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
                                   const VeloPixClusterWord& obj)
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

inline VeloPixClusterWord::operator short() const
{
  return m_value;
}

inline unsigned int VeloPixClusterWord::value() const
{
  return m_value;
}

inline long VeloPixClusterWord::pixel() const{
  return (m_value & pixelMask) >> pixelBits;
}

inline unsigned int VeloPixClusterWord::totValue() const{
  return (m_value & totValueMask) >> totValueBits;
}

inline unsigned int VeloPixClusterWord::xFract() const{
  return (m_value & xFractMask) >> xFractBits;
}

inline unsigned int VeloPixClusterWord::yFract() const{
  return (m_value & yFractMask) >> yFractBits;
}

inline bool VeloPixClusterWord::hasIsLong() const{
  return (((m_value & isLongMask) >> isLongBits != 0) ? true : false);
}

inline std::ostream& VeloPixClusterWord::fillStream(std::ostream& s) const
{
  s << "{ "
    << " pixel:\t" << pixel() << std::endl
    << " totValue:\t" << totValue() << std::endl 
    << " xFract :\t" << xFract() << std::endl 
    << " xFract :\t" << xFract() << std::endl 
    << " isLong:\t" << hasIsLong() << " } ";
  return s;
}

#endif // _VeloPixClusterWord_H
