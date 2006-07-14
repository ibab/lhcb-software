// $Id: DecodeRawBankToLiteClusters.cpp,v 1.4 2006-07-14 11:42:24 dhcroft Exp $

#include "VeloRawBankDecoder.h"
#include "DecodeRawBankToLiteClusters.h"

unsigned int VeloDAQ::decodeRawBankToLiteClusters (const SiDAQ::buffer_word* bank, 
                                                   unsigned int sensorNumber,
                                                   LHCb::VeloLiteCluster::FastContainer* clusters)
{
  // construct new raw decoder, implicitely decodes header
  VeloRawBankDecoder decoder(bank);


  // decode the clusterpositions, create lite clusters and
  // append them to the container
  for (VeloRawBankDecoder::pos_iterator cpi = decoder.posBegin();
       cpi != decoder.posEnd(); 
       ++cpi) {
    clusters->push_back(LHCb::VeloLiteCluster(cpi->fracStripBits(),
                                              cpi->pseudoSizeBits(),
                                              cpi->hasHighThreshold(),
                                              LHCb::VeloChannelID(sensorNumber,cpi->channelID())));
  }

  return decoder.nClusters();
}
