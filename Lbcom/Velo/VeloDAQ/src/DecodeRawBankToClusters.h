// $Id: DecodeRawBankToClusters.h,v 1.1 2006-02-23 18:56:35 krinnert Exp $
#ifndef VELODAQ_DECODERAWBANKTOCLUSTERS_H 
#define VELODAQ_DECODERAWBANKTOCLUSTERS_H 1

#include "Event/VeloCluster.h"
#include "SiDAQ/SiRawBufferWord.h"

namespace VeloDAQ {
  /**  Append  clusters to keyed container
   *   This function decodes the cluster position and adc counts
   *   from the  raw bank and appends the resulting VeloClusters to
   *   to the keyed container provided by the client.
   *
   *   @arg bank pointer to raw bank
   *   @arg sensorNumber sensor id corresponding to bank, needed to create channel id
   *   @arg clusters keyed container to which decoded clusters will be appended
   *   @return number of decoded clusters appended
   *   @see VeloCluster
   *   @author Kurt Rinnert
   *   @date   2006-02-17
   */
  unsigned int decodeRawBankToClusters(const SiDAQ::buffer_word* bank, 
                                       unsigned int sensorNumber, 
                                       LHCb::VeloClusters* clusters); 
}
#endif // VELODAQ_DECODERAWBANKTOCLUSTERS_H
