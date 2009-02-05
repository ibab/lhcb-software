// $Id: STDAQDefinitions.h,v 1.6 2009-02-05 13:45:55 jluisier Exp $
#ifndef _STDAQDefinitions_H
#define _STDAQDefinitions_H

#include <utility>  

class STTell1ID;

/** @namespace STDAQDefinitions STDAQDefinitions.h  Kernel/STDAQDefinitions.h
 *
 *  Namespace for some useful type definitions and const
 *
 *  @author M.Needham
 *  @date   13/3/2002
*/


namespace STDAQ{

  typedef std::pair<STTell1ID,unsigned int> chanPair; 

  typedef unsigned int rawInt;

  /// number of bits for adc
  const unsigned int numBitsADC = 8u;
  
  /// and a mask
  const unsigned int adcMask = 0xff;

  /// version of the decoding software
  enum version { inValidVersion = -1, v1 = 0, v2 = 1 , v3 = 2 , v4 = 3};

  /// n strips per ppx
  const unsigned int nStripPerPPx = 768u;

  /// n strips per Tell1 boar
  const unsigned int nStripsPerBoard = 3072u;

  /// invalid pcn
  const unsigned int inValidPcn = 200u;


  /// TELL1 links
  const unsigned int noptlinks = 24u;  
  const unsigned int nheaders = 3u;
  const unsigned int nbeetles = 3u;
  const unsigned int nports = 4u;
  const unsigned int nstrips = 32u;
  const unsigned int nBeetlesPerPPx = 6u;
  const unsigned int mask1= 0x000000ff;
  const unsigned int mask2= 0x0000ff00;
  const unsigned int mask3= 0x00ff0000;
  const unsigned int mask4= 0xff000000;

 
  // Non-Zero suppressed specific
  const unsigned int nwordsFull = 896u;
  const unsigned int sizebankFull = nwordsFull*4;

  // Pedestal specific
  const unsigned int nwordsPedestal = 768u; 
  const unsigned int sizebankPedestal = nwordsPedestal*4; 
  
  /// Event Info
  const unsigned int npp = 4u;
  const unsigned int ninfo = 32u;

  //// min size of error bank
  const unsigned int minErrorBankWords = 28u;

  /// max sixr of error bank
  const unsigned int maxErrorBankWords = 56u;

};

#endif // _STDAQDefinitions_H
