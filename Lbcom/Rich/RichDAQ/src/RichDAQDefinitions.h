// $Id: RichDAQDefinitions.h,v 1.5 2003-12-11 07:36:12 cattanem Exp $
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

namespace Rich {

  // Data defintions
  typedef raw_int        LongType;
  typedef unsigned int   ShortType;

  // Container for PD data
  typedef std::map< RichSmartID, MCRichDigitVector > PDMap;

  // Local definition of a RawBank constructed from 32bit ints
  typedef std::vector<Rich::LongType>                RAWBank;

  // Container of Rich RawBanks
  typedef std::vector<RawBank>                       RAWBanks;

  // Container of RichSmartIDs
  typedef std::vector<RichSmartID>                   SmartIDs;

}

#endif // RICHDAQ_RICHDAQDEFINITIONS_H
