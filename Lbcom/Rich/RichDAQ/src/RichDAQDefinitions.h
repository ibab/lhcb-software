
/** @file RichDAQDefinitions.h
 *
 *  Header file for RICH DAQ general definitions
 *
 *  CVS Log :-
 *  $Id: RichDAQDefinitions.h,v 1.7 2004-07-27 13:46:06 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */

#ifndef RICHDAQ_RICHDAQDEFINITIONS_H 
#define RICHDAQ_RICHDAQDEFINITIONS_H 1

// from STL
#include <vector>
#include <map>

// Event model
#include "Event/DAQTypes.h"
#include "Event/MCRichDigit.h"
#include "Event/RichDigit.h"
#include "Event/RawEvent.h"

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
  typedef raw_int        LongType;

  /// DAQ short type definition
  typedef unsigned int   ShortType;

  /// Container for PD data
  typedef std::map< RichSmartID, MCRichDigitVector > PDMap;

  /// Local definition of a RawBank constructed from 32bit ints
  typedef std::vector<LongType>                      RAWBank;

  /// Container of Rich RawBanks
  typedef std::vector<RawBank>                       RAWBanks;

  /// Container of RichSmartIDs
  typedef std::vector<RichSmartID>                   SmartIDs;

}

#endif // RICHDAQ_RICHDAQDEFINITIONS_H
