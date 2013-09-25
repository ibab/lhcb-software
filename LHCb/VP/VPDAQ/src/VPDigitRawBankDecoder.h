#ifndef VPDigitRawBankDecoder_H 
#define VPDigitRawBankDecoder_H 1
// STL
#include <vector>
// Si
#include "SiDAQ/SiHeaderWord.h"
#include "SiDAQ/SiRawBufferWord.h"
#include "SiDAQ/SiADCBankTraits.h"
// Local
#include "Event/VPDigit.h"
#include "VPDigitWord.h"

#ifdef _WIN32
#pragma warning(disable : 4355) // 'this' used in base member initializer list
#endif

template<class DIGITWORD>
class VPDigitRawBankDecoder;

template<class DIGITWORD>
class VPDigitRawBankDecoder {
public:
  // Constructor
  //  Takes constant pointer to beginning of data content of a raw bank
  VPDigitRawBankDecoder(const SiDAQ::buffer_word* bank) :
    m_bank(bank),
    m_header(SiHeaderWord(bank[0])),
    m_nDigits(m_header.nClusters()),
    m_posBegin(pos_iterator(0,this)),
    m_posEnd(pos_iterator(m_nDigits,this))
  { ; }
  // Copy construction   
  VPDigitRawBankDecoder(const VPDigitRawBankDecoder& ini) :
    m_bank(ini.m_bank),
    m_header(ini.m_header),
    m_nDigits(ini.m_nDigits),
    m_posBegin(pos_iterator(0,this)),
    m_posEnd(pos_iterator(m_nDigits,this))
  { ; }
  // Assignment
  const VPDigitRawBankDecoder& operator= (const VPDigitRawBankDecoder& rhs) 
  {
    m_bank            = rhs.m_bank;
    m_header          = rhs.m_header;
    m_nDigits         = rhs.m_nDigits;
    m_posBegin        = pos_iterator(0,this);
    m_posEnd          = pos_iterator(m_nDigits,this);
    return *this;
  }
  // Destructor
  ~VPDigitRawBankDecoder() { ; }

  //  Digit decoding
  class pos_iterator {
  public:
    pos_iterator() { ; }
    pos_iterator(const pos_iterator& init) : 
      m_pos(init.m_pos),
      m_decoder(init.m_decoder),
      m_digit(init.m_digit)
    { ; }
    ~pos_iterator() { ; }
    const pos_iterator& operator=(const pos_iterator& rhs) 
    { 
      m_pos     = rhs.m_pos; 
      m_decoder = rhs.m_decoder;
      m_digit = rhs.m_digit;
      return *this;
    }
    // Increment operator reads the next digit position word
    const pos_iterator& operator++() const;
    bool operator==(const pos_iterator& rhs) const 
    { return m_pos == rhs.m_pos; }
    bool operator!=(const pos_iterator& rhs) const 
    { return m_pos != rhs.m_pos; }
    // Dereferencing to VPDigitWord
    const DIGITWORD& operator*() const { return m_digit; }
    const DIGITWORD* operator->() const { return &m_digit; }
  private:
    // Constructor with position in raw bank reference to decoder
    pos_iterator(unsigned int pos, const VPDigitRawBankDecoder* decoder);
    // Decode digit position
    void decode() const;
  private:
    mutable unsigned int m_pos;
    const VPDigitRawBankDecoder* m_decoder;
    mutable DIGITWORD m_digit;
    friend class VPDigitRawBankDecoder;
  };
  friend class pos_iterator;

  const SiHeaderWord& header() const { return m_header; }
  bool hasError() const { return m_header.hasError(); }
  unsigned int nDigits() const { return m_nDigits; }
  const pos_iterator& posBegin() const { return m_posBegin; }
  const pos_iterator& posEnd() const { return m_posEnd; }

private:
  const SiDAQ::buffer_word* m_bank;
  SiHeaderWord              m_header;
  unsigned int              m_nDigits;
  pos_iterator              m_posBegin;
  pos_iterator              m_posEnd;

};


template<class DIGITWORD>
inline VPDigitRawBankDecoder<DIGITWORD>::pos_iterator::pos_iterator
(unsigned int pos, const VPDigitRawBankDecoder<DIGITWORD>* decoder) : 
  m_pos(pos),
  m_decoder(decoder)
{ 
  decode(); 
}
template<class DIGITWORD>
inline void VPDigitRawBankDecoder<DIGITWORD>::pos_iterator::decode() const
{
  if(m_pos < m_decoder->m_nDigits) m_digit = DIGITWORD(m_decoder->m_bank[m_pos+1]);

  return;
}
template<class DIGITWORD>
const typename VPDigitRawBankDecoder<DIGITWORD>::pos_iterator& 
VPDigitRawBankDecoder<DIGITWORD>::pos_iterator::operator++() const
{
  if (*this != m_decoder->m_posEnd) {
    ++m_pos;
    decode();
  }
  return *this; 
}

#endif // VPDigitRawBankDecoder_H
