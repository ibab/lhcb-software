// $Id: VeloTell1Channel.h,v 1.1 2009-03-24 13:28:24 krinnert Exp $
#ifndef INCLUDE_VELOTELL1CHANNEL_H
#define INCLUDE_VELOTELL1CHANNEL_H 1


/** @file VeloTell1Channel.h
 *  
 *  A set of free functions that define the mapping from chip channels to TELL1
 *  channels and vice versa.
 *
 *  On the TELL1 the channel order is inverted in blocks of 512 channels (4
 *  chips).  This is due to a wrong assumption about the cabling in the TELL1
 *  firmware.  The firmware can not easily be fixed due to the complicated and
 *  hard to debug strip reordering implementation.  Hence we have to live with
 *  this and do the mapping in software.  This problem is not new, however its
 *  solution was never made explicit.  These functions are provided to make
 *  code that needs these mappings more readable.  All this is only relevant
 *  for the analysis of NZS data.
 *  
 *
 * @author Kurt Rinnert <kurt.rinnert@cern.ch>
 * @date   2009-03-24
 */

namespace Velo {
  
  /** @function chipChannelToTell1Channel()
   *  
   *  Mapping of chip channel to TELL1 channel.
   *
   * @param  chipChannel  the chip channel
   * @return the TELL1 channel
   */
  inline unsigned int chipChannelToTell1Channel(unsigned int chipChannel) 
  {
    return (3 - chipChannel/512)*512 + chipChannel%512;
  }

  /** @function tell1ChannelToChipChannel()
   *  
   *  Mapping of TELL1 channel to chip channel.
   *
   * @param  tell1Channel  the TELL1 channel
   * @return the chip channel
   */
  inline unsigned int tell1ChannelToChipChannel(unsigned int tell1Channel) 
  {
    // Since the mapping is an inversion, the function is the same for
    // both directions.
    return chipChannelToTell1Channel(tell1Channel);
  }

}
#endif // INCLUDE_VELOTELL1CHANNEL_H

