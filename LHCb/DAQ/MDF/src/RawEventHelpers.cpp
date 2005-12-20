// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/src/RawEventHelpers.cpp,v 1.1 2005-12-20 16:33:39 frankb Exp $
//	====================================================================
//  RawEventHelpers.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#include "MDF/RawEventDescriptor.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/MDFHeader.h"
#include "Event/RawEvent.h"


using LHCb::RawEvent;
using LHCb::RawBank;

/// Determine length of the sequential buffer from RawEvent object
size_t LHCb::rawEventLength(const RawEvent* evt)    {
  size_t i, len;
  RawEvent* raw = const_cast<RawEvent*>(evt);
  for(len=0, i=RawBank::L0Calo; i<RawBank::LastType; ++i)  {
    typedef std::vector<RawBank*> _BankV;
    const _BankV& banks = raw->banks(RawBank::BankType(i));
    for(_BankV::const_iterator j=banks.begin(); j != banks.end(); ++j)  {
      len += raw->paddedBankLength((*j)->size());
    }
  }
  return len;
}

/// Generate XOR Checksum
unsigned int LHCb::xorChecksum(const void* ptr, size_t len)  {
  unsigned int checksum = 0;
  len = len/sizeof(int) + (len%sizeof(int) ? 1 : 0);
  for(int *p=(int*)ptr, *end=p+len; p<end; ++p)  {
    checksum ^= *p;
  }
  return checksum;
}

/// Compress opaque data buffer
StatusCode LHCb::compressBuffer(int algtype, char* tar, size_t tar_len, const char* src, size_t src_len, size_t& new_len)  {
  new_len = src_len;
  ::memcpy(tar, src, src_len);
  return StatusCode::SUCCESS;
}

/// Decompress opaque data buffer
StatusCode LHCb::decompressBuffer(int algtype, char* tar, size_t tar_len, const char* src, size_t src_len, size_t& new_len)  {
  new_len = src_len;
  ::memcpy(tar, src, src_len);
  return StatusCode::SUCCESS;
}

/// Conditional decoding of raw buffer from MDF to raw event object
StatusCode LHCb::decodeRawBanks(RawEvent* raw, const char* start, const char* end) {
  while (start < end)  {
    RawBank* bank = (RawBank*)start;
    if ( bank->magic() != RawBank::MagicPattern )  {
      // Error: Bad magic pattern; needs handling
      throw std::runtime_error("Bad magic pattern in Tell1 bank!");
    }
    raw->adoptBank(bank, false);
    start += raw->paddedBankLength(bank->size());
  }
  return StatusCode::SUCCESS;
}

/// Copy RawEvent data from the object to sequential buffer
StatusCode LHCb::encodeRawBanks(const RawEvent* evt, char* const data, size_t size)  {
  if ( data )  {
    RawEvent* raw = const_cast<RawEvent*>(evt);
    for(size_t len=0, i=RawBank::L0Calo; i<RawBank::LastType; ++i)  {
      typedef std::vector<RawBank*> _BankV;
      const _BankV& banks = raw->banks(RawBank::BankType(i));
      for(_BankV::const_iterator j=banks.begin(); j != banks.end(); ++j)  {
        size_t s = raw->paddedBankLength((*j)->size());
        if ( size >= (len+s) )  {
          ::memcpy(data+len,*j,s);
          len += s;
        }
        else   {
          return StatusCode::FAILURE;
        }
      }
    }
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

/// Conditional decoding of raw buffer from MDF to raw event object
StatusCode LHCb::decodeFragment(const MEPFragment* f, RawEvent* raw)  {
  return decodeRawBanks(raw, f->start(), f->end());
}

/// Copy MEP fragment into opaque data buffer
StatusCode LHCb::encodeFragment(const MEPFragment* f, char* const data, size_t len)  {
  char* ptr = data;
  for(RawBank* b=f->first(); b < f->last(); b=f->next(b))  {
    size_t s = RawEvent::paddedBankLength(b->size());
    if ( ptr+s > data+len )  {
      return StatusCode::FAILURE;
    }
    ::memcpy(ptr, b, s);
    ptr += s;
  }
  return StatusCode::SUCCESS;
}

/// Copy array of bank pointers into opaque data buffer
StatusCode LHCb::encodeFragment(const std::vector<RawBank*>& b, MEPFragment* f)    {
  unsigned short len = 0;
  for(std::vector<RawBank*>::const_iterator i=b.begin(); i != b.end(); ++i)  {
    size_t s = RawEvent::paddedBankLength((*i)->size());
    ::memcpy(f->start()+len, (*i), s);
    len += s;
  }
  f->setSize(len);
  return StatusCode::SUCCESS;
}

/// Conditional decoding of raw buffer to raw event object
StatusCode LHCb::decodeDescriptors(const RawEventDescriptor* dsc, RawEvent* raw)   {
  for(int i=0, n=dsc->numberOfFragments(); i<n; ++i)   {
    decodeFragment(dsc->fragment(i), raw);
  }
  return StatusCode::SUCCESS;
}

void LHCb::makeMDFHeader(void* const data, int len, int evtype, int hdrType, 
                   longlong trNumber, unsigned int trMask[4],
                   int compression, int checksum)  
{
  MDFHeader* header = (MDFHeader*)data;
  header->setEventType(evtype);
  header->setHeaderVersion(hdrType);
  header->setTriggerNumber(trNumber);
  header->setCompression(compression);
  header->setTriggerMask(trMask);
  header->setSize(len);
  header->setChecksum(checksum);
}
