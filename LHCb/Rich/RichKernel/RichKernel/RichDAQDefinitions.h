
/** @file RichDAQDefinitions.h
 *
 *  Header file for RICH DAQ general definitions
 *
 *  CVS Log :-
 *  $Id: RichDAQDefinitions.h,v 1.1 2005-01-07 13:16:23 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.7  2004/07/27 13:46:06  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */

#ifndef RICHKERNEL_RICHDAQDEFINITIONS_H 
#define RICHKERNEL_RICHDAQDEFINITIONS_H 1

// from STL
#include <vector>
#include <map>

// Event model
#include "Event/DAQTypes.h"
#include "Event/RawBank.h"

// Kernel
#include "Kernel/RichSmartID.h"

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
  typedef std::map< RichSmartID, RichSmartID::Collection >  PDMap;

  /// Container for RICH HPD data, sorted by Level1 board number
  typedef std::map< Level1ID, PDMap >                       L1Map;

  /// Local definition of a RawBank constructed from 32bit ints
  typedef std::vector< LongType >                           RAWBank;

  /// Container of Rich RawBanks
  typedef std::vector< RawBank >                            RAWBanks;

  /// RICH HPD Hardware ID
  typedef unsigned int                                      HPDHardwareID;

  /// Vector of HPD Hardware IDs
  typedef std::vector< HPDHardwareID >                      HPDHardwareIDs;

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
      LHCb0  = 0,   ///< First LHCb mode version. Compatible with DC04
      LHCb1  = 1,   ///< Second LHCb mode version. Level1 grouping + new zero suppression format
      Alice0 = 2    ///< First Alice mode version.
    };

}

#endif // RICHKERNEL_RICHDAQDEFINITIONS_H 
