// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/src/RawEventHelpers.cpp,v 1.9 2006-04-18 09:18:30 frankb Exp $
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
#ifdef _WIN32
#include <winsock.h>
#define LITTLE_ENDIAN
#else
#include <netinet/in.h>
#endif

using LHCb::RawEvent;
using LHCb::RawBank;
using LHCb::MEPEvent;
extern "C" void R__zip (int cxlevel, int *nin, const char *bufin, int *lout, char *bufout, int *nout);
extern "C" void R__unzip(int *nin, const char *bufin, int *lout, char *bufout, int *nout);

/// Determine length of the sequential buffer from RawEvent object
size_t LHCb::rawEventLength(const RawEvent* evt)    {
  size_t i, len;
  RawEvent* raw = const_cast<RawEvent*>(evt);
  for(len=0, i=RawBank::L0Calo; i<RawBank::LastType; ++i)  {
    typedef std::vector<RawBank*> _BankV;
    const _BankV& banks = raw->banks(RawBank::BankType(i));
    for(_BankV::const_iterator j=banks.begin(); j != banks.end(); ++j)  {
      len += (*j)->totalSize();
    }
  }
  return len;
}

/// Determine number of banks from rawEvent object
size_t LHCb::numberOfBanks(const RawEvent* evt)   {
  size_t count = 0;
  RawEvent* raw = const_cast<RawEvent*>(evt);
  for(size_t i=RawBank::L0Calo; i<RawBank::LastType; ++i)  {
    count += raw->banks(RawBank::BankType(i)).size();
  }
  return count;
}

/// Determine number of bank types from rawEvent object
size_t LHCb::numberOfBankTypes(const RawEvent* evt) {
  size_t count = 0;
  RawEvent* raw = const_cast<RawEvent*>(evt);
  for(size_t i=RawBank::L0Calo; i<RawBank::LastType; ++i)  {
    if ( !raw->banks(RawBank::BankType(i)).empty() ) count++;
  }
  return count;
}

static unsigned int adler32Checksum(unsigned int old, const unsigned char *buf, size_t len)  {
#define BASE 65521  // largest prime smaller than 65536
  unsigned int s1 = old & 0xffff;
  unsigned int s2 = (old >> 16) & 0xffff;
  for (size_t n = 0; n < len; n++)   {
    s1 = (s1 + buf[n]) % BASE;
    s2 = (s2 + s1) % BASE;
  }
  return( (s2 << 16) + s1 );
} // update_adler32()

static unsigned int xorChecksum(const int* ptr, size_t len)  {
  unsigned int checksum = 0;
  len = len/sizeof(int) + (len%sizeof(int) ? 1 : 0);
  for(const int *p=ptr, *end=p+len; p<end; ++p)  {
    checksum ^= *p;
  }
  return checksum;
}

#define QUOTIENT  0x04c11db7
class CRC32Table  {
public:
  unsigned int m_data[256];
  CRC32Table()  {
    unsigned int crc;
    for (int i = 0; i < 256; i++)    {
      crc = i << 24;
      for (int j = 0; j < 8; j++)   {
        if (crc & 0x80000000)
          crc = (crc << 1) ^ QUOTIENT;
        else
          crc = crc << 1;
      }
      m_data[i] = htonl(crc);
    }
  }
  const unsigned int* data() const { return m_data; }
};

// Only works for word aligned data and assumes that the data is an exact number of words
// Copyright © 1993 Richard Black. All rights are reserved. 
static unsigned int crc32Checksum(const unsigned char *data, size_t len)    {  
  static CRC32Table table;
  const unsigned int *crctab = table.data();
  const unsigned int *p = (const unsigned int *)data;
  const unsigned int *e = (const unsigned int *)(data + len);   
  if ( len < 4 || (size_t(data)%sizeof(unsigned int)) != 0 ) return ~0x0;
  unsigned int result = ~*p++;
  while( p < e )  {
#if defined(LITTLE_ENDIAN)
    result = crctab[result & 0xff] ^ result >> 8;
    result = crctab[result & 0xff] ^ result >> 8;
    result = crctab[result & 0xff] ^ result >> 8;
    result = crctab[result & 0xff] ^ result >> 8;
    result ^= *p++;
#else
    result = crctab[result >> 24] ^ result << 8;
    result = crctab[result >> 24] ^ result << 8;
    result = crctab[result >> 24] ^ result << 8;
    result = crctab[result >> 24] ^ result << 8;
    result ^= *p++;
#endif
  }

  return ~result;
}

static unsigned short crc16Checksum (const unsigned char *data, size_t len) {
  static const unsigned short wCRCTable[] =
  { 0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241,
    0XC601, 0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440,
    0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40,
    0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841,
    0XD801, 0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40,
    0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01, 0X1DC0, 0X1C80, 0XDC41,
    0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 0XD641,
    0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040,
    0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
    0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441,
    0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
    0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840,
    0X2800, 0XE8C1, 0XE981, 0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41,
    0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 0XEC81, 0X2C40,
    0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640,
    0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041,
    0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281, 0X6240,
    0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
    0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41,
    0XAA01, 0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840,
    0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41,
    0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
    0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640,
    0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101, 0X71C0, 0X7080, 0XB041,
    0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 0X5280, 0X9241,
    0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440,
    0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
    0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841,
    0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40,
    0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41,
    0X4400, 0X84C1, 0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641,
    0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040 };
    unsigned char nTemp;
    unsigned short wCRCWord = 0xFFFF;
    while (len--)  {
      nTemp = *data++ ^ wCRCWord;
      wCRCWord >>= 8;
      wCRCWord ^= wCRCTable[nTemp];
    }
    return wCRCWord;
}

static unsigned char crc8Checksum(const unsigned char *data, int len) {
  static unsigned char crc8_table[] = 
  { 0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
    157,195, 33,127,252,162, 64, 30, 95,  1,227,189, 62, 96,130,220,
    35,125,159,193, 66, 28,254,160,225,191, 93,  3,128,222, 60, 98,
    190,224,  2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
    70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89,  7,
    219,133,103, 57,186,228,  6, 88, 25, 71,165,251,120, 38,196,154,
    101, 59,217,135,  4, 90,184,230,167,249, 27, 69,198,152,122, 36,
    248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91,  5,231,185,
    140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
    17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
    175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
    50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
    202,148,118, 40,171,245, 23, 73,  8, 86,180,234,105, 55,213,139,
    87,  9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
    233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
    116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53
  };
  const unsigned char *s = data;
  unsigned char c = 0;
  while (len--) c = crc8_table[c ^ *s++];
  return c;
}

/// Generate XOR Checksum
unsigned int LHCb::genChecksum(int flag,const void* ptr, size_t len)  {
  switch(flag)  {
    case 0:
      return xorChecksum((const int*)ptr, len);
    case 1:
      return crc32Checksum((const unsigned char*)ptr, len);
    case 2:
      return crc16Checksum((const unsigned char*)ptr, len);
    case 3:
      return crc8Checksum((const unsigned char*)ptr, len);
    case 4:
      return adler32Checksum(1, (const unsigned char*)ptr, len);
    default:
      return ~0x0;
  }
}

/// Compress opaque data buffer
/*
  The algorithm applied is the ROOT compression mechanism.
  Option "algtype" is used to specify the compression level:
  compress = 0 objects written to this file will not be compressed.
  compress = 1 minimal compression level but fast.
  ....
  compress = 9 maximal compression level but slow.
*/
StatusCode LHCb::compressBuffer(int           algtype, 
                                char*         tar, 
                                size_t        tar_len, 
                                const char*   src, 
                                size_t        src_len, 
                                size_t&       new_len)  
{
  int in_len, out_len, res_len = 0;
  switch(algtype)  {
    case 0:
      if ( tar_len >= src_len )  {
        new_len = src_len;
        ::memcpy(tar, src, src_len);
        return StatusCode::SUCCESS;
      }
      // target buffer too small
      return StatusCode::FAILURE;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
      in_len = src_len;
      out_len = tar_len;
      ::R__zip(algtype, &in_len, src, &out_len, tar, &res_len);
      if (new_len == 0 || new_len >= src_len) {
        //this happens when the buffer cannot be compressed
        new_len = 0;
        return StatusCode::FAILURE;
      }
      new_len = res_len;
      return StatusCode::SUCCESS;
    default:
      break;
  }
  return StatusCode::FAILURE;
}

/// Decompress opaque data buffer
StatusCode LHCb::decompressBuffer(int           algtype, 
                                  char*         tar, 
                                  size_t        tar_len, 
                                  const char*   src, 
                                  size_t        src_len, 
                                  size_t&       new_len)  
{
  int in_len, out_len, res_len = 0;
  switch(algtype)  {
    case 0:
      if ( tar_len >= src_len )  {
        new_len = src_len;
        ::memcpy(tar, src, src_len);
        return StatusCode::SUCCESS;
      }
      break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
      in_len = src_len;
      out_len = tar_len;
      ::R__unzip(&in_len, src, &out_len, tar, &res_len);
      if ( new_len > 0 )  {
        new_len = res_len;
        return StatusCode::SUCCESS;
      }
      break;
    default:
      break;
  }
  return StatusCode::FAILURE;
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
    start += bank->totalSize();
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
    start += bank->totalSize();
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
    s += bank->totalSize();
  }
  return StatusCode::SUCCESS;
}

/// Copy RawEvent data from the object to sequential buffer
StatusCode LHCb::encodeRawBanks(const RawEvent* evt, char* const data, size_t size)  {
  if ( data )  {
    RawEvent* raw = const_cast<RawEvent*>(evt);
    for(size_t total=0, len=0, i=RawBank::L0Calo; i<RawBank::LastType; ++i)  {
      if(encodeRawBanks(raw->banks(RawBank::BankType(i)), data+total, size-total, &len).isSuccess())  {
        total += len;
        continue;
      }
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

/// Copy RawEvent data from bank vectors to sequential buffer
StatusCode LHCb::encodeRawBanks(const std::vector<RawBank*>& banks, char* const data, size_t size, size_t* length) {
  typedef std::vector<RawBank*> _BankV;
  size_t len = 0;
  for(_BankV::const_iterator j=banks.begin(); j != banks.end(); ++j)  {
    size_t s = (*j)->totalSize();
    if ( size >= (len+s) )  {
      ::memcpy(data+len,*j,s);
      len += s;
      continue;
    }
    return StatusCode::FAILURE;
  }
  if ( length ) *length = len;
  return StatusCode::SUCCESS;
}

/// Conditional decoding of raw buffer from MDF to raw event object
StatusCode LHCb::decodeFragment(const MEPFragment* f, RawEvent* raw)  {
  return decodeRawBanks(f->start(), f->end(), raw);
}

/// Conditional decoding of raw buffer from MDF to vector of bank pointers
StatusCode LHCb::decodeFragment(const MEPFragment* f, std::vector<RawBank*>& raw)  {
  return decodeRawBanks(f->start(), f->end(), raw);
}

/// Check consistency of MEP fragment using magic bank patterns.
bool LHCb::checkFragment(const MEPFragment* f)  {
  bool res = true;
  for(RawBank* b=f->first(); b < f->last(); b=f->next(b))  {
    size_t s = b->totalSize();
    if ( b->magic() != RawBank::MagicPattern )  {
      // Error: Bad magic pattern; needs handling
      printf("Bad magic pattern in Tell1 bank %p: srcID=%d Size:%d [%d] Vsn:%d Magic:%X\n",
        b, b->sourceID(), b->size(), s, b->version(), b->magic());
      res = false;
    }
    else  {
//      printf("Tell1 bank OK %p: srcID=%d Size:%d [%d] Vsn:%d Magic:%X\n",
//        b, b->sourceID(), b->size(), s, b->version(), b->magic());
    }
  }
  return res;
}

/// Copy MEP fragment into opaque data buffer
StatusCode LHCb::encodeFragment(const MEPFragment* f, char* const data, size_t len)  {
  char* ptr = data;
  for(RawBank* b=f->first(); b < f->last(); b=f->next(b))  {
    size_t s = b->totalSize();
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
    size_t s = (*i)->totalSize();
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
        evtlen += (*j)->totalSize();
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
    me->setSize(me->size()+mf->size()+mf->sizeOf()-mf->hdrSize());
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

void LHCb::makeMDFHeader( void* const data, int len, int evtype, int hdrType, 
                          long long trNumber, unsigned int trMask[4],
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

/// read MDF record from input stream 
StatusCode 
LHCb::readMDFrecord(StreamDescriptor& dsc, const StreamDescriptor::Access& con)  {
  dsc.setLength(0);
  if ( con.ioDesc > 0 )  {
    MDFHeader h;
    if ( StreamDescriptor::read(con,&h,sizeof(MDFHeader)) )  {
      if ( h.compression() == 0 )  {
        if ( h.size()+sizeof(MDFHeader) > size_t(dsc.max_length()) )  {
          dsc.allocate(sizeof(MDFHeader) + size_t(h.size()*1.5));
        }
        char* ptr = dsc.data()+sizeof(MDFHeader);
        MDFHeader* hdr = (MDFHeader*)dsc.data();
        *hdr = h;
        if ( StreamDescriptor::read(con,ptr,h.size()) )  {
          dsc.setLength(h.size()+sizeof(MDFHeader));
          return StatusCode::SUCCESS;
        }
      }
      else  {
        char* raw = new char[h.size()];
        size_t new_len = 0, len = 2*h.size()+sizeof(MDFHeader);
        if ( StreamDescriptor::read(con,raw,h.size()) )  {
          for(int i=0; i<4; ++i)  {
            if ( len > size_t(dsc.max_length()) )  {
              dsc.allocate(sizeof(MDFHeader) + size_t(len*1.5));
            }
            char* ptr = dsc.data()+sizeof(MDFHeader);
            if ( decompressBuffer(h.compression(),ptr,dsc.max_length(),raw,h.size(),new_len).isSuccess()) {
              MDFHeader* hdr = (MDFHeader*)dsc.data();
              h.setSize(new_len);
              h.setCompression(0);
              h.setChecksum(0);
              *hdr = h;
              dsc.setLength(new_len+sizeof(MDFHeader));
              delete [] raw;
              return StatusCode::SUCCESS;
            }
            len *= 2;
          }
        }
        delete [] raw;
      }
    }
  }
  return StatusCode::FAILURE;
}

/// read MEP record from input stream 
StatusCode LHCb::readMEPrecord(StreamDescriptor& dsc, const StreamDescriptor::Access& con)  {
  unsigned int len = 0;
  dsc.setLength(0);
  if ( con.ioDesc > 0 )  {
    if ( StreamDescriptor::read(con,&len,sizeof(len)) )  {
      if ( len+sizeof(len) > size_t(dsc.max_length()) )  {
        dsc.allocate(sizeof(len) + size_t(len*1.5));
      }
      MEPEvent* me = (MEPEvent*)dsc.data();
      me->setSize(len);
      if ( StreamDescriptor::read(con,me->first(),len) )  {
        dsc.setLength(len+sizeof(len));
        return StatusCode::SUCCESS;
      }
    }
  }
  return StatusCode::FAILURE;
}
