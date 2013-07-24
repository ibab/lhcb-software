// $Id: VPToTWord.h,v 1.1.1.1 2010-01-21 13:16:41 marcin Exp $
#ifndef _VPToTWord_H
#define _VPToTWord_H 1

#include <iostream>

/** @class VPToTWord
 *
 *  Class for encapsulating word in RAW data format
 *  for the VP detectors. 
 *
 *  @author Marcin Kucharczyk
 *  @date   27/12/2009
 */

class VPToTWord {

public:

  VPToTWord(int ToT) {

    m_value = (ToT << ToTBits);
  
  }

  /** Constructor with int
  @param value 
  */
  explicit VPToTWord(unsigned int value = 0):
    m_value(value){}

  /** Destructor */
  ~VPToTWord(){}

  /** Cast to short*/
  operator short() const;

  /** Retrieve value
  @return value
  */
  unsigned int value() const;

  /** Return ToT
  @return ToT
  */
  int ToT() const;

  /** Operator overloading for stringoutput */
  friend std::ostream& operator<< (std::ostream& s, 
                                   const VPToTWord& obj)
  {
    return obj.fillStream(s);
  }

  /** Fill the ASCII output stream */
  std::ostream& fillStream(std::ostream& s) const;

private:

  enum bits {
    ToTBits           =  0
  };
  enum mask {
    ToTMask     = 0x3fffffL
  };

  unsigned int m_value;

};

inline VPToTWord::operator short() const
{
  return m_value;
}

inline unsigned int VPToTWord::value() const
{
  return m_value;
}

inline int VPToTWord::ToT() const{

  return (m_value & ToTMask) >> ToTBits;

}

inline std::ostream& VPToTWord::fillStream(std::ostream& s) const
{
  s << "{ "
    << " ToT:\t" << ToT() << " } ";
  return s;
}

#endif // _VPToTWord_H
