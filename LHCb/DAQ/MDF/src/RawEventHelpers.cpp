// $Id: RawEventHelpers.cpp,v 1.49 2010-03-16 22:12:40 frankb Exp $
//  ====================================================================
//  RawEventHelpers.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#include "MDF/RawEventHelpers.h"
#include "MDF/RawEventPrintout.h"
#include "MDF/RawEventDescriptor.h"
#include "MDF/OnlineRunInfo.h"
#include "MDF/MDFHeader.h"
#include "MDF/MEPEvent.h"
#include "Event/RawEvent.h"
#include <stdexcept>
#include <iostream>
#include <cstring> // For memcpy with gcc 4.3
#include <memory> // For memcpy with gcc 4.3

#ifdef _WIN32
#define NOATOM
#define NOGDI
#define NOGDICAPMASKS
#define NOMETAFILE
#define NOMINMAX
#define NOMSG
#define NOOPENFILE
#define NORASTEROPS
#define NOSCROLL
#define NOSOUND
#define NOSYSMETRICS
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOCRYPT
#define NOMCX
#include <winsock.h>
#define LITTLE_ENDIAN
#else
#include <netinet/in.h>
#endif
#include "Event/ODIN.h"

static const char* s_checkLabel = "BankCheck    ERROR  ";

using LHCb::RawEvent;
using LHCb::RawBank;
using LHCb::MEPEvent;
using namespace std;
extern "C" void R__zip (int cxlevel, int *nin, const char *bufin, int *lout, char *bufout, int *nout);
extern "C" void R__unzip(int *nin, const char *bufin, int *lout, char *bufout, int *nout);

namespace LHCb {
  // Decode MEP into banks event by event
  StatusCode RTTC2Current( const MEPEvent* me )  {
    for (MEPMultiFragment* mf = me->first(); mf<me->last(); mf=me->next(mf)) {
      for (MEPFragment* f = mf->first(); f<mf->last(); f=mf->next(f)) {
        const RawBank* l = f->last();
        for(RawBank* b=f->first(); b<l; b=f->next(b)) {
          int s = (b->size()+b->hdrSize())*sizeof(int) - b->hdrSize();
          b->setSize(s);
        }
      }
    }
    return StatusCode::SUCCESS;
  }
}

/// one-at-time hash function
unsigned int LHCb::hash32Checksum(const void* ptr, size_t len) {
  unsigned int hash = 0;
  const char* k = (const char*)ptr;
  for (size_t i=0; i<len; ++i, ++k) {
    hash += *k;
    hash += (hash << 10);
    hash ^= (hash >> 6);
  }
  hash += (hash << 3);
  hash ^= (hash >> 11);
  hash += (hash << 15);
  return hash;
}

/* ========================================================================= */
unsigned int LHCb::adler32Checksum(unsigned int adler,
				   const char* buf,
				   size_t len)
{
#define DO1(buf,i)  {s1 +=(unsigned char)buf[i]; s2 += s1;}
#define DO2(buf,i)  DO1(buf,i); DO1(buf,i+1);
#define DO4(buf,i)  DO2(buf,i); DO2(buf,i+2);
#define DO8(buf,i)  DO4(buf,i); DO4(buf,i+4);
#define DO16(buf)   DO8(buf,0); DO8(buf,8);

  static const unsigned int BASE = 65521;    /* largest prime smaller than 65536 */
  /* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */
  static const unsigned int NMAX = 5550;
  unsigned int s1 = adler & 0xffff;
  unsigned int s2 = (adler >> 16) & 0xffff;
  int k;

  if (buf == NULL) return 1;

  while (len > 0) {
    k = len < NMAX ? (int)len : NMAX;
    len -= k;
    while (k >= 16) {
      DO16(buf);
      buf += 16;
      k -= 16;
    }
    if (k != 0) do {
      s1 += (unsigned char)*buf++;
      s2 += s1;
    } while (--k);
    s1 %= BASE;
    s2 %= BASE;
  }
  unsigned int result = (s2 << 16) | s1;
  return result;
}
/* ========================================================================= */

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
// Copyright  1993 Richard Black. All rights are reserved.
static unsigned int crc32Checksum(const char *data, size_t len)    {
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

static unsigned short crc16Checksum (const char *data, size_t len) {
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
    unsigned int nTemp;
    unsigned short wCRCWord = 0xFFFF;
    while (len--)  {
      nTemp = *data++ ^ wCRCWord;
      wCRCWord >>= 8;
      wCRCWord = (unsigned short)(wCRCWord^wCRCTable[nTemp]);
    }
    return wCRCWord;
}

static char crc8Checksum(const char *data, int len) {
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
  const char *s = data;
  char c = 0;
  while (len--) c = crc8_table[c ^ *s++];
  return c;
}

/// Generate XOR Checksum
unsigned int LHCb::genChecksum(int flag,const void* ptr, size_t len)  {
  switch(flag)  {
    case 0:
      return xorChecksum((const int*)ptr, len);
    case 1:
      return hash32Checksum(ptr, len);
    case 2:
      len = (len/sizeof(int))*sizeof(int);
      return crc32Checksum((const char*)ptr, len);
    case 3:
      len = (len/sizeof(short))*sizeof(short);
      return crc16Checksum((const char*)ptr, len);
    case 4:
      return crc8Checksum((const char*)ptr, len);
    case 5:
      len = (len/sizeof(int))*sizeof(int);
      return adler32Checksum(1, (const char*)ptr, len);
    case 22:  // Old CRC32 (fixed by now)
      return crc32Checksum((const char*)ptr, len);
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
                                char*         src,
                                size_t        src_len,
                                size_t&       new_len)
{
  int in_len, out_len, res_len = 0;
  switch(algtype)  {
    case 0:
      if ( tar == src ) {
        new_len = src_len;
        return StatusCode::SUCCESS;
      }
      else if ( tar != src && tar_len >= src_len )  {
        ::memcpy(tar, src, src_len);
        new_len = src_len;
        return StatusCode::SUCCESS;
      }
      new_len = 0;
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
      if (res_len == 0 || size_t(res_len) >= src_len) {
        //this happens when the buffer cannot be compressed
        res_len = 0;
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
      if ( tar != src && tar_len >= src_len )  {
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
      if ( res_len > 0 )  {
        new_len = res_len;
        return StatusCode::SUCCESS;
      }
      break;
    default:
      break;
  }
  return StatusCode::FAILURE;
}

/// Clone rawevent structure
StatusCode LHCb::cloneRawEvent(RawEvent* source, RawEvent*& result)  {
  if ( source )  {
    std::unique_ptr<RawEvent> raw(new RawEvent());
    for(int i=RawBank::L0Calo; i<RawBank::LastType; ++i) {
      for( const auto& bank : source->banks(RawBank::BankType(i)) )
        raw->adoptBank(bank, false);
    }
    result = raw.release();
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

/// Deep copy raw event structure (including baw bank memory - hence heavy)
StatusCode LHCb::deepCopyRawEvent(RawEvent* source, RawEvent*& result)  {
  if ( !source )  return StatusCode::FAILURE;
  RawEvent* raw = ( result ? result : new RawEvent() );
  for(int i=RawBank::L0Calo; i<RawBank::LastType; ++i) {
    for(const auto& bank : source->banks(RawBank::BankType(i)))
      raw->addBank(bank);
  }
  result = raw;
  return StatusCode::SUCCESS;
}

/// Determine length of the sequential buffer from RawEvent object
size_t LHCb::rawEventLength(const RawEvent* evt)    {
  size_t  len = 0;
  RawEvent* raw = const_cast<RawEvent*>(evt);
  for(size_t i=RawBank::L0Calo; i<RawBank::LastType; ++i)  {
    len += rawEventLength(raw->banks(RawBank::BankType(i)));
  }
  return len;
}

/// Determine length of the sequential buffer from RawEvent object
size_t LHCb::rawEventLength(const vector<RawBank*>& banks)    {
  return std::accumulate( banks.begin(), banks.end(), size_t{0},
                          [](size_t l, const RawBank* bank) {
                              return l+bank->totalSize();
                          });
}

/// Determine length of the sequential buffer from RawEvent object
size_t LHCb::rawEventLengthTAE(const vector<RawBank*>& banks)    {
  return std::accumulate( banks.begin(), banks.end(), size_t{0},
                          [](size_t l, const RawBank* bank) {
    return ( !bank->type() == RawBank::DAQ && bank->version() == DAQ_STATUS_BANK) ?
                    l + bank->totalSize() : l;
                          });
}
/// Determine length of the sequential buffer from RawEvent object
size_t LHCb::rawEventLengthTAE(const RawEvent* evt)    {
  size_t i, len;
  RawEvent* raw = const_cast<RawEvent*>(evt);
  for(len=0, i=RawBank::L0Calo; i<RawBank::LastType; ++i)  {
    len += rawEventLengthTAE(raw->banks(RawBank::BankType(i)));
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

static void print_previous_bank(const RawBank* prev) {
  char txt[255];
  if ( prev == 0 )
    ::sprintf(txt,"%s Bad bank is the first bank in the MEP fragment.",s_checkLabel);
  else
    ::sprintf(txt,"%s Previous (good) bank [%p]: %s",s_checkLabel,
	      (void*)prev,LHCb::RawEventPrintout::bankHeader(prev).c_str());
  cout << txt << endl;
}

/// Check sanity of raw bank structure
bool LHCb::checkRawBank(const RawBank* b, bool throw_exc, bool print_cout)  {
  typedef RawEventPrintout _P;
  // Check bank's magic word
  if ( b->magic() == RawBank::MagicPattern )  {
    // Crude check on bank type
    if ( b->type() < RawBank::LastType )  {
      // Now check source ID range:
      //// TBD !!
      return true;
    }
    char txt1[255];
    ::sprintf(txt1,"%s Unknown Bank type in Tell1 bank %p: %s", s_checkLabel, reinterpret_cast<const void*>(b), _P::bankHeader(b).c_str());
    if ( print_cout ) cout << txt1 << endl;
    if ( throw_exc  ) throw runtime_error(txt1);
    return false;
  }
  // Error: Bad magic pattern; needs handling
  char txt0[255];
  ::sprintf(txt0,"%s Bad magic pattern in Tell1 bank %p: %s", s_checkLabel, reinterpret_cast<const void*>(b), _P::bankHeader(b).c_str());
  if ( print_cout ) cout << txt0 << endl;
  if ( throw_exc  ) throw runtime_error(txt0);
  return false;
}

/// Check consistency of MEP fragment using magic bank patterns.
bool LHCb::checkRawBanks(const char* start, const char* end, bool exc,bool prt)  {
  char txt[255];
  RawBank* prev = 0;
  if ( end >= start )  {
    for(RawBank* b=(RawBank*)start, *e=(RawBank*)end; b < e; b=MEPFragment::next(b))  {
      if ( !checkRawBank(b,false,true) ) goto Error;  // Check bank sanity
      prev = b;
    }
    return true;
  }
Error:  // Anyhow only end up here if no exception was thrown...
  ::sprintf(txt,"%s Error in multi raw bank buffer start:%p end:%p",s_checkLabel,start,end);
  if ( prt ) {
    cout << txt << endl;
    print_previous_bank(prev);
  }
  if ( exc ) throw runtime_error(txt);
  return false;
}

/// Check consistency of MEP fragment
bool LHCb::checkFragment(const MEPFragment* f, bool exc, bool prt)  {
  const char* s = f->start();
  const char* e = f->end();
  if ( s+f->size()-1 == e )  {
    if ( checkRawBanks(s, e, exc, prt) ) return true;
  }
  char txt[255];
  ::sprintf(txt,"%s MEP fragment error at %p: EID_l:%d Size:%ld %ld",
            s_checkLabel,reinterpret_cast<const void*>(f),f->eventID(),long(f->size()),long(e-s));
  if ( prt ) cout << txt << endl;
  if ( exc ) throw runtime_error(txt);
  return false;
}

/// Check consistency of MEP multi fragment
bool LHCb::checkMultiFragment(const MEPMultiFragment* mf, bool exc, bool prt)  {
  char txt[255];
  const MEPFragment* s = mf->first();
  const MEPFragment* e = mf->last();
  size_t siz = mf->size();
  if ( e >= s && mf->start()+siz == mf->end() )  {
    for( ; s < e; s = mf->next(s) )  {
      if ( !checkFragment(s,exc,prt) )  goto Error;
    }
    return true;
  }
Error:  // Anyhow only end up here if no exception was thrown...
  ::sprintf(txt,"%s MEP multi fragment error at %p: EID_l:%u Size:%ld %ld",
            s_checkLabel,reinterpret_cast<const void*>(mf),mf->eventID(),long(siz),long(s-e));
  if ( prt ) cout << txt << endl;
  if ( exc ) throw runtime_error(txt);
  return false;
}

/// Check consistency of MEP multi event
bool LHCb::checkMEPEvent (const MEPEvent* me, bool exc, bool prt)  {
  char txt[255];
  const MEPMultiFragment* s = me->first();
  const MEPMultiFragment* e = me->last();
  if ( e >= s && me->start()+me->size()-1 == me->end() )  {
    for( ; s < e; s = me->next(s) )  {
      if ( !checkMultiFragment(s,exc,prt) ) goto Error;
    }
    return true;
  }
Error:  // Anyhow only end up here if no exception was thrown...
  ::sprintf(txt,"%s MEP event error at %p: Size:%ld",s_checkLabel,reinterpret_cast<const void*>(me),long(me->size()));
  if ( prt ) cout << txt << endl;
  if ( exc ) throw runtime_error(txt);
  return false;
}

/// Check consistency of MEP multi event fragment
bool LHCb::checkMDFRecord(const MDFHeader* h, int opt_len, bool exc,bool prt)    {
  if ( h )  {
    int  compress;
    char txt[255];
    const char *start, *end;
    if ( h->size0() != h->size1() || h->size0() != h->size2() )  {
      ::sprintf(txt,"%s Inconsistent MDF header size: %u <-> %u <-> %u at %p",
		s_checkLabel,h->size0(),h->size1(),h->size2(),reinterpret_cast<const void*>(h));
      goto Error;
    }
    if ( opt_len != ~0x0 && size_t(opt_len) != h->size0() )  {
      ::sprintf(txt,"%s Wrong MDF header size: %u <-> %d at %p",
		s_checkLabel,h->size0(),opt_len,reinterpret_cast<const void*>(h));
      goto Error;
    }
    compress  = h->compression()&0xF;
    if ( compress )  {
      // No uncompressing here! Assume everything is OK.
      return true;
    }
    start = ((char*)h) + sizeof(MDFHeader) + h->subheaderLength();
    end   = ((char*)h) + h->size0();
    if ( !checkRawBanks(start,end,exc,prt) )  {
      ::sprintf(txt,"%s Error in multi raw bank buffer start:%p end:%p",
		s_checkLabel,start,end);
      goto Error;
    }
    return true;

Error:  // Anyhow only end up here if no exception was thrown...
    if ( prt ) cout << txt << endl;
    if ( exc ) throw runtime_error(txt);
  }
  return false;
}

/// Conditional decoding of raw buffer from MDF to raw event object
StatusCode LHCb::decodeRawBanks(const char* start, const char* end, RawEvent* raw) {
  const RawBank* prev = 0;
  try {
    while (start < end)  {
      RawBank* bank = (RawBank*)start;
      checkRawBank(bank,true,true);  // Check bank sanity
      raw->adoptBank(bank, false);
      start += bank->totalSize();
      prev = bank;
    }
    return StatusCode::SUCCESS;
  }
  catch(const exception& e) {
    print_previous_bank(prev);
  }
  catch(...) {
    print_previous_bank(prev);
  }
  throw runtime_error("Error decoding raw banks!");
}

/// Conditional decoding of raw buffer from MDF to vector of raw banks
StatusCode LHCb::decodeRawBanks(const char* start, const char* end, vector<RawBank*>& banks) {
  RawBank *prev = 0, *bank = (RawBank*)start;
  try {
    while (start < end)  {
      bank = (RawBank*)start;
      checkRawBank(bank,true,true);  // Check bank sanity
      banks.push_back(bank);
      start += bank->totalSize();
      prev = bank;
    }
    return StatusCode::SUCCESS;
  }
  catch(const exception& e) {
    print_previous_bank(prev);
  }
  catch(...) {
    print_previous_bank(prev);
  }
  throw runtime_error("Error decoding raw banks!");
}

/// Conditional decoding of raw buffer from MDF to bank offsets
StatusCode LHCb::decodeRawBanks(const char* start, const char* end, int* offsets, int* noffset) {
  const RawBank *prev = 0;
  try {
    const char* s = start;
    *noffset = 0;
    while (s < end)  {
      RawBank* bank = (RawBank*)s;
      checkRawBank(bank,true,true);  // Check bank sanity
      offsets[*noffset] = s-start;
      (*noffset)++;
      s += bank->totalSize();
    }
    return StatusCode::SUCCESS;
  }
  catch(const exception& e) {
    print_previous_bank(prev);
  }
  catch(...) {
    print_previous_bank(prev);
  }
  throw runtime_error("Error decoding raw banks!");
}

/// Copy RawEvent data from the object to sequential buffer
StatusCode LHCb::encodeRawBanks(const RawEvent* evt, char* const data, size_t size, bool skip_hdr_bank)  {
  if ( data )  {
    typedef vector<RawBank*> _BankV;
    RawEvent* raw = const_cast<RawEvent*>(evt);
    size_t total, len, i;
    for(total=0, len=0, i=RawBank::L0Calo; i<RawBank::LastType; ++i)  {
      if ( i != RawBank::DAQ ) {
	const _BankV& b = raw->banks(RawBank::BankType(i));
	if(encodeRawBanks(b,data+total,size-total,skip_hdr_bank,&len).isSuccess())  {
	  total += len;
	  continue;
	}
	return StatusCode::FAILURE;
      }
    }
    return encodeRawBanks(raw->banks(RawBank::DAQ), data+total, size-total, skip_hdr_bank, &len);
  }
  return StatusCode::FAILURE;
}

/// Copy RawEvent data from bank vectors to sequential buffer
StatusCode LHCb::encodeRawBanks(const vector<RawBank*>& banks, char* const data,
                                size_t size, bool skip_hdr_bank, size_t* length) {
  typedef vector<RawBank*> _BankV;
  RawBank* fid = 0;
  size_t len = 0, s;
  for(_BankV::const_iterator j=banks.begin(); j != banks.end(); ++j)  {
    RawBank* b = (*j);
    if ( skip_hdr_bank )  {
      if ( b->type() == RawBank::DAQ )   {
	if ( b->version() == DAQ_STATUS_BANK ) {
	  continue;
	}
      }
    }
    if ( b->type() == RawBank::DAQ )   {
      if ( b->version() == DAQ_FILEID_BANK ) {
	fid = b;
	continue;
      }
    }
    s = b->totalSize();
    if ( size >= (len+s) )  {
      ::memcpy(data+len, b, s);
      len += s;
      continue;
    }
    return StatusCode::FAILURE;
  }
  // ALWAYS attach FID bank at the very end!
  if ( fid )  {
    s = fid->totalSize();
    if ( size >= (len+s) ) {
      ::memcpy(data+len, fid, s);
      len += s;
    }
  }
  if ( length ) *length = len;
  return StatusCode::SUCCESS;
}

/// Conditional decoding of raw buffer from MDF to raw event object
StatusCode LHCb::decodeFragment(const MEPFragment* f, RawEvent* raw)  {
  return decodeRawBanks(f->start(), f->end(), raw);
}

/// Conditional decoding of raw buffer from MDF to vector of bank pointers
StatusCode LHCb::decodeFragment(const MEPFragment* f, vector<RawBank*>& raw)  {
  return decodeRawBanks(f->start(), f->end(), raw);
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
StatusCode LHCb::encodeFragment(const vector<RawBank*>& banks, MEPFragment* f)    {
  RawBank* b = f->first();
  for(vector<RawBank*>::const_iterator i=banks.begin(); i != banks.end(); ++i)  {
    size_t s = (*i)->totalSize();
    ::memcpy(b, *i, s);
    f->setSize(s+f->size());
    b = f->next(b);
  }
  return StatusCode::SUCCESS;
}

/// Encode entire mep from map of events
StatusCode
LHCb::encodeMEP(const map<unsigned int, RawEvent*>& events,
                unsigned int partID,
                void*        alloc_ctxt,
                void*       (*alloc)(void* ctxt, size_t len),
                MEPEvent**   mep_event)
{
  typedef map<unsigned int, RawEvent*> Events;
  typedef vector<RawBank*>             BankV;
  typedef map<unsigned int, BankV >    BankMap;
  typedef map<unsigned int, BankMap >  BankMap2;

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
  unsigned short packing = (unsigned short)events.size();
  MEPEvent* me = new(memory) MEPEvent(0);
  MEPMultiFragment* mf = me->first();
  for(BankMap2::iterator j=m.begin(); j != m.end(); ++j, mf = me->next(mf)) {
    BankMap& bm = (*j).second;
    bool first = true;
    mf->setSize(0);
    mf->setPacking(packing);
    mf->setPartitionID(partID);
    MEPFragment* f = mf->first();
    for(BankMap::iterator l=bm.begin(); l != bm.end(); ++l, f = mf->next(f))   {
      unsigned int eid = (*l).first;
      if ( first ) {
	mf->setEventID(eid);
	first = false;
      }
      f->setEventID((unsigned short)(eid&0xFFFF));
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

// Decode MEP into bank collections
StatusCode LHCb::decodeMEP( const MEPEvent* me,
                            unsigned int&   partitionID,
                            map<unsigned int,vector<RawBank*> >& events)
{
  typedef vector<RawBank*> _Banks;
  unsigned int evID, eid_h = 0, eid_l = 0;
  const RawBank *prev = 0, *l = 0;
  RawBank* curr = 0;
  try {
    for (MEPMultiFragment* mf = me->first(); mf<me->last(); mf=me->next(mf)) {
      partitionID = mf->partitionID();
      eid_h = mf->eventID()&0xFFFF0000;
      prev = 0;
      for (MEPFragment* f = mf->first(); f<mf->last(); f=mf->next(f)) {
	eid_l = f->eventID();
	if ( prev && eid_l==0 ) eid_h = (mf->eventID()+1)&0xFFFF0000;
	evID = eid_h + (eid_l&0xFFFF);
	_Banks& banks = events[evID];
	for(curr=f->first(), l=f->last(); curr<l; curr=f->next(curr)) {
	  checkRawBank(curr,true,true);
	  banks.push_back(curr);
	  prev = curr;
	}
      }
    }
  }
  catch(const exception& e) {
    print_previous_bank(prev);
    throw e;
  }
  catch(...) {
    print_previous_bank(prev);
    throw runtime_error("Unknown error while checking banks.");
  }
  return StatusCode::SUCCESS;
}

// Decode MEP into RawEvents
StatusCode LHCb::decodeMEP( const MEPEvent* me,
                            bool            copyBanks,
                            unsigned int&   partitionID,
                            map<unsigned int, RawEvent*>& events)
{
  typedef vector<RawBank*> _Banks;
  typedef map<unsigned int,_Banks> _Evt;
  _Evt evts;
  StatusCode sc = decodeMEP(me,partitionID, evts);
  if ( sc.isSuccess() )  {
    for(_Evt::iterator i=evts.begin(); i!=evts.end(); ++i) {
      RawEvent* e = events[(*i).first] = new RawEvent();
      _Banks&   b = (*i).second;
      for(_Banks::iterator j=b.begin();j!=b.end(); ++j) {
        e->adoptBank(*j, copyBanks);
      }
    }
  }
  return sc;
}

// Decode MEP into fragments event by event
StatusCode
LHCb::decodeMEP2EventFragments( const MEPEvent* me,
                            unsigned int&   partitionID,
                            map<unsigned int, vector<MEPFragment*> >& events)
{
  unsigned int evID, eid_h = 0, eid_l = 0;
  for (MEPMultiFragment* mf = me->first(); mf<me->last(); mf=me->next(mf)) {
    bool first = true;
    partitionID = mf->partitionID();
    eid_h = mf->eventID()&0xFFFF0000;
    for (MEPFragment* f = mf->first(); f<mf->last(); f=mf->next(f)) {
      if ( !first && eid_l==0 ) {
	eid_h = (mf->eventID()+1)&0xFFFF0000;
      }
      first = false;
      eid_l = f->eventID();
      evID = eid_h + (eid_l&0xFFFF);
      events[evID].push_back(f);
    }
  }
  return StatusCode::SUCCESS;
}

// Decode MEP into banks event by event
StatusCode
LHCb::decodeMEP2EventBanks( const MEPEvent* me,
                            unsigned int&   partitionID,
                            map<unsigned int, vector<RawBank*> >& events)
{
  unsigned int evID, eid_h = 0, eid_l = 0;
  const RawBank *prev = 0;
  try {
    for (MEPMultiFragment* mf = me->first(); mf<me->last(); mf=me->next(mf)) {
      partitionID = mf->partitionID();
      eid_h = mf->eventID()&0xFFFF0000;
      prev = 0;
      for (MEPFragment* f = mf->first(); f<mf->last(); f=mf->next(f)) {
	eid_l = f->eventID();
	if ( prev && eid_l==0 ) eid_h = (mf->eventID()+1)&0xFFFF0000;
	evID = eid_h + (eid_l&0xFFFF);
	vector<LHCb::RawBank*>& banks = events[evID];
	const RawBank* l = f->last();
	for(RawBank* b=f->first(); b<l; b=f->next(b)) {
	  checkRawBank(b,true,true);
	  banks.push_back(b);
	  prev = b;
	}
      }
    }
  }
  catch(const exception& e) {
    print_previous_bank(prev);
    throw e;
  }
  catch(...) {
    print_previous_bank(prev);
    throw runtime_error("Unknown error while checking banks.");
  }
  return StatusCode::SUCCESS;
}

/// Decode single fragment into a list of pairs (event id,bank)
StatusCode
LHCb::decodeFragment2Banks(const MEPFragment* f,
                           unsigned int event_id_high,
                           vector<pair<unsigned int,RawBank*> >& banks)
{
  if ( f )  {
    const RawBank *prev = 0;
    try {
      unsigned int evID = (event_id_high&0xFFFF0000) + (f->eventID()&0xFFFF);
      const RawBank* l = f->last();
      for(RawBank* b=f->first(); b<l; b=f->next(b)) {
	checkRawBank(b,true,true);
	banks.emplace_back(evID,b);
      }
      return StatusCode::SUCCESS;
    }
    catch(const exception& e) {
      print_previous_bank(prev);
      throw e;
    }
    catch(...) {
      print_previous_bank(prev);
      throw runtime_error("Unknown error while checking banks.");
    }
  }
  return StatusCode::FAILURE;
}

/// Decode multi fragment into a list of pairs (event id,bank)
StatusCode
LHCb::decodeMultiFragment2Banks(const MEPMultiFragment* mf,
                                unsigned int&   partitionID,
                                vector<pair<unsigned int,RawBank*> >& banks)
{
  unsigned int eid_h = 0;
  if ( mf )  {
    partitionID = mf->partitionID();
    eid_h = mf->eventID();
    for (MEPFragment* f = mf->first(); f<mf->last(); f=mf->next(f)) {
      if (f != mf->first() && 0 == f->eventID() ) ++eid_h;
      StatusCode sc = decodeFragment2Banks(f,eid_h,banks);
      if ( !sc.isSuccess() )  {
        char txt[132];
        sprintf(txt,"%s Failed to decode MEP fragment at %p",s_checkLabel,reinterpret_cast<const void*>(mf));
        throw runtime_error(txt);
      }
    }
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

// Decode MEP into a list of banks
StatusCode
LHCb::decodeMEP2Banks( const MEPEvent* me,
                       unsigned int&   partitionID,
                       vector<pair<unsigned int,RawBank*> >& banks)
{
  for (MEPMultiFragment* mf = me->first(); mf<me->last(); mf=me->next(mf)) {
    StatusCode sc = decodeMultiFragment2Banks(mf,partitionID,banks);
    if ( !sc.isSuccess() )  {
      char txt[132];
      sprintf(txt,"%s Failed to decode MEP multi fragment at %p",s_checkLabel,reinterpret_cast<void*>(mf));
      throw runtime_error(txt);
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

/// Return vector of MEP sub-event names
vector<string> LHCb::buffersMEP(const char* start) {
  vector<string> result;
  MEPEvent* me = (MEPEvent*)start;
  MEPEvent::Fragment* f = me->first();
  int num_subevt = f->packing();
  for(int i=-((num_subevt-1)/2), n=0; n<num_subevt; ++n,++i)
    result.push_back(rootFromBxOffset(i));
  return result;
}

/// Unpacks the buffer given by the start and end pointers, and return a vector of Raw Events pointers
vector<pair<string,LHCb::RawEvent*> > LHCb::unpackTAEBuffer( const char* start, const char* end ) {
  vector<pair<string,LHCb::RawEvent*> > result;
  RawBank* b = (RawBank*)start;            // Get the first bank in the buffer
  if ( b->type() != RawBank::TAEHeader ) { // Is it the TAE bank?
    RawEvent* full = new RawEvent();       // No: This is a single RawEvent, unpack it
    decodeRawBanks( start, end, full );
    pair<string,LHCb::RawEvent*> alone( "", full );
    result.push_back( alone );
    return result;
  }
  int nBlocks = b->size()/sizeof(int)/3;  // The TAE bank is a vector of triplets
  int* block  = (int*)start;
  block += 2;                             // skip bank header
  start += b->totalSize();                // skip TAE bank
  for ( int nbl = 0; nBlocks > nbl; ++nbl ) {
    int nBx = *block++;
    int off = *block++;
    int len = *block++;
    const char* myBeg = start+off;
    const char* myEnd = myBeg + len;
    RawEvent* evt = new RawEvent();
    decodeRawBanks( myBeg, myEnd, evt );
    result.emplace_back(rootFromBxOffset(nBx),evt);
  }
  return result;
}

/// Access to the TAE bank (if present)
RawBank* LHCb::getTAEBank(const char* start) {
  RawBank* b = (RawBank*)start;              // Get the first bank in the buffer
  if ( b->type() == RawBank::TAEHeader ) {   // Is it the TAE bank?
    return b;
  }
  if ( b->type() == RawBank::DAQ ) {         // Is it the TAE bank?
    start += b->totalSize();                 //
    b = (RawBank*)start;                     // If the first bank is a MDF (DAQ) bank,
  }                                          // then the second bank must be the TAE header
  if ( b->type() == RawBank::TAEHeader ) {   // Is it the TAE bank?
    return b;
  }
  return nullptr;
}

/// Return vector of TAE event names
vector<string> LHCb::buffersTAE(const char* start) {
  vector<string> result;
  RawBank* b = getTAEBank(start);
  if ( b && b->type() == RawBank::TAEHeader ) {   // Is it the TAE bank?
    int nBlocks = b->size()/sizeof(int)/3;   // The TAE bank is a vector of triplets
    const int* block  = b->begin<int>();
    for(int nbl = 0; nBlocks > nbl; ++nbl, block +=3)
      result.push_back(rootFromBxOffset(*block));
  }
  return result;
}

/// Returns the prefix on TES according to bx number, - is previous, + is next
string LHCb::rootFromBxOffset(int bxOffset) {
  if ( 0 == bxOffset )
    return "/Event";
  if ( 0 < bxOffset )
    return string("/Event/Next") + char('0'+bxOffset);
  return string("/Event/Prev") + char('0'-bxOffset);
}

/// Returns the offset of the TAE with respect to the central bx
int LHCb::bxOffsetTAE(const string& root) {
  size_t idx = string::npos;
  if ( (idx=root.find("/Prev")) != string::npos )
    return -(root[idx+5]-'0');
  if ( (idx=root.find("/Next")) != string::npos )
    return root[idx+5]-'0';
  return 0;
}

/// Unpacks the buffer given by the start and end pointers, and return a vector of Raw Events pointers
StatusCode LHCb::unpackTAE(const char* start, const char* end, const string& loc, RawEvent* raw) {
  RawBank* b = getTAEBank(start);
  // cout << "UnpackTAE:" << (void*)b << " " << (void*)start << " " << (void*)(*(int*)start) << endl;
  if ( b ) {   // Is it the TAE bank?
    int bx = bxOffsetTAE(loc);
    if ( -7 <= bx && 7 >= bx )  {
      int nBlocks = b->size()/sizeof(int)/3;   // The TAE bank is a vector of triplets
      const int* block  = b->begin<int>();     // skip bank header
      start = ((char*)b) + b->totalSize();     // skip TAE bank
      for(int nbl = 0; nBlocks > nbl; ++nbl, block +=3 ) {
        if ( *block == bx )  {
          int off = *(++block);
          int len = *(++block);
          return decodeRawBanks(start+off, start+off+len,raw);
        }
      }
    }
    return StatusCode::FAILURE;
  }
  return decodeRawBanks(start,end,raw);
}

/// Unpacks the buffer given by the start and end pointers, and fill the rawevent structure
StatusCode LHCb::unpackTAE(const MDFDescriptor& data, const string& loc, RawEvent* raw)  {
  return unpackTAE(data.first,data.first+data.second,loc,raw);
}

/// Force the event type in ODIN to be a TAE event
StatusCode LHCb::change2TAEEvent(RawEvent* raw, int halfWindow)  {
  StatusCode sc = StatusCode::FAILURE;
  if ( raw ) {
    for(const auto& bank : raw->banks(RawBank::ODIN) ) {
      //bank->begin<OnlineRunInfo>()->triggerType = ODIN::TimingTrigger;
      bank->begin<OnlineRunInfo>()->TAEWindow = halfWindow;
      sc = StatusCode::SUCCESS;
    }
  }
  return sc;
}

/// Check if a given RawEvent structure belongs to a TAE event
bool LHCb::isTAERawEvent(RawEvent* raw)  {
  if ( !raw ) return false;
  //== Check ODIN event type to see if this is TAE
  auto& oBnks = raw->banks(RawBank::ODIN);
  return std::any_of( oBnks.begin(), oBnks.end(), [](const RawBank* bank) {
    return  bank->begin<OnlineRunInfo>()->triggerType == ODIN::TimingTrigger ||
         bank->begin<OnlineRunInfo>()->TAEWindow   != 0              ;
  } );
}
