// $Id: STDAQDefinitions.h,v 1.1.1.1 2007-11-16 16:15:15 mneedham Exp $
#ifndef _STDAQDefinitions_H
#define _STDAQDefinitions_H

//#include <utility>  
//#include <vector>

#include "Event/RawBank.h"


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

  typedef std::pair<STTell1ID,unsigned int> chanPair; 

  typedef unsigned int rawInt;

  // number of bits for adc
  const unsigned int numBitsADC = 8u;
  
  // and a mask
  const unsigned int adcMask = 0xff;

  enum version { v1 = 0, v2 = 1 , v3 = 2 , v4 = 3};

  // n strips per ppx
  const unsigned int nStripPerPPx = 768;

  // n strips per Tell1 boar
  const unsigned int nStripsPerBoard = 3072;

};

#endif // _STDAQDefinitions_H
