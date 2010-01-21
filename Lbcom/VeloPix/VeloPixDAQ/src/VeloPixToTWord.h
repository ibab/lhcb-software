// $Id: VeloPixToTWord.h,v 1.1.1.1 2010-01-21 13:16:41 marcin Exp $
#ifndef _VeloPixToTWord_H
#define _VeloPixToTWord_H 1

#include <iostream>

/** @class VeloPixToTWord
 *
 *  Class for encapsulating word in RAW data format
 *  for the VeloPix detectors. 
 *
 *  @author Marcin Kucharczyk
 *  @date   27/12/2009
 */

class VeloPixToTWord {

public:

  VeloPixToTWord(int ToT) {

    m_value = (ToT << ToTBits);
  
  }

  /** Constructor with int
  @param value 
  */
  explicit VeloPixToTWord(unsigned int value = 0):
    m_value(value){}

  /** Destructor */
  ~VeloPixToTWord(){}

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
                                   const VeloPixToTWord& obj)
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

inline VeloPixToTWord::operator short() const
{
  return m_value;
}

inline unsigned int VeloPixToTWord::value() const
{
  return m_value;
}

inline int VeloPixToTWord::ToT() const{

  return (m_value & ToTMask) >> ToTBits;

}

inline std::ostream& VeloPixToTWord::fillStream(std::ostream& s) const
{
  s << "{ "
    << " ToT:\t" << ToT() << " } ";
  return s;
}

#endif // _VeloPixToTWord_H
