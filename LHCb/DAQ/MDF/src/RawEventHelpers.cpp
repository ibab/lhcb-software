// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/src/RawEventHelpers.cpp,v 1.3 2006-01-10 14:00:44 frankb Exp $
//	====================================================================
//  RawEventHelpers.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#include "MDF/RawEventHelpers.h"
#include "MDF/RawEventDescriptor.h"
#include "MDF/MDFHeader.h"
#include "MDF/MEPEvent.h"
#include "Event/RawEvent.h"
#include <stdexcept>

using LHCb::RawEvent;
using LHCb::RawBank;
using LHCb::MEPEvent;

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
StatusCode LHCb::compressBuffer(int        /* algtype */, 
                                char*         tar, 
                                size_t     /* tar_len */, 
                                const char*   src, 
                                size_t        src_len, 
                                size_t&       new_len)  
{
  new_len = src_len;
  ::memcpy(tar, src, src_len);
  return StatusCode::SUCCESS;
}

/// Decompress opaque data buffer
StatusCode LHCb::decompressBuffer(int        /* algtype */, 
                                  char*         tar, 
                                  size_t     /* tar_len */, 
                                  const char*   src, 
                                  size_t        src_len, 
                                  size_t&       new_len)  
{
  new_len = src_len;
  ::memcpy(tar, src, src_len);
  return StatusCode::SUCCESS;
}

/// Conditional decoding of raw buffer from MDF to raw event object
StatusCode LHCb::decodeRawBanks(const char* start, const char* end, RawEvent* raw) {
  while (start < end)  {
    RawBank* bank = (RawBank*)start;
    if ( bank->magic() != RawBank::MagicPattern )  {
      // Error: Bad magic pattern; needs handling
      throw std::runtime_error("Bad magic pattern in Tell1 bank!");
    }
    raw->adoptBank(bank, false);
    start += RawEvent::paddedBankLength(bank->size());
  }
  return StatusCode::SUCCESS;
}

/// Conditional decoding of raw buffer from MDF to vector of raw banks
StatusCode LHCb::decodeRawBanks(const char* start, const char* end, std::vector<RawBank*>& banks) {
  while (start < end)  {
    RawBank* bank = (RawBank*)start;
    if ( bank->magic() != RawBank::MagicPattern )  {
      // Error: Bad magic pattern; needs handling
      throw std::runtime_error("Bad magic pattern in Tell1 bank!");
    }
    banks.push_back(bank);
    start += RawEvent::paddedBankLength(bank->size());
  }
  return StatusCode::SUCCESS;
}

/// Conditional decoding of raw buffer from MDF to bank offsets
StatusCode LHCb::decodeRawBanks(const char* start, const char* end, int* offsets, int* noffset) {
  const char* s = start;
  *noffset = 0;
  while (s < end)  {
    RawBank* bank = (RawBank*)s;
    if ( bank->magic() != RawBank::MagicPattern )  {
      // Error: Bad magic pattern; needs handling
      throw std::runtime_error("Bad magic pattern in Tell1 bank!");
    }
    offsets[*noffset] = s-start;
    (*noffset)++;
    s += RawEvent::paddedBankLength(bank->size());
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
  return decodeRawBanks(f->start(), f->end(), raw);
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
StatusCode LHCb::encodeFragment(const std::vector<RawBank*>& banks, MEPFragment* f)    {
  RawBank* b = f->first();
  for(std::vector<RawBank*>::const_iterator i=banks.begin(); i != banks.end(); ++i)  {
    size_t s = RawEvent::paddedBankLength((*i)->size());
    ::memcpy(b, *i, s);
    f->setSize(s+f->size());
    b = f->next(b);
  }
  return StatusCode::SUCCESS;
}

/// Encode entire mep from map of events
StatusCode
LHCb::encodeMEP(const std::map<unsigned int, RawEvent*>& events, 
                unsigned int partID, 
                void*        alloc_ctxt,
                void*       (*alloc)(void* ctxt, size_t len),
                MEPEvent**   mep_event)
{
  typedef std::map<unsigned int, RawEvent*> Events;
  typedef std::vector<RawBank*>             BankV;
  typedef std::map<unsigned int, BankV >    BankMap;
  typedef std::map<unsigned int, BankMap >  BankMap2;

  size_t evtlen = MEPEvent::sizeOf();
  BankMap2 m;
  for(Events::const_iterator i=events.begin(); i != events.end(); ++i)  {
    unsigned int eid = (*i).first;
    RawEvent*    evt = const_cast<RawEvent*>((*i).second);
    for(size_t t=RawBank::L0Calo; t<RawBank::LastType; ++t)  {
      const BankV& banks = evt->banks(RawBank::BankType(t));
      for(BankV::const_iterator j=banks.begin(); j != banks.end(); ++j)  {
        m[(*j)->sourceID()][eid].push_back((*j));
        evtlen += RawEvent::paddedBankLength((*j)->size());
      }
    }
  }
  evtlen += m.size()*MEPMultiFragment::sizeOf();
  for(BankMap2::const_iterator j=m.begin(); j != m.end(); ++j) {
    evtlen += (*j).second.size()*MEPFragment::sizeOf();
  }
  void* memory = (*alloc)(alloc_ctxt, evtlen);
  int packing = events.size();
  MEPEvent* me = new(memory) MEPEvent(0);
  MEPMultiFragment* mf = me->first();
  for(BankMap2::iterator j=m.begin(); j != m.end(); ++j, mf = me->next(mf)) {
    BankMap& bm = (*j).second;
    mf->setSize(0);
    mf->setPacking(packing);
    mf->setPartitionID(partID);
    MEPFragment* f = mf->first();
    for(BankMap::iterator l=bm.begin(); l != bm.end(); ++l, f = mf->next(f))   {
      unsigned int eid = (*l).first;
      mf->setEventID(eid&0xFFFF0000);
      f->setEventID(eid&0xFFFF);
      f->setSize(0);
      encodeFragment((*l).second, f);
      mf->setSize(mf->size()+f->size()+f->sizeOf());
    }
    me->setSize(me->size()+mf->size()+mf->sizeOf());
    // printf("MF:%p  %p\n",mf,((char*)me)+evtlen);
  }
  *mep_event = me;
  return StatusCode::SUCCESS;
}

// Decode MEP into RawEvents
StatusCode LHCb::decodeMEP( const MEPEvent* me, 
                            bool            copyBanks,
                            unsigned int&   partitionID, 
                            std::map<unsigned int, RawEvent*>& events)
{
  unsigned int evID, eid_h = 0, eid_l = 0;
  for (MEPMultiFragment* mf = me->first(); mf<me->last(); mf=me->next(mf)) {
    partitionID = mf->partitionID();
    eid_h = mf->eventID();
    for (MEPFragment* f = mf->first(); f<mf->last(); f=mf->next(f)) {
      eid_l = f->eventID();
      evID = (eid_h&0xFFFF0000) + (eid_l&0xFFFF);
      RawEvent* evt = events[evID];
      if ( 0 == evt ) events[evID] = evt = new RawEvent();
      const RawBank* l = f->last();
      for(RawBank* b=f->first(); b<l; b=f->next(b)) {
        if ( b->magic() != RawBank::MagicPattern )  {
          throw std::runtime_error("Bad magic word in RawBank!");
        }
        evt->adoptBank(b, copyBanks);
      }
    }
  }
  return StatusCode::SUCCESS;
}

// Decode MEP into fragments event by event
StatusCode 
LHCb::decodeMEP2EventFragments( const MEPEvent* me, 
                            unsigned int&   partitionID, 
                            std::map<unsigned int, std::vector<MEPFragment*> >& events)
{
  typedef std::map<unsigned int, std::vector<MEPFragment*> > Events;
  unsigned int evID, eid_h = 0, eid_l = 0;
  for (MEPMultiFragment* mf = me->first(); mf<me->last(); mf=me->next(mf)) {
    partitionID = mf->partitionID();
    eid_h = mf->eventID();
    for (MEPFragment* f = mf->first(); f<mf->last(); f=mf->next(f)) {
      eid_l = f->eventID();
      evID = (eid_h&0xFFFF0000) + (eid_l&0xFFFF);
      events[evID].push_back(f);
    }
  }
  return StatusCode::SUCCESS;
}

// Decode MEP into banks event by event
StatusCode 
LHCb::decodeMEP2EventBanks( const MEPEvent* me, 
                            unsigned int&   partitionID, 
                            std::map<unsigned int, std::vector<RawBank*> >& events)
{
  typedef std::map<unsigned int, std::vector<MEPFragment*> > Events;
  unsigned int evID, eid_h = 0, eid_l = 0;
  for (MEPMultiFragment* mf = me->first(); mf<me->last(); mf=me->next(mf)) {
    partitionID = mf->partitionID();
    eid_h = mf->eventID();
    for (MEPFragment* f = mf->first(); f<mf->last(); f=mf->next(f)) {
      eid_l = f->eventID();
      evID = (eid_h&0xFFFF0000) + (eid_l&0xFFFF);
      std::vector<LHCb::RawBank*>& banks = events[evID];
      const RawBank* l = f->last();
      for(RawBank* b=f->first(); b<l; b=f->next(b)) {
        if ( b->magic() != RawBank::MagicPattern )  {
          throw std::runtime_error("Bad magic word in RawBank!");
        }
        banks.push_back(b);
      }
    }
  }
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
