#pragma once
#include <iostream>
/* 
 * RawBank word format for VPDigit raw bank - for storing ChannelID + ToT of each
 * hit in the VeloPix detector
 * Tim Evans 25-09-2013
 */
class VPDigitWord {

public:

  VPDigitWord(const unsigned int channelID, const unsigned int ToT)
  {
    m_value = channelID + (ToT << totBits);
  }

  explicit VPDigitWord(unsigned int value = 0):
    m_value(value){}

  ~VPDigitWord(){}

  operator short() const;
  unsigned int value() const;
  unsigned int ChannelID() const;
  unsigned int ToT() const;

  friend std::ostream& operator<< (std::ostream& s, const VPDigitWord& obj)
  {
    return obj.fillStream(s);
  }

  std::ostream& fillStream(std::ostream& s) const;

private:

  enum bits
  {
    chidBits =  0,
    totBits  =  28
  };
  enum mask
  {
    chidMask     = 0x0fffffffL,
    totMask	 = 0xf0000000L
  };

  unsigned int m_value;

};

inline VPDigitWord::operator short() const
{
  return m_value;
}

inline unsigned int VPDigitWord::value() const
{
  return m_value;
}

inline unsigned int VPDigitWord::ChannelID() const{
  return (m_value & chidMask) >>  chidBits;
}

inline unsigned int VPDigitWord::ToT() const{
  return (m_value & totMask) >> totBits;
}

inline std::ostream& VPDigitWord::fillStream(std::ostream& s) const
{
  s << "{ " << " ChannelID:\t" << ChannelID() << " ToT:\t" << ToT() << " } ";
  return s;
}

