// $Id: VPRawBankDecoder.h,v 1.2 2010-06-10 17:06:10 cattanem Exp $
#ifndef VPRAWBANKDECODER_H 
#define VPRAWBANKDECODER_H 1
// STL
#include <vector>
// Si
#include "SiDAQ/SiHeaderWord.h"
#include "SiDAQ/SiRawBufferWord.h"
#include "SiDAQ/SiADCBankTraits.h"
// Local
#include "VPClusterWord.h"

#ifdef _WIN32
#pragma warning(disable : 4355) // 'this' used in base member initializer list
#endif

template<class CLUSTERWORD>
class VPRawBankDecoder;

template<class CLUSTERWORD>
class VPRawBankDecoder {
public:
  // Constructor
  //  Takes constant pointer to beginning of data content of a raw bank
  VPRawBankDecoder(const SiDAQ::buffer_word* bank) :
    m_bank(bank),
    m_header(SiHeaderWord(bank[0])),
    m_nClusters(m_header.nClusters()),
    m_posBegin(pos_iterator(0,this)),
    m_posEnd(pos_iterator(m_nClusters,this))
  { ; }
  // Copy construction   
  VPRawBankDecoder(const VPRawBankDecoder& ini) :
    m_bank(ini.m_bank),
    m_header(ini.m_header),
    m_nClusters(ini.m_nClusters),
    m_posBegin(pos_iterator(0,this)),
    m_posEnd(pos_iterator(m_nClusters,this))
  { ; }
  // Assignment
  const VPRawBankDecoder& operator= (const VPRawBankDecoder& rhs) 
  {
    m_bank            = rhs.m_bank;
    m_header          = rhs.m_header;
    m_nClusters       = rhs.m_nClusters;
    m_posBegin        = pos_iterator(0,this);
    m_posEnd          = pos_iterator(m_nClusters,this);
    return *this;
  }
  // Destructor
  ~VPRawBankDecoder() { ; }


  // Lite cluster decoding
  class pos_iterator {
  public:
    pos_iterator() { ; }
    pos_iterator(const pos_iterator& init) : 
      m_pos(init.m_pos),
      m_decoder(init.m_decoder),
      m_cluster(init.m_cluster)
    { ; }
    ~pos_iterator() { ; }
    const pos_iterator& operator=(const pos_iterator& rhs) 
    { 
      m_pos     = rhs.m_pos; 
      m_decoder = rhs.m_decoder;
      m_cluster = rhs.m_cluster;
      return *this;
    }
    // Increment operator reads the next cluster position word
    const pos_iterator& operator++() const;
    bool operator==(const pos_iterator& rhs) const 
    { return m_pos == rhs.m_pos; }
    bool operator!=(const pos_iterator& rhs) const 
    { return m_pos != rhs.m_pos; }
    // Dereferencing to VPCLusterWord
    const CLUSTERWORD& operator*() const { return m_cluster; }
    const CLUSTERWORD* operator->() const { return &m_cluster; }
  private:
    // Constructor with position in raw bank reference to decoder
    pos_iterator(unsigned int pos, const VPRawBankDecoder* decoder);
    // Decode cluster position
    void decode() const;
  private:
    mutable unsigned int m_pos;
    const VPRawBankDecoder* m_decoder;
    mutable CLUSTERWORD m_cluster;
    friend class VPRawBankDecoder;
  };
  friend class pos_iterator;

  // Accessors
  // Access decoded bank header
  const SiHeaderWord& header() const { return m_header; }
  bool hasError() const { return m_header.hasError(); }
  // Access number of clusters  
  unsigned int nClusters() const { return m_nClusters; }
  // Start iterator for cluster positions
  const pos_iterator& posBegin() const { return m_posBegin; }
  // End iterator for cluster positions
  const pos_iterator& posEnd() const { return m_posEnd; }

private:
  const SiDAQ::buffer_word* m_bank;
  SiHeaderWord              m_header;
  unsigned int              m_nClusters;
  pos_iterator              m_posBegin;
  pos_iterator              m_posEnd;

};


//-----------------------------------------------------------------------------
// Implementations for class template: VPRawBankDecoder<CLUSTERWORD>
//
// 2009-12-27 : Marcin Kucharczyk
//-----------------------------------------------------------------------------

// LiteCluster position iterator
template<class CLUSTERWORD>
inline VPRawBankDecoder<CLUSTERWORD>::pos_iterator::pos_iterator
(unsigned int pos, const VPRawBankDecoder<CLUSTERWORD>* decoder) : 
  m_pos(pos),
  m_decoder(decoder)
{ 
  decode(); 
}
template<class CLUSTERWORD>
inline void VPRawBankDecoder<CLUSTERWORD>::pos_iterator::decode() const
{
  if(m_pos < m_decoder->m_nClusters) m_cluster = CLUSTERWORD(m_decoder->m_bank[m_pos+1]);

  return;
}
template<class CLUSTERWORD>
const typename VPRawBankDecoder<CLUSTERWORD>::pos_iterator& 
VPRawBankDecoder<CLUSTERWORD>::pos_iterator::operator++() const
{
  if (*this != m_decoder->m_posEnd) {
    ++m_pos;
    decode();
  }
  return *this; 
}

#endif // VPRAWBANKDECODER_H
