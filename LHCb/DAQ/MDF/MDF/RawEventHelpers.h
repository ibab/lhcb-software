// $Id: RawEventHelpers.h,v 1.23 2008-09-25 13:07:46 frankb Exp $
//  ====================================================================
//  MDFIO.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
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
  class MEPMultiFragment;
  class MEPFragment;
  class MDFHeader;
  class MEPEvent;
  typedef std::pair<char*,int> MDFDescriptor;

  unsigned int hash32Checksum(const void* ptr, size_t len); 
  unsigned int adler32Checksum(unsigned int old, const char *buf, size_t len);

  /// Check sanity of raw bank structure
  bool checkRawBank(const RawBank* b, bool throw_exc=true,bool print_cout=false);
  /// Check consistency of raw bank sequence using magic words
  bool checkRawBanks(const char* start, const char* end, bool throw_exc=true,bool print_cout=false);
  /// Check consistency of MEP fragment 
  bool checkFragment (const MEPFragment* f, bool throw_exc=true,bool print_cout=false);
  /// Check consistency of MEP multi event fragment
  bool checkMultiFragment (const MEPMultiFragment* f, bool throw_exc=true,bool print_cout=false);
  /// Check consistency of MEP multi event fragment
  bool checkMEPEvent (const MEPEvent* e, bool throw_exc=true,bool print_cout=false);
  /// Check consistency of MEP multi event fragment
  bool checkMDFRecord(const MDFHeader* h, int opt_len=~0x0, bool throw_exc=true,bool print_cout=false);

  /// Deep copy raw event structure (including baw bank memory - hence heavy)
  StatusCode deepCopyRawEvent(RawEvent* source, RawEvent*& result);
  /// Clone rawevent structure
  StatusCode cloneRawEvent(RawEvent* source, RawEvent*& result);
  /// Determine length of the sequential buffer from RawEvent object
  size_t rawEventLength(const RawEvent* evt);
  /// Determine length of the sequential buffer from vector of raw banks
  size_t rawEventLength(const std::vector<RawBank*>& banks);
   /// Determine length of the sequential buffer from RawEvent object
  size_t rawEventLengthTAE(const std::vector<RawBank*>& banks);
  /// Determine length of the sequential buffer from RawEvent object
  size_t rawEventLengthTAE(const RawEvent* evt);

  /// Determine number of banks from rawEvent object
  size_t numberOfBanks(const RawEvent* evt);
  /// Determine number of bank types from rawEvent object
  size_t numberOfBankTypes(const RawEvent* evt);
  /// Generate XOR Checksum
  unsigned int genChecksum(int flag, const void* ptr, size_t len);
  /// Compress opaque data buffer
  /** The algorithm applied is the ROOT compression mechanism.
    * Option "algtype" is used to specify the compression level:
    * compress = 0 objects written to this file will not be compressed.
    * compress = 1 minimal compression level but fast.
    * ....
    * compress = 9 maximal compression level but slow.
    */
  StatusCode compressBuffer(int algtype, char* tar, size_t tar_len, char* src, size_t src_len, size_t& new_len);
  /// Decompress opaque data buffer using the ROOT (de-)compression mechanism.
  StatusCode decompressBuffer(int algtype, char* tar, size_t tar_len, const char* src, size_t src_len, size_t& new_len);

  /// Copy RawEvent data from the object to sequential buffer
  StatusCode encodeRawBanks(const RawEvent* evt,char* const data, size_t len, bool skip_hdr_bank);
  /// Copy RawEvent data from bank vectors to sequential buffer
  StatusCode encodeRawBanks(const std::vector<RawBank*>& banks, char* const data, 
                            size_t size, bool skip_hdr_bank, size_t* length);
  /// Conditional decoding of raw buffer from MDF to raw event object
  StatusCode decodeRawBanks(const char* start, const char* end, RawEvent* raw);
  /// Conditional decoding of raw buffer from MDF to raw event object. Data are passed as descriptor
  inline StatusCode decodeRawBanks(const MDFDescriptor& data, RawEvent* raw)    {
    return decodeRawBanks(data.first,data.first+data.second,raw);
  }
  /// Conditional decoding of raw buffer from MDF to bank offsets
  StatusCode decodeRawBanks(const char* start, const char* end, int* offsets, int* noffset);
  /// Conditional decoding of raw buffer from MDF to bank offsets. Data are passed as descriptor
  inline StatusCode decodeRawBanks(const MDFDescriptor& data, int* offsets, int* noffset)    {
    return decodeRawBanks(data.first,data.first+data.second,offsets,noffset);
  }
  /// Conditional decoding of raw buffer from MDF to vector of raw banks
  StatusCode decodeRawBanks(const char* start, const char* end, std::vector<RawBank*>& banks);
  /// Conditional decoding of raw buffer from MDF to vector of raw banks. Data are passed as descriptor
  inline StatusCode decodeRawBanks(const MDFDescriptor& data, std::vector<RawBank*>& banks)    {
    return decodeRawBanks(data.first,data.first+data.second,banks);
  }

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
  /// Decode MEP into bank collections
  StatusCode decodeMEP( const MEPEvent* me, 
                        unsigned int&   partitionID, 
                        std::map<unsigned int,std::vector<RawBank*> >& events);
  /// Decode MEP into fragments event by event
  StatusCode decodeMEP2EventFragments(const MEPEvent* me, 
                        unsigned int&   partitionID, 
                        std::map<unsigned int, std::vector<MEPFragment*> >& events);
  /// Decode MEP into banks event by event
  StatusCode decodeMEP2EventBanks(const MEPEvent* me, 
                                  unsigned int&   partitionID, 
                                  std::map<unsigned int, std::vector<RawBank*> >& events);

  /// Decode single fragment into a list of pairs (event id,bank)
  StatusCode decodeFragment2Banks(const MEPFragment* f, 
                                  unsigned int event_id_high,
                                  std::vector<std::pair<unsigned int,RawBank*> >& banks);

  /// Decode multi fragment into a list of pairs (event id,bank)
  StatusCode decodeMultiFragment2Banks( const MEPMultiFragment* mf, 
                                        unsigned int&   partitionID, 
                                        std::vector<std::pair<unsigned int,RawBank*> >& banks);

  /// Decode MEP event into a plain list of pairs (event id,bank)
  StatusCode decodeMEP2Banks( const MEPEvent* me, 
                              unsigned int&   partitionID, 
                              std::vector<std::pair<unsigned int,RawBank*> >& banks);

  /// Decoding of MEP event descriptor and append content to raw event object
  StatusCode decodeDescriptors(const RawEventDescriptor* pAddr, RawEvent* raw);

  /// Read MEP record from input stream 
  StatusCode readMEPrecord(StreamDescriptor& dsc, const StreamDescriptor::Access& con);

  /// Return vector of MEP sub-event names
  std::vector<std::string> buffersMEP(const char* start);

  /// Unpack the buffer specified by the two pointers, and return a vector of pairs: root on TES and RawEvent
  std::vector<std::pair<std::string,RawEvent*> > unpackTAEBuffer(const char* start, const char* end);

  /// Return vector of TAE event names
  std::vector<std::string> buffersTAE(const char* start);

  /// Returns the prefix on TES according to bx number, - is previous, + is next
  std::string rootFromBxOffset(int bxOffset);

  /// Returns the offset of the TAE with respect to the central bx
  int bxOffsetTAE(const std::string& root);

  /// Access to TAE bank (if present)
  RawBank* getTAEBank(const char* start);

  /// Unpacks the buffer given by the start and end pointers, and fill the rawevent structure
  StatusCode unpackTAE(const char* start, const char* end, const std::string& loc, RawEvent* raw);

  /// Unpacks the buffer given by the start and end pointers, and fill the rawevent structure
  StatusCode unpackTAE(const MDFDescriptor& data, const std::string& loc, RawEvent* raw);

  /// Force the event type in ODIN to be a TAE event
  StatusCode change2TAEEvent(RawEvent* raw, int halfWindow);

  /// Check if a given RawEvent structure belongs to a TAE event
  bool isTAERawEvent(RawEvent* raw);

}
#endif // MDF_RAWEVENTHELPERS_H
