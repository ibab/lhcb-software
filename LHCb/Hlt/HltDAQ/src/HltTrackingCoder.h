// $Id: HltTrackingCoder.h $
#ifndef HLTTRACKINGCODER_H 
#define HLTTRACKINGCODER_H 1

// Include files
#include "Event/Track.h"
#include <vector>

/** @function HltTrackingCoder HltTrackingCoder.h
 *  
 *
 *  @author Sebastian Neubert
 *  @date   2014-01-10
 *
 *  Helper to code / decode Tracks into / from RawBank
 *
 *  Unit tests for these functions are in utestTrackingCoder.cxx 
 *
 *  Encoding of track information into RawBank
 *  For each track:
 *  (nLhHCbIDs, ID0, ID1, ... IDn)
 *   
 *  tracks are just concatenated
 *
 */

void
encodeTracks(const LHCb::Tracks* tracks, std::vector<unsigned int>& rawBank);

unsigned int
// returns number of decoded tracks
decodeTracks(const std::vector<unsigned int>& rawBank, LHCb::Tracks* tracks);


#endif // HLTTRACKINGCODER_H 
