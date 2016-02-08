// $Id: DecodeRawBankToClusters.h,v 1.5 2008-10-16 15:31:30 krinnert Exp $
#ifndef VELODAQ_DECODERAWBANKTOCLUSTERS_H 
#define VELODAQ_DECODERAWBANKTOCLUSTERS_H 1

#include <string>

#include "Event/VeloCluster.h"
#include "SiDAQ/SiRawBufferWord.h"

class DeVeloSensor;

namespace VeloDAQ {
  /**  Append  clusters to keyed container, bank version 2
   *   This function decodes the cluster position and adc counts
   *   from the  raw bank and appends the resulting VeloClusters to
   *   to the keyed container provided by the client.
   *
   *   @arg bank pointer to raw bank
   *   @arg sensor sensor corresponding to bank, needed to create channel id and for channel to strip mapping
   *   @arg assumeChipChannels if true, assume chip channels instead of strip numbers in raw bank
   *   @arg clusters keyed container to which decoded clusters will be appended
   *   @arg number of bytes in the bank, including 4 byte header, without padding bytes at the end. Is set by this function.
   *   @return number of decoded clusters appended (-1 = header error)
   *   @see VeloCluster
   *   @author Kurt Rinnert
   *   @date   2006-02-17
   */
  int decodeRawBankToClustersV2(
      const SiDAQ::buffer_word* bank, 
      const DeVeloSensor* sensor,
      const bool assumeChipChannels,
      LHCb::VeloClusters& clusters,
      int& byteCount,
      bool ignoreErrors=false); 
  
  /**  Append  clusters to keyed container, bank version 3
   *   This function decodes the cluster position and adc counts
   *   from the  raw bank and appends the resulting VeloClusters to
   *   to the keyed container provided by the client.  It is very similar to 
   *   the version 2 decoder.  However it treats the inter strip position
   *   computation and rounding axactly like the TELL1.
   *
   *   @arg bank pointer to raw bank
   *   @arg sensor sensor corresponding to bank, needed to create channel id and for channel to strip mapping
   *   @arg assumeChipChannels if true, assume chip channels instead of strip numbers in raw bank
   *   @arg clusters keyed container to which decoded clusters will be appended
   *   @arg number of bytes in the bank, including 4 byte header, without padding bytes at the end. Is set by this function.
   *   @return number of decoded clusters appended (-1 = header error)
   *   @see VeloCluster
   *   @author Kurt Rinnert
   *   @date   2006-02-17
   */
  int decodeRawBankToClustersV3(
      const SiDAQ::buffer_word* bank, 
      const DeVeloSensor* sensor,
      const bool assumeChipChannels,
      LHCb::VeloClusters& clusters,
      int& byteCount,
      std::string& errorMsg,
      bool ignoreErrors=false); 
}
#endif // VELODAQ_DECODERAWBANKTOCLUSTERS_H
