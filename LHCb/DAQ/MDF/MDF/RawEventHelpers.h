// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/MDF/RawEventHelpers.h,v 1.1 2005-12-20 16:33:38 frankb Exp $
//	====================================================================
//  MDFWriter.h
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#ifndef MDF_RAWEVENTHELPERS_H
#define MDF_RAWEVENTHELPERS_H

#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatusCode.h"
#include <vector>

/*
 *    LHCb namespace
 */
namespace LHCb  {

  // Forward declarations
  class RawBank;
  class RawEvent;
  class RawEventDescriptor;
  class MEPFragment;

  /// Fill MDF header structure in given memory location
  void makeMDFHeader(void* const data, int len, int evtype, int hdrType, 
                     longlong trNumber, unsigned int trMask[4],
                     int compression, int checksum);
  /// Determine length of the sequential buffer from RawEvent object
  size_t rawEventLength(const RawEvent* evt);
  /// Generate XOR Checksum
  unsigned int xorChecksum(const void* ptr, size_t len);
  /// Compress opaque data buffer
  StatusCode compressBuffer(int algtype, char* tar, size_t tar_len, const char* src, size_t src_len, size_t& new_len);
  /// Decompress opaque data buffer
  StatusCode decompressBuffer(int algtype, char* tar, size_t tar_len, const char* src, size_t src_len, size_t& new_len);

  /// Copy RawEvent data from the object to sequential buffer
  StatusCode encodeRawBanks(const RawEvent* evt,char* const data, size_t len);
  /// Conditional decoding of raw buffer from MDF to raw event object
  StatusCode decodeRawBanks(RawEvent* raw, const char* start, const char* end);

  /// Copy MEP fragment into opaque sequential data buffer
  StatusCode encodeFragment(const LHCb::MEPFragment* f, char* const data, size_t len);
  /// Copy array of bank pointers into opaque data buffer
  StatusCode encodeFragment(const std::vector<LHCb::RawBank*>& b, LHCb::MEPFragment* f);
  /// Decoding of MEP event fragment and append content to raw event object
  StatusCode decodeFragment(const LHCb::MEPFragment* f, RawEvent* raw);

  /// Decoding of MEP event descriptor and append content to raw event object
  StatusCode decodeDescriptors(const LHCb::RawEventDescriptor* pAddr, LHCb::RawEvent* raw);


}
#endif // MDF_RAWEVENTHELPERS_H
