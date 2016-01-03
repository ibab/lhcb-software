#ifndef KERNEL_READROUTINGBITS_H 
#define KERNEL_READROUTINGBITS_H 1

// Include files
#include "Event/RawEvent.h" 
#include "Event/RawBank.h" 
#include <boost/dynamic_bitset.hpp>
#include "GaudiKernel/GaudiException.h"

/** @function ReadRoutingBits Kernel/ReadRoutingBits.h
 *  
 *  helper functions that return trigger routing bits
 *
 *  @author Patrick Koppenburg
 *  @date   2009-11-06
 */
namespace Hlt 
{
  /// get routing bits as bitset
  boost::dynamic_bitset<unsigned int> readRoutingBits(const LHCb::RawEvent* rawEvent);
  /// get list of bits that fired among a list of relevant bits
  std::vector<unsigned int> firedRoutingBits(const LHCb::RawEvent* rawEvent, 
                                             const std::vector<unsigned int>& relevantBits);
  /// get list of bits that fired among a range of relevant bits (default: 0 to 95)
  std::vector<unsigned int> firedRoutingBits(const LHCb::RawEvent* rawEvent, 
                                             const unsigned int relevantMin=0, 
                                             const unsigned int relevantMax=95);
}
#endif // KERNEL_READROUTINGBITS_H
