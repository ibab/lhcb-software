// $Id: DecodeRawBankToLiteClusters.cpp,v 1.7 2010-05-13 17:20:20 dhcroft Exp $

#include "VeloDet/DeVeloSensor.h"

#include "VeloRawBankDecoder.h"
#include "DecodeRawBankToLiteClusters.h"

int VeloDAQ::decodeRawBankToLiteClusters (
    const SiDAQ::buffer_word* bank, 
    const DeVeloSensor* sensor,
    const bool assumeChipChannels,
    LHCb::VeloLiteCluster::FastContainer* clusters,
    int& byteCount,
    bool ignoreErrors )
{
  // construct new raw decoder, implicitely decodes header
  VeloRawBankDecoder decoder(bank);

  // only decode banks without errors
  if ( decoder.hasError() && !ignoreErrors ) { return -1; }

  // decode the clusterpositions, create lite clusters and
  // append them to the container
  unsigned int sensorNumber = sensor->sensorNumber();
  unsigned int stripNumber;
  // only need to read ADCs to check for wrong bank length
  VeloRawBankDecoder::posadc_iterator cpi = decoder.posAdcBegin();
  for (;cpi != decoder.posAdcEnd(); ++cpi) {
    stripNumber = cpi->first.channelID();
    if (assumeChipChannels) stripNumber = 
      sensor->ChipChannelToStrip(stripNumber);
    clusters->push_back(LHCb::VeloLiteCluster(
          cpi->first.fracStripBits(),
          cpi->first.pseudoSizeBits(),
          cpi->first.hasHighThreshold(),
          LHCb::VeloChannelID(sensorNumber,stripNumber)));
  }

  // fetch number of decoded bytes, including 4 byte header, without
  // the padding bytes at the end.
  byteCount = cpi.bytesRead();
  return decoder.nClusters();
}

int VeloDAQ::decodeRawBankToLiteClusters (
    const SiDAQ::buffer_word* bank, 
    const DeVeloSensor* sensor,
    LHCb::VeloLiteCluster::FastContainer* clusters,
    bool ignoreErrors )
{
  // construct new raw decoder, implicitely decodes header
  VeloRawBankDecoder decoder(bank);

  // only decode banks without errors
  if ( decoder.hasError() && !ignoreErrors ) { return -1; }

  // decode the clusterpositions, create lite clusters and
  // append them to the container
  unsigned int sensorNumber = sensor->sensorNumber();
   // decode the clusterpositions, create lite clusters and
  // append them to the container
  for (VeloRawBankDecoder::pos_iterator cpi = decoder.posBegin();
       cpi != decoder.posEnd(); 
       ++cpi) {
    clusters->push_back(LHCb::VeloLiteCluster(cpi->fracStripBits(),
                                              cpi->pseudoSizeBits(),
                                              cpi->hasHighThreshold(),
                                              LHCb::VeloChannelID(sensorNumber,
                                                                  cpi->channelID())));
  }
  return decoder.nClusters();
}
