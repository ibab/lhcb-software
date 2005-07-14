
//-----------------------------------------------------------------------------
/** @file RichDAQDefinitions.h
 *
 *  Header file for RICH DAQ general definitions
 *
 *  CVS Log :-
 *  $Id: RichDAQDefinitions.h,v 1.5 2005-07-14 14:10:41 jonrob Exp $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHDAQDEFINITIONS_H
#define RICHKERNEL_RICHDAQDEFINITIONS_H 1

// from STL
#include <vector>

// GaudiKernel
#include "GaudiKernel/stl_hash.h"

// Event model
#include "Event/DAQTypes.h"
#include "Event/RawBank.h"

// Kernel
#include "Kernel/RichSmartID.h"
#include "RichKernel/RichHashMap.h"
#include "RichKernel/RichMap.h"
#include "RichKernel/RichSmartIDHashFuncs.h"

/** @namespace RichDAQ
 *
 *  Namespace for definitions related to Rich DAQ processing
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */
namespace RichDAQ {

  /// DAQ long type definition
  typedef raw_int                                           LongType;

  /// DAQ short type definition
  typedef unsigned int                                      ShortType;

  /// Level1 ID number
  typedef ShortType                                         Level1ID;

  /// Container for RICH HPD data, sorted by HPD number
  typedef RichMap< RichSmartID, RichSmartID::Collection >   PDMap;

  /// Container for RICH HPD data, sorted by Level1 board number
  typedef RichMap< Level1ID, PDMap >                        L1Map;

  /// Local definition of a RawBank constructed from 32bit ints
  typedef std::vector< LongType >                           RAWBank;

  /// Container of Rich RawBanks
  typedef std::vector< RawBank >                            RAWBanks;

  /// RICH HPD Hardware ID
  typedef unsigned int                                      HPDHardwareID;

  /// Vector of HPD Hardware IDs
  typedef std::vector< HPDHardwareID >                      HPDHardwareIDs;

  /// Vector of Level 1 IDs
  typedef std::vector< Level1ID >                           Level1IDs;

  /// Mapping from Level1 ID to list of HPD RichSmartIDs
  typedef RichMap< const RichDAQ::Level1ID, RichSmartID::Collection > L1ToSmartIDs;

  /// Mapping from Level1 ID to list of HPD RichSmartIDs
  typedef RichMap< const RichDAQ::Level1ID, RichDAQ::HPDHardwareIDs > L1ToHardIDs;

  /// Maximum data block size
  static const RichDAQ::ShortType MaxDataSize = 32;

  /** @enum BankVersion
   *
   *  Enumeration for the RICH DAQ bank version
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   20/12/2004
   */
  enum BankVersion
    {
      LHCb0  = 0, ///< First LHCb mode version. Compatible with DC04
      LHCb1  = 1, ///< Second LHCb mode version. Same as LHCb0 with new header + Level1 grouping 
      LHCb2  = 2  ///< Third LHCb mode version. Same as LHCb1 with new zero suppression format
    };

}

// Hash functions

#ifdef __GNUC__
namespace __gnu_cxx
{
  // HPDHardwareID
  template <> struct hash<const RichDAQ::HPDHardwareID>
  { size_t operator() ( const RichDAQ::HPDHardwareID id ) const { return (size_t)id ; } } ;
  template <> struct hash<const RichDAQ::HPDHardwareID&>
  { size_t operator() ( const RichDAQ::HPDHardwareID id ) const { return (size_t)id ; } } ;
}
#endif

#endif // RICHKERNEL_RICHDAQDEFINITIONS_H
