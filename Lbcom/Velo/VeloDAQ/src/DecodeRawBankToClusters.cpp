// $Id: DecodeRawBankToClusters.cpp,v 1.1 2006-02-23 18:56:35 krinnert Exp $

#include <vector>
#include <algorithm>

#include "VeloRawBankDecoder.h"
#include "VeloClusterPtrLessThan.h"
#include "DecodeRawBankToClusters.h"

unsigned int VeloDAQ::decodeRawBankToClusters(const SiDAQ::buffer_word* bank, 
                                              unsigned int sensorNumber,
                                              LHCb::VeloClusters* clusters)
{
  // construct new raw decoder, implicitely decodes header
  VeloRawBankDecoder decoder(bank);

  // make sure we have enough capacity in the container
  // to avoid unnecessary relocations
  clusters->reserve(clusters->size()+decoder.nClusters());

  // decode the clusterpositions, create  clusters and
  // append them to the container
  for (VeloRawBankDecoder::posadc_iterator padci = decoder.posAdcBegin();
       padci != decoder.posAdcEnd(); 
       ++padci) {

    LHCb::VeloChannelID vcid(sensorNumber,padci->first.channelID());
    LHCb::VeloLiteCluster lc(padci->first.hasHighThreshold(),
                             padci->first.pseudoSizeBits(),
                             padci->first.hasHighThreshold(),
                             vcid);

    
    const std::vector<SiADCWord>& adcWords = padci->second;

    int firstStrip = static_cast<int>(padci->first.channelID());

    // if we have more than two strips on the cluster we have to
    // find the offset of the cluster centre by computing
    // the weighted mean
    if (adcWords.size() > 2) {
      double adcValue;
      double sumADC         = 0.0;
      double sumWeightedPos = 0.0;
      for (unsigned int adci = 0; adci < adcWords.size(); ++adci) {
        adcValue        = adcWords[adci].adc();
        sumWeightedPos += adci*adcValue;
        sumADC         += adcValue;
      }
      firstStrip -= static_cast<unsigned int>(sumWeightedPos/sumADC);
    }

    LHCb::VeloCluster::ADCVector adcs;
    for (unsigned int i = 0; i<adcWords.size(); ++i) {
      std::pair<int,unsigned int> stripAdc(static_cast<int>(firstStrip+i),
                                           static_cast<unsigned int>(adcWords[i].adc()));
      adcs.push_back(stripAdc);
    }

    // got all we need, now append new cluster
    clusters->insert(new LHCb::VeloCluster(lc,adcs),vcid);
  }

  // finally sort them (currently by strip number from channel ID)
  std::sort(clusters->begin(),clusters->end(),VeloDAQ::veloClusterPtrLessThan);

  return decoder.nClusters();
}
