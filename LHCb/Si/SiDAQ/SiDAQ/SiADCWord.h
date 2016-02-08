#ifndef _SiADCWord_H
#define _SiADCWord_H 1

/** @class SiADCWord SiADCWord.h "SiDAQ/SiADCWord.h"
 *
 *  Class for encapsulating ADC word 
 *  for the Si. 
 *
 *  @author M.Needham
 *  @date   18/10/2005
 */


#include <iostream>

class SiADCWord final {

public:

  /** constructer with chan + number of ADC values
  @param adcvalue
  @param flag EOC 
  */ 
  SiADCWord(double adcValue, bool newCluster);

  /** constructer with int
  @param value 
  */
  explicit SiADCWord(unsigned int value):
  m_value(value){}

  /** Default Constructor */
  SiADCWord() = default;

  /** cas to char */
  operator char() const;
 
  /** Retrieve value 
  @return value
  */
  unsigned int value() const;

  /** return adc counts 
  @return adc
  */
  double adc() const;

  /** end of cluster flag 
  @return end of cluster
  */
  bool endCluster() const;

  /** Operator overloading for stringoutput */
  friend std::ostream& operator<< (std::ostream& s, const SiADCWord& obj)
  {
    return obj.fillStream(s);
  }

  /** Fill the ASCII output stream */
  std::ostream& fillStream(std::ostream& s) const;
 
private:

  enum bits {adcBits = 0, endBits= 7, adcPrecision = endBits};
  
  enum mask {adcMask = 0x7f , endMask = 0x80 };

  unsigned char m_value = 0; /// Packed word
};


inline SiADCWord::SiADCWord(double adc, bool endCluster)
{

  // constructer
  unsigned int endC = (unsigned int)(endCluster);
   
  unsigned char tADC;
  adc > ((1<<adcPrecision)-1) ? tADC = (1<<adcPrecision) -1 : tADC = char(adc);

  m_value  = ((tADC << SiADCWord::adcBits) & SiADCWord::adcMask) |
             ((endC << SiADCWord::endBits) & SiADCWord::endMask);
 }

inline  SiADCWord::operator char() const
{
  return m_value;
}

inline unsigned int SiADCWord::value() const
{
  return m_value;
}

inline double SiADCWord::adc() const{
  return double(((m_value & adcMask) >> adcBits));
}

inline bool SiADCWord::endCluster() const{
  return (((m_value & endMask) >> endBits !=0u) ? true : false);
}

inline std::ostream& SiADCWord::fillStream(std::ostream& s) const
{
  s << "{ "
    << " value:\t" << std::hex << value() <<  std::endl
    << " start cluster:\t" << endCluster() << std::endl 
    << " adc counts \t" << adc() << std::endl;
  return s;
}

#endif // _SiADCWord_H
