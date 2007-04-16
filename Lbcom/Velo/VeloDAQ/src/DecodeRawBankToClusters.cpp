// $Id: DecodeRawBankToClusters.cpp,v 1.10 2007-04-16 12:27:34 mjohn Exp $

#include <vector>
#include <algorithm>

#include "VeloDet/DeVeloSensor.h"

#include "VeloRawBankDecoder.h"
#include "DecodeRawBankToClusters.h"

unsigned int VeloDAQ::decodeRawBankToClusters(
    const SiDAQ::buffer_word* bank, 
    const DeVeloSensor* sensor,
    const bool assumeChipChannels,
    LHCb::VeloClusters* clusters,
    int& byteCount)
{
  // construct new raw decoder, implicitely decodes header
  VeloRawBankDecoder decoder(bank);

  // make sure we have enough capacity in the container
  // to avoid unnecessary relocations
  clusters->reserve(clusters->size()+decoder.nClusters());

  // decode the clusterpositions, create  clusters and
  // append them to the container
  unsigned int sensorNumber = sensor->sensorNumber();
  unsigned int stripNumber;
  VeloRawBankDecoder::posadc_iterator padci = decoder.posAdcBegin();
  for ( ; padci != decoder.posAdcEnd(); ++padci) {

    stripNumber = padci->first.channelID();
    if (assumeChipChannels) stripNumber = sensor->ChipChannelToStrip(stripNumber);
    LHCb::VeloChannelID vcid(sensorNumber,stripNumber);
    LHCb::VeloLiteCluster lc(
        padci->first.fracStripBits(),
        padci->first.pseudoSizeBits(),
        padci->first.hasHighThreshold(),
        vcid);

    
    const std::vector<SiADCWord>& adcWords = padci->second;

    int firstStrip = static_cast<int>(stripNumber);

    // if we have more than two strips on the cluster we have to
    // find the offset of the cluster centre by computing
    // the weighted mean
    if (adcWords.size() > 1) {
      double adcValue;
      double sumADC         = 0.0;
      double sumWeightedPos = 0.0;
      for (unsigned int adci = 0; adci < adcWords.size(); ++adci) {
        adcValue        = adcWords[adci].adc();
        sumWeightedPos += adci*adcValue;
        sumADC         += adcValue;
      }
      firstStrip -= static_cast<int>(sumWeightedPos*static_cast<int>(65536./sumADC+0.5)/65536+1/16.);
    }

    LHCb::VeloCluster::ADCVector adcs;
    adcs.reserve(adcWords.size());
    for (unsigned int i = 0; i<adcWords.size(); ++i) {
      adcs.push_back( std::pair<int,unsigned int>
		      (static_cast<int>(firstStrip+i),
		       static_cast<unsigned int>(adcWords[i].adc())) );
    }

    // got all we need, now append new cluster
    clusters->insert(new LHCb::VeloCluster(lc,adcs),vcid);
  }

  // fetch number of decoded bytes, including 4 byte header, without
  // the padding bytes at the end.
  byteCount = padci.bytesRead();

  return decoder.nClusters();
}
