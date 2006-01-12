// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/MDF/RawEventHelpers.h,v 1.5 2006-01-12 19:09:33 frankb Exp $
//	====================================================================
//  MDFWriter.h
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#ifndef MDF_RAWEVENTHELPERS_H
#define MDF_RAWEVENTHELPERS_H

// Framework include files
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatusCode.h"
#include <map>
#include "MDF/StreamDescriptor.h"
// C++ includes
#include <vector>
#include <map>

/*
 *    LHCb namespace
 */
namespace LHCb  {

  // Forward declarations
  class RawBank;
  class RawEvent;
  class RawEventDescriptor;
  class MEPFragment;
  class MEPEvent;

  /// Fill MDF header structure in given memory location
  void makeMDFHeader(void* const data, int len, int evtype, int hdrType, 
                     longlong trNumber, unsigned int trMask[4],
                     int compression, int checksum);
  /// Determine length of the sequential buffer from RawEvent object
  size_t rawEventLength(const RawEvent* evt);
  /// Determine number of banks from rawEvent object
  size_t numberOfBanks(const RawEvent* evt);
  /// Determine number of bank types from rawEvent object
  size_t numberOfBankTypes(const RawEvent* evt);
  /// Generate XOR Checksum
  unsigned int xorChecksum(const void* ptr, size_t len);
  /// Compress opaque data buffer
  StatusCode compressBuffer(int algtype, char* tar, size_t tar_len, const char* src, size_t src_len, size_t& new_len);
  /// Decompress opaque data buffer
  StatusCode decompressBuffer(int algtype, char* tar, size_t tar_len, const char* src, size_t src_len, size_t& new_len);

  /// Copy RawEvent data from the object to sequential buffer
  StatusCode encodeRawBanks(const RawEvent* evt,char* const data, size_t len);
  /// Conditional decoding of raw buffer from MDF to raw event object
  StatusCode decodeRawBanks(const char* start, const char* end, RawEvent* raw);
  /// Conditional decoding of raw buffer from MDF to bank offsets
  StatusCode decodeRawBanks(const char* start, const char* end, int* offsets, int* noffset);
  /// Conditional decoding of raw buffer from MDF to vector of raw banks
  StatusCode decodeRawBanks(const char* start, const char* end, std::vector<RawBank*>& banks);

  /// Copy MEP fragment into opaque sequential data buffer
  StatusCode encodeFragment(const MEPFragment* f, char* const data, size_t len);
  /// Copy array of bank pointers into opaque data buffer
  StatusCode encodeFragment(const std::vector<RawBank*>& b, MEPFragment* f);
  /// Decoding of MEP event fragment and append content to raw event object
  StatusCode decodeFragment(const MEPFragment* f, RawEvent* raw);
  /// Conditional decoding of raw buffer from MDF to vector of bank pointers
  StatusCode decodeFragment(const MEPFragment* f, std::vector<RawBank*>& raw);

  /// Encode entire mep from map of events
  StatusCode encodeMEP( const std::map<unsigned int, RawEvent*>& events, 
                        unsigned int partID, 
                        void*        alloc_ctxt,
                        void*       (*alloc)(void* ctxt, size_t len),
                        MEPEvent**   mep_event);
  /// Decode MEP into RawEvents
  StatusCode decodeMEP( const MEPEvent* me, 
                        bool            copyBanks,
                        unsigned int&   partID, 
                        std::map<unsigned int, RawEvent*>& events);
  /// Decode MEP into fragments event by event
  StatusCode decodeMEP2EventFragments(const MEPEvent* me, 
                        unsigned int&   partitionID, 
                        std::map<unsigned int, std::vector<MEPFragment*> >& events);
  /// Decode MEP into banks event by event
  StatusCode decodeMEP2EventBanks( const MEPEvent* me, 
                              unsigned int&   partitionID, 
                              std::map<unsigned int, std::vector<RawBank*> >& events);

  /// Decoding of MEP event descriptor and append content to raw event object
  StatusCode decodeDescriptors(const RawEventDescriptor* pAddr, RawEvent* raw);

  /// read MEP record from input stream 
  StatusCode readMEPrecord(StreamDescriptor& dsc, const StreamDescriptor::Access& con);
  /// read MDF record from input stream 
  StatusCode readMDFrecord(StreamDescriptor& dsc, const StreamDescriptor::Access& con);

}
#endif // MDF_RAWEVENTHELPERS_H
