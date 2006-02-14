// $Id: VeloRawBankDecoder.cpp,v 1.1 2006-02-14 18:25:36 krinnert Exp $

#include "VeloDAQ/VeloRawBankDecoder.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloRawBankDecoder
//
// 2006-02-08 : Kurt Rinnert
//-----------------------------------------------------------------------------

// position iterator

VeloRawBankDecoder::pos_iterator::pos_iterator(unsigned int pos, const VeloRawBankDecoder* decoder) : 
  m_pos(pos),
  m_decoder(decoder)
{ 
  decode(); 
}

void VeloRawBankDecoder::pos_iterator::decode() const
{
  if (m_pos >= m_decoder->m_nClusters) return;  // nothing to see anymore, move along

  m_cluster = VeloClusterWord((m_decoder->m_bank[1+m_pos/2] >> ((m_pos%2) << 4) & 0x0000FFFF));
  return;
}

const VeloRawBankDecoder::pos_iterator& VeloRawBankDecoder::pos_iterator::operator++() const
{
  if (*this != m_decoder->m_posEnd) {
    ++m_pos;
    decode();
  }

  return *this; 
}

// position and ADC cluster iterator

VeloRawBankDecoder::posadc_iterator::posadc_iterator(unsigned int pos, const VeloRawBankDecoder* decoder) : 
      m_pos(pos),
      m_nADC(0),
      m_ADC32(0),
      m_offset(1+ decoder->m_nClusters/2 + decoder->m_nClusters%2),
      m_decoder(decoder)
{ 
  decode(); 
}

void VeloRawBankDecoder::posadc_iterator::decode() const
{
  if (m_pos >= m_decoder->m_nClusters) return;  // nothing to see anymore, move along

  m_cluster.first = VeloClusterWord((m_decoder->m_bank[1+m_pos/2] >> ((m_pos%2) << 4)) & 0x0000FFFF);

  m_cluster.second.clear();

  // there is a first time for everything
  if (m_nADC%4 == 0) m_ADC32 = m_decoder->m_bank[m_offset+m_nADC/4];
  m_cluster.second.push_back(SiADCWord((m_ADC32 >> ((m_nADC%4)<<3)) & 0x000000FF));
  ++m_nADC;

  while (!m_cluster.second.back().endCluster()) {
    if (m_nADC%4 == 0) m_ADC32 = m_decoder->m_bank[m_offset+m_nADC/4];
    m_cluster.second.push_back(SiADCWord((m_ADC32 >> ((m_nADC%4)<<3)) & 0x000000FF));
    ++m_nADC;
  }

  return;
}

const VeloRawBankDecoder::posadc_iterator& VeloRawBankDecoder::posadc_iterator::operator++() const
{
  if (*this != m_decoder->m_posAdcEnd) {
    ++m_pos;
    decode();
  }
  
  return *this; 
}
