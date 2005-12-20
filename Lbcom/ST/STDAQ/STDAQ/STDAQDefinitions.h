// $Id: STDAQDefinitions.h,v 1.1.1.1 2005-12-20 12:47:27 mneedham Exp $
#ifndef _STDAQDefinitions_H
#define _STDAQDefinitions_H

#include <utility>  
#include <vector>

#include "Event/RawBank.h"

//#include "STDAQChannelID.h"
//#include "STTell1ID.h"

class STDAQChannelID;
class STTell1ID;
class ITCluster;

/** @namespace STDAQDefinitions STDAQDefinitions.h  STDAQ/STDefinitions
 *
 *  Namespace for some useful type definitions and const
 *
 *  @author M.Needham
 *  @date   13/3/2002
*/


namespace STDAQ{

  typedef std::pair<STTell1ID,STDAQChannelID> chanPair; 

  typedef unsigned int rawInt;

  // number of bits for adc
  const unsigned int numBitsADC = 8u;
  
  // and a mask
  const unsigned int adcMask = 0xff;

  enum version { v1 = 0, v2 = 1 , v3 = 2};

  // n strips per ppx
  const unsigned int nStripPerPPx = 768;

};

#endif // _STDAQDefinitions_H
