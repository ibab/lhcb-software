// $Id: DecodeRawBankToLiteClusters.cpp,v 1.6 2010-03-17 18:33:19 krinnert Exp $

#include "VeloDet/DeVeloSensor.h"

#include "VeloRawBankDecoder.h"
#include "DecodeRawBankToLiteClusters.h"

unsigned int VeloDAQ::decodeRawBankToLiteClusters (
    const SiDAQ::buffer_word* bank, 
    const DeVeloSensor* sensor,
    const bool assumeChipChannels,
    LHCb::VeloLiteCluster::FastContainer* clusters)
{
  // construct new raw decoder, implicitely decodes header
  VeloRawBankDecoder decoder(bank);

  // only decode banks without errors
  if ( decoder.hasError() ) { return 0; }

  // decode the clusterpositions, create lite clusters and
  // append them to the container
  unsigned int sensorNumber = sensor->sensorNumber();
  unsigned int stripNumber;
  for (VeloRawBankDecoder::pos_iterator cpi = decoder.posBegin();
       cpi != decoder.posEnd(); 
       ++cpi) {
    stripNumber = cpi->channelID();
    if (assumeChipChannels) stripNumber = sensor->ChipChannelToStrip(stripNumber);
    clusters->push_back(LHCb::VeloLiteCluster(
          cpi->fracStripBits(),
          cpi->pseudoSizeBits(),
          cpi->hasHighThreshold(),
          LHCb::VeloChannelID(sensorNumber,stripNumber)));
  }

  return decoder.nClusters();
}
