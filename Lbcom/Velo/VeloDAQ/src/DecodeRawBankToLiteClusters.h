// $Id: DecodeRawBankToLiteClusters.h,v 1.1 2006-02-23 18:56:35 krinnert Exp $
#ifndef VELODAQ_DECODERAWBANKTOLITECLUSTERS_H 
#define VELODAQ_DECODERAWBANKTOLITECLUSTERS_H 1

#include "Event/VeloLiteCluster.h"
#include "SiDAQ/SiRawBufferWord.h"

namespace VeloDAQ {
  /**  Append lite clusters to fast container  
   *   This function decodes the cluster position part of the
   *   raw bank and appends the resulting VeloLiteClusters to
   *   to the fast container provided by the client.
   *
   *   @arg bank pointer to raw bank
   *   @arg sensorNumber sensor id corresponding to bank, needed to create channel id
   *   @arg clusters fast container to which decoded clusters will be appended
   *   @return number of decoded clusters appended
   *   @see VeloLiteCluster
   *   @author Kurt Rinnert
   *   @date   2006-02-17
   */
  unsigned int decodeRawBankToLiteClusters(const SiDAQ::buffer_word* bank, 
                                           unsigned int sensorNumber, 
                                           LHCb::VeloLiteCluster::FastContainer* clusters); 
}
#endif // VELODAQ_DECODERAWBANKTOLITECLUSTERS_H
