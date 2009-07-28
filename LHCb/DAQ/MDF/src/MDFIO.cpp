// $Id: MDFIO.cpp,v 1.36 2009-07-28 17:36:35 snies Exp $
//  ====================================================================
//  MDFIO.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/MsgStream.h"
#include "MDF/RawEventPrintout.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/RawDataAddress.h"
#include "MDF/OnlineRunInfo.h"
#include "MDF/MDFHeader.h"
#include "MDF/MDFIO.h"
#include "Event/RawEvent.h"

#include <cstring> // For memcpy, memmove with gcc 4.3

using namespace LHCb;

/// Transform file name in presence of catalogs
std::string LHCb::MDFIO::getConnection(const std::string& org_conn)  {
  return org_conn;
}

MDFHeader* LHCb::MDFIO::getHeader()  {
  switch(m_dataType)  {
    case MDF_NONE:    // Pure RawEvent structure with MDF Header encoded as bank
      {
        SmartDataPtr<RawEvent> raw(m_evtSvc,"/Event/DAQ/RawEvent");
        if ( raw )  {
          typedef std::vector<RawBank*> _V;
          const _V& bnks = raw->banks(RawBank::DAQ);
          for(_V::const_iterator i=bnks.begin(); i != bnks.end(); ++i)  {
            RawBank* b = *i;
            if ( b->version() == DAQ_STATUS_BANK )  {
              return (MDFHeader*)b->data();
            }
          }
        }
      }
      break;
    case MDF_RECORDS:   // Ready to write MDF records...
      return (MDFHeader*)getDataFromAddress().first;
    case MDF_BANKS:     // Ready to write banks structure with first bank containing MDF header...
      {
        RawBank* b = (RawBank*)getDataFromAddress().first;
        return (MDFHeader*) (b ? b->data() : 0);
      }
      break;
  }
  return 0;
}

std::pair<const char*,int> LHCb::MDFIO::getDataFromAddress() {
  SmartDataPtr<DataObject> evt(m_evtSvc,"/Event");
  if ( evt )  {
    IRegistry* reg = evt->registry();
    if ( reg )  {
      IOpaqueAddress* padd = reg->address();
      RawDataAddress* pA = dynamic_cast<RawDataAddress*>(padd);
      if ( pA )  {
        return pA->data();
      }
    }
    MsgStream log1(m_msgSvc, m_parent);
    log1 << MSG::ERROR << "[Direct I/O] operations can only be performed on valid raw buffers!" << endmsg;
    return std::pair<const char*,int>(0,0);
  }
  MsgStream log2(m_msgSvc, m_parent);
  log2 << MSG::ERROR << "[Direct I/O] failed to retrieve event object: /Event" << endmsg;
  return std::pair<const char*,int>(0,0);
}

StatusCode LHCb::MDFIO::commitRawBanks(RawEvent*         raw,
                                       RawBank*          hdr_bank,
                                       int               compTyp,
                                       int               chksumTyp,
                                       void* const       ioDesc)
{
  try {
    size_t len = rawEventLength(raw);
    size_t hdrSize = hdr_bank->totalSize();
    MDFDescriptor space = getDataSpace(ioDesc, len);
    if ( space.first )  {
      m_spaceActions++;
      StatusCode sc = encodeRawBanks(raw, space.first+hdrSize, len, true);
      if ( sc.isSuccess() ) {
        sc = writeDataSpace(compTyp, chksumTyp, ioDesc, hdr_bank, space.first, len);
        if ( sc.isSuccess() ) {
          m_writeActions++;
          return sc;
        }
        MsgStream err(m_msgSvc, m_parent);
        err << MSG::ERROR << "Failed write data to output device." << endmsg;
        m_writeErrors++;
        return sc;
      }
      MsgStream err(m_msgSvc, m_parent);
      err << MSG::ERROR << "Failed to encode output banks." << endmsg;
      m_writeErrors++;
      return sc;
    }
    MsgStream log0(m_msgSvc, m_parent);
    log0 << MSG::ERROR << "Failed allocate output space." << endmsg;
    m_spaceErrors++;
    return StatusCode::FAILURE;
  }
  catch(std::exception& e)  {
    MsgStream log(m_msgSvc,m_parent);
    log << MSG::ERROR << "Got exception when writing data:" << e.what() << endmsg;
  }
  catch(...)  {
    MsgStream log(m_msgSvc,m_parent);
    log << MSG::ERROR << "Got unknown exception when writing data." << endmsg;
  }
  m_writeErrors++;
  return StatusCode::FAILURE;  
}

StatusCode 
LHCb::MDFIO::commitRawBanks(int compTyp, int chksumTyp, void* const ioDesc, const std::string& location)
{
  SmartDataPtr<RawEvent> raw(m_evtSvc,location);
  if ( raw )  {
    size_t len;
    bool isTAE = m_forceTAE || isTAERawEvent(raw);   // false in principle...unless TAE
    typedef std::vector<RawBank*> _V;
    if ( !isTAE ) {
      const _V& bnks = raw->banks(RawBank::DAQ);
      for(_V::const_iterator i=bnks.begin(); i != bnks.end(); ++i)  {
        RawBank* b = *i;
        if ( b->version() == DAQ_STATUS_BANK )  {
          return commitRawBanks(raw,b,compTyp,chksumTyp,ioDesc);
        }
      }

      len = rawEventLength(raw);
      const _V& odin = raw->banks(RawBank::ODIN);
      RawBank* hdrBank = createDummyMDFHeader( raw, len );
      if ( odin.empty() )  {
        MsgStream log1(m_msgSvc, m_parent);
        log1 << MSG::INFO << "Adding dummy MDF/DAQ[DAQ_STATUS_BANK] information." << endmsg;
      }
      else   {
        unsigned int trMask[4] = {~0,~0,~0,~0};
        const OnlineRunInfo* odin_info = odin[0]->begin<OnlineRunInfo>();
        MDFHeader::SubHeader inf = hdrBank->begin<MDFHeader>()->subHeader();
        MsgStream log1(m_msgSvc, m_parent);
        log1 << MSG::DEBUG << "Creating MDF/DAQ[DAQ_STATUS_BANK] with ODIN information." << endmsg;
        inf.H1->setTriggerMask(trMask);
        inf.H1->setRunNumber(odin_info->Run);
        inf.H1->setOrbitNumber(odin_info->Orbit);
        inf.H1->setBunchID(odin_info->bunchID);
      }
      raw->adoptBank(hdrBank, true);
      return commitRawBanks(raw,hdrBank,compTyp,chksumTyp,ioDesc);
    }
    //== TAE event. Start scanning the whole TES for previous and next events.
    MsgStream msg(m_msgSvc, m_parent);
    RawEvent privateBank;                         // This holds the two temporary banks
    std::vector<RawEvent*> theRawEvents;          // Keep pointer on found events
    msg << MSG::DEBUG << "Found a TAE event. scan for various BXs" << endmsg;

    // prepare the bank containing information for each BX
    // Maximum +-7 crossings, due to hardware limitation of derandomisers = 15 consecutive BX
    int sizeCtrlBlock = 3 * 15; 
    std::vector<int> ctrlData;
    ctrlData.reserve(sizeCtrlBlock);
    size_t offset = 0;
    RawBank *centerMDF=0;
    for ( int n = -7; 7 >= n; ++n ) {
      std::string loc = rootFromBxOffset(n)+"/"+location;
      SmartDataPtr<RawEvent> rawEvt(m_evtSvc, loc);
      if ( rawEvt ) {
        theRawEvents.push_back( rawEvt );
        ctrlData.push_back( n );               // BX offset
        ctrlData.push_back( offset );          // offset in buffer, after end of this bank 
        if ( centerMDF == 0 || n == 0 ) {
          const _V& bnks = raw->banks(RawBank::DAQ);
          for(_V::const_iterator i=bnks.begin(); i != bnks.end(); ++i)  {
            if ( (*i)->version() == DAQ_STATUS_BANK )  {
              centerMDF = *i;
              break;
            }
          }
        }
        size_t l = rawEventLengthTAE(rawEvt); 
        ctrlData.push_back(l);                 // size of this BX information
        offset += l;
        msg << MSG::DEBUG << "Found RawEvent in " << loc << ", size =" << l << endmsg;
      }
    }
    len = ctrlData.size();
    RawBank* ctrlBank = privateBank.createBank(0,RawBank::TAEHeader,0,sizeof(int)*len,&ctrlData[0]);

    // Create now the complete event header (MDF header) with complete length
    len = offset + ctrlBank->totalSize();
    RawBank* hdrBank = createDummyMDFHeader(&privateBank, len);
    if ( centerMDF ) {
      MDFHeader::SubHeader inf = hdrBank->begin<MDFHeader>()->subHeader();
      MDFHeader::SubHeader center = centerMDF->begin<MDFHeader>()->subHeader();
      inf.H1->setTriggerMask(center.H1->triggerMask());
      inf.H1->setRunNumber(center.H1->runNumber());
      inf.H1->setOrbitNumber(center.H1->orbitNumber());
      inf.H1->setBunchID(center.H1->bunchID());
    }
    len += hdrBank->totalSize();

    // Prepare the input to add these two banks to an output buffer
    std::vector<RawBank*> banks;
    banks.push_back( hdrBank );
    banks.push_back( ctrlBank );

    size_t total = len;
    size_t length= 0;
    MDFDescriptor space = getDataSpace(ioDesc, total);
    if ( space.first )  {
      char* dest = space.first;
      m_spaceActions++;
      if ( encodeRawBanks( banks, dest, len, false, &length ).isSuccess())  {
        dest  += length;
        len   -= length;
      }
      for(unsigned int kk=0; theRawEvents.size() != kk; ++kk ) {
        size_t myLength = ctrlData[3*kk + 2];  // 2 words header, third element for each buffer
        StatusCode sc = encodeRawBanks(theRawEvents[kk], dest, myLength, true);
        if ( !sc.isSuccess() ) {
          msg << MSG::ERROR << "Failed to encode output raw data banks." << endmsg;
          m_spaceErrors++;
          privateBank.removeBank( ctrlBank );
          privateBank.removeBank( hdrBank );
          return sc;
        }
        dest  += myLength;
        len   -= myLength;
      }
      //== here, len should be 0...
      if ( 0 != len ) {
        msg << MSG::ERROR << "Expect length=0, found " << len << " starting from " << total << endmsg;
      }

      StatusCode sc = writeDataSpace(compTyp, chksumTyp, ioDesc, hdrBank, space.first, total);
      if ( !sc.isSuccess() ) {
        msg << MSG::ERROR << "Failed write data to output device." << endmsg;
      }
      msg << MSG::DEBUG << "Wrote TAE event of length:" << total 
        << " bytes from " << theRawEvents.size() << " Bxs" << endmsg;
      privateBank.removeBank( ctrlBank );
      privateBank.removeBank( hdrBank );
      return sc;
    }
    m_spaceErrors++;
    msg << MSG::ERROR << "Failed to get space, size = " << len << endmsg;
    privateBank.removeBank( ctrlBank );
    privateBank.removeBank( hdrBank );
    return StatusCode::FAILURE;
  }
  MsgStream log2(m_msgSvc, m_parent);
  log2 << MSG::ERROR << "Failed to retrieve raw event object at " << location << endmsg;
  return StatusCode::FAILURE;
}

RawBank* LHCb::MDFIO::createDummyMDFHeader( RawEvent* raw, size_t len ) {
  unsigned int trMask[] = {~0,~0,~0,~0};
  RawBank* hdrBank = raw->createBank(0, RawBank::DAQ, DAQ_STATUS_BANK, sizeof(MDFHeader)+sizeof(MDFHeader::Header1), 0);
  MDFHeader* hdr = (MDFHeader*)hdrBank->data();
  hdr->setChecksum(0);
  hdr->setCompression(0);
  hdr->setHeaderVersion(3);
  hdr->setSpare(0);
  hdr->setDataType(MDFHeader::BODY_TYPE_BANKS);
  hdr->setSubheaderLength(sizeof(MDFHeader::Header1));
  hdr->setSize(len);
  MDFHeader::SubHeader h = hdr->subHeader();
  h.H1->setTriggerMask(trMask);
  h.H1->setRunNumber(~0x0);
  h.H1->setOrbitNumber(~0x0);
  h.H1->setBunchID(~0x0);
  return hdrBank;
}

/// Direct I/O with valid existing raw buffers
StatusCode 
LHCb::MDFIO::commitRawBuffer(const void*       data,
                             size_t            len,
                             int               type,
                             int            /* compTyp */,
                             int            /* chksumTyp */,
                             void* const       ioDesc)
{
  StatusCode sc = StatusCode::FAILURE;
  const char* ptr = (const char*)data;
  switch(type)  {
    case MDF_BANKS:  // data buffer with bank structure
      {
        const RawBank* hdr = (const RawBank*)ptr;
        MDFHeader* h = (MDFHeader*)hdr->data();
        int hdrSize  = sizeof(MDFHeader)+h->subheaderLength();
        int bnkSize  = hdr->totalSize();
        writeBuffer(ioDesc, h, hdrSize);
        // Need two write due to RAW bank alignment
        sc = writeBuffer(ioDesc, ptr+bnkSize, len-bnkSize);
        sc.isSuccess() ? ++m_writeActions : ++m_writeErrors;
        return sc;
      }
    case MDF_RECORDS:  // Already ready to write MDF record
      sc = writeBuffer(ioDesc, ptr, len);
      sc.isSuccess() ? ++m_writeActions : ++m_writeErrors;
      return sc;
    default:
      m_writeErrors++;
      break;
  }
  return sc;
}

/// Direct I/O with valid existing raw buffers
StatusCode 
LHCb::MDFIO::commitRawBuffer(int               type,
                             int               compTyp,
                             int               chksumTyp,
                             void* const       ioDesc)
{
  std::pair<const char*,int> data = getDataFromAddress();
  if ( data.first )  {
    return commitRawBuffer(data.first, data.second, type, compTyp, chksumTyp, ioDesc);
  }
  return StatusCode::FAILURE;
}

/// Write MDF record from serialization buffer
StatusCode 
LHCb::MDFIO::writeDataSpace(int           compTyp, 
                            int           chksumTyp, 
                            void* const   ioDesc, 
                            RawBank*      hdr, 
                            char* const   data, 
                            size_t        len)  
{
  MDFHeader* h = (MDFHeader*)hdr->data();
  int    bnkSize = hdr->totalSize();
  size_t newlen  = len-bnkSize;
  int    hdrSize = sizeof(MDFHeader)+h->subheaderLength();
  char*  ptr     = data+bnkSize;
  if ( compTyp )   {
    m_tmp.reserve(len+bnkSize);
    if (compressBuffer(compTyp,m_tmp.data()+bnkSize,newlen,ptr,newlen,newlen).isSuccess())  {
      ptr = m_tmp.data()+bnkSize;
      int cmp = (len-bnkSize)/newlen - 1;
      compTyp = (compTyp&0xF) + ((cmp>0xF ? 0xF : cmp)<<4);
    }
    else  {
      // Bad compression; file uncompressed buffer
      newlen = len-bnkSize;
      compTyp = 0;
      ptr = data+bnkSize;
    }
  }
  int chkSize = newlen+hdrSize-4*sizeof(int);
  if ( m_dataType == MDF_RECORDS )  {
    ptr -= hdrSize;
    ::memmove(ptr, h, hdrSize);
    h = (MDFHeader*)ptr;
    h->setSize(newlen);
    newlen += hdrSize;
  }
  else  {
    ptr -= bnkSize;
    // Careful: if the MDFHeaders are not 32 bit aligned,
    //          the checksum later will be wrong!
    ::memmove(ptr, hdr, bnkSize);
    h = (MDFHeader*)((RawBank*)ptr)->data();
    h->setSize(newlen);
    newlen += bnkSize;
  }
  const char* chk_begin = ((char*)h)+4*sizeof(int);
  h->setCompression(compTyp);
  h->setChecksum(chksumTyp>0 ? genChecksum(chksumTyp,chk_begin,chkSize) : 0);
  StatusCode sc = writeBuffer(ioDesc, ptr, newlen);
  sc.isSuccess() ? ++m_writeActions : ++m_writeErrors;
  return sc;
}

bool LHCb::MDFIO::checkSumOk(int checksum, const char* src, int datSize, bool prt)  {
  if ( checksum )  {
    int chk = genChecksum(1,src,datSize);
    if ( chk != checksum )  {  // Try to fix with old checksum calculation
      int chk2 = genChecksum(22,src,datSize);
      if ( chk2 != checksum )  {
        if ( prt )  {
          MsgStream log(m_msgSvc, m_parent);
          log << MSG::ERROR << "Data corruption. [Invalid checksum] expected:" 
            << std::hex << checksum << " got:" << std::hex << chk << endmsg;
        }
        return false;
      }
    }
  }
  return true;
}

MDFDescriptor 
LHCb::MDFIO::readLegacyBanks(const MDFHeader& h, void* const ioDesc, bool dbg)
{
  int  rawSize    = sizeof(MDFHeader);
  int  checksum   = h.checkSum();
  int  compress   = h.compression()&0xF;
  int  expand     = (h.compression()>>4) + 1;
  bool velo_patch = h.subheaderLength()==sizeof(int);
  int  datSize    = h.size()+h.sizeOf(velo_patch ? 0 : 1);
  int  hdrSize    = velo_patch ? sizeof(MDFHeader::Header0)-2 : h.subheaderLength()-2;
  int  readSize   = hdrSize+datSize;
  int  alloc_len  = rawSize+readSize + (compress ? expand*readSize : 0);
  if ( dbg )  {
    MsgStream log(m_msgSvc, m_parent);
    if ( velo_patch )
      log << MSG::INFO << "Velo testbeam data - Compression:" << compress << " Checksum:" << (checksum != 0) <<  endmsg;
    else if ( h.headerVersion() == 1 )
      log << MSG::INFO << "Legacy format[1] - Compression:" << compress << " Checksum:" << (checksum != 0) <<  endmsg;
  }
  // accomodate for potential padding of MDF header bank!
  MDFDescriptor space = getDataSpace(ioDesc, alloc_len+sizeof(int)+sizeof(RawBank));
  char* data = space.first;
  if ( !data )  {
    m_spaceErrors++;
  }
  else {
    m_spaceActions++;
    RawBank* b = (RawBank*)data;
    b->setMagic();
    b->setType(RawBank::DAQ);
    b->setSize(rawSize+hdrSize);
    b->setVersion(DAQ_STATUS_BANK);
    b->setSourceID(0);
    int bnkSize = b->totalSize();
    ::memcpy(b->data(), &h, rawSize);
    char* bptr = (char*)b->data();
    MDFHeader* hdr = (MDFHeader*)bptr;
    if ( compress != 0 )  {
      m_tmp.reserve(readSize);
      size_t new_len = 0;
      StatusCode sc = readBuffer(ioDesc, m_tmp.data(), readSize);
      if ( sc.isSuccess() )  {
        const char* src = m_tmp.data();
        hdr->setHeaderVersion(3);
        hdr->setDataType(MDFHeader::BODY_TYPE_BANKS);
        hdr->setSubheaderLength(sizeof(MDFHeader::Header1));
        hdr->setSize(datSize);
        src += hdrSize;
        char* ptr = ((char*)data) + bnkSize;
        size_t space_size = space.second - bnkSize;
        if ( m_ignoreChecksum )  {
          hdr->setChecksum(0);
        }
        else if ( !checkSumOk(checksum,src,datSize,true) )  {
          return MDFDescriptor(0,-1);
        }
        if ( decompressBuffer(compress,ptr,space_size,src,datSize,new_len).isSuccess()) {
          hdr->setHeaderVersion(3);
          hdr->setSize(new_len);
          hdr->setCompression(0);
          hdr->setChecksum(0);
          if ( h.headerVersion() == 1 )  {
            hdr->setDataType(0);
            hdr->setSpare(0);
          }
          return std::pair<char*, int>(data,bnkSize+new_len);
        }
        MsgStream log0(m_msgSvc, m_parent);
        log0 << MSG::ERROR << "Cannot allocate sufficient space for decompression." << endmsg;
        return MDFDescriptor(0,-1);
      }
      else if ( sc == 0 ) {
        MsgStream log(m_msgSvc, m_parent);
        log << MSG::INFO << "Cannot read more data. End-of-File reached." << endmsg;
        return MDFDescriptor(0,-1);
      }

      MsgStream log1(m_msgSvc, m_parent);
      log1 << MSG::ERROR << "Cannot read " << readSize << " bytes of compressed data." << endmsg;
      return MDFDescriptor(0,-1);
    }
    // Read uncompressed data file...
    int off = bnkSize - hdrSize;
    StatusCode sc = readBuffer(ioDesc, data+off, readSize);
    if ( sc.isSuccess() )  {
      if ( m_ignoreChecksum )  {
        hdr->setChecksum(0);
      }
      else if ( !checkSumOk(checksum,data+off+hdrSize,datSize,true) )  {
        return MDFDescriptor(0,-1);
      }
      off -= rawSize + b->hdrSize();
      if ( off != 0 )  {
        ::memmove(bptr+rawSize, bptr+rawSize+off, hdrSize);
      }
      if ( velo_patch )  {
        // Fix for intermediate VELO data
        MDFHeader::Header0* h0 = hdr->subHeader0().H0;
        if ( h0->triggerMask()[0] != 0x103 )  {
          MsgStream log(m_msgSvc, m_parent);
          log << MSG::ERROR << "Data corruption. [Velo_Patch]....expect trouble!!!" << endmsg;
        }
        h0->setEventType(hdr->hdr());
        hdr->setSubheaderLength(sizeof(MDFHeader::Header0));
        hdr->setHeaderVersion(0);
      }
      else if ( h.headerVersion() == 1 )  {
        hdr->setHeaderVersion(3);
        hdr->setDataType(0);
        hdr->setSpare(0);
      }
      return std::pair<char*, int>(data, bnkSize+datSize);
    }
    else if ( sc == 0 ) {
      MsgStream log(m_msgSvc, m_parent);
      log << MSG::INFO << "Cannot read more data. End-of-File reached." << endmsg;
      return MDFDescriptor(0,-1);
    }

    MsgStream log2(m_msgSvc, m_parent);
    log2 << MSG::ERROR << "Cannot allocate buffer to read:" << readSize << " bytes "  << endmsg;
    return MDFDescriptor(0,-1);
  }
  MsgStream log3(m_msgSvc, m_parent);
  log3 << MSG::ERROR << "Cannot read " << readSize << " bytes of uncompressed data." << endmsg;
  return MDFDescriptor(0,-1);
}

MDFDescriptor 
LHCb::MDFIO::readBanks(const MDFHeader& h, void* const ioDesc, bool dbg)  {
  int rawSize   = sizeof(MDFHeader);
  unsigned int checksum  = h.checkSum();
  int compress  = h.compression()&0xF;
  int expand    = (h.compression()>>4) + 1;
  int hdrSize   = h.subheaderLength();
  int readSize  = h.recordSize()-rawSize;
  int chkSize   = h.recordSize()-4*sizeof(int);
  int alloc_len = rawSize+readSize+sizeof(MDFHeader)+sizeof(RawBank)+
    sizeof(int) + (compress ? expand*readSize : 0);
  if ( dbg )  {
    MsgStream log(m_msgSvc, m_parent);
    log << MSG::INFO << "Compression:" << compress << " Checksum:" << (checksum != 0) <<  endmsg;
  }
  // accomodate for potential padding of MDF header bank!
  MDFDescriptor space = getDataSpace(ioDesc, alloc_len+sizeof(int)+sizeof(RawBank));
  char* data = space.first;
  if ( !data )  {
    m_spaceErrors++;
  }
  else {
    m_spaceActions++;
    RawBank* b = (RawBank*)data;
    b->setMagic();
    b->setType(RawBank::DAQ);
    b->setSize(rawSize+hdrSize);
    b->setVersion(DAQ_STATUS_BANK);
    b->setSourceID(0);
    int bnkSize = b->totalSize();
    ::memcpy(b->data(), &h, rawSize);
    char* bptr = (char*)b->data();
    MDFHeader* hdr = (MDFHeader*)bptr;
    if ( compress != 0 )  {
      m_tmp.reserve(readSize+rawSize);
      ::memcpy(m_tmp.data(),&h,rawSize); // Need to copy header to get checksum right
      StatusCode sc = readBuffer(ioDesc,m_tmp.data()+rawSize,readSize);
      if ( sc.isSuccess() )  {
        int space_retry = 0;
        while ( space_retry++ < 5 ) {
          if ( space_retry>1 ) {
            MsgStream log(m_msgSvc, m_parent);
            alloc_len *= 2;
            log << MSG::INFO << "Retry with increased buffer space of " << alloc_len << " bytes." << endmsg;
            space = getDataSpace(ioDesc, alloc_len+sizeof(int)+sizeof(RawBank));
            data = space.first;
            if ( !data )  {
              m_spaceErrors++;
              goto NoSpace;
            }
            m_spaceActions++;
            b = (RawBank*)data;
            b->setMagic(); 
            b->setType(RawBank::DAQ);
            b->setSize(rawSize+hdrSize);
            b->setVersion(DAQ_STATUS_BANK);
            b->setSourceID(0);
            bnkSize = b->totalSize();
            ::memcpy(b->data(), &h, rawSize);
            bptr = (char*)b->data();
            hdr = (MDFHeader*)bptr;
          }
          ::memcpy(bptr+rawSize, m_tmp.data()+rawSize, hdrSize); 
          if ( m_ignoreChecksum )  {
            hdr->setChecksum(0);
          }
          else if ( checksum && checksum != genChecksum(1,m_tmp.data()+4*sizeof(int),chkSize) )  {
            return MDFDescriptor(0,-1);
          }
          // Checksum is correct...from all we know data integrity is proven
          size_t new_len = 0;
          const char* src = m_tmp.data()+rawSize;
          char* ptr = ((char*)data) + bnkSize;
          size_t space_size = space.second - bnkSize;
          if ( decompressBuffer(compress,ptr,space_size,src+hdrSize,h.size(),new_len).isSuccess()) {
            hdr->setSize(new_len);
            hdr->setCompression(0);
            hdr->setChecksum(0);
            return std::pair<char*, int>(data,bnkSize+new_len);
          }
          ++space_retry;
        }
NoSpace:
        MsgStream log0(m_msgSvc, m_parent);
        log0 << MSG::ERROR << "Cannot allocate sufficient space for decompression." << endmsg;
        return MDFDescriptor(0,-1);
      }
      else if ( sc == 0 ) {
        MsgStream log(m_msgSvc, m_parent);
        log << MSG::INFO << "Cannot read more data. End-of-File reached." << endmsg;
        return MDFDescriptor(0,-1);
      }
      MsgStream log1(m_msgSvc, m_parent);
      log1 << MSG::ERROR << "Cannot read " << readSize << " bytes of compressed data." << endmsg;
      return MDFDescriptor(0,-1);
    }
    // Read uncompressed data file...
    StatusCode sc = readBuffer(ioDesc, bptr+rawSize,readSize);
    if ( sc.isSuccess() )  {
      if ( m_ignoreChecksum )  {
        hdr->setChecksum(0);
      }
      else if ( checksum && checksum != genChecksum(1,bptr+4*sizeof(int),chkSize) )  {
        return MDFDescriptor(0,-1);
      }
      return std::pair<char*, int>(data, bnkSize+h.size());
    }
    else if ( sc == 0 ) {
      MsgStream log(m_msgSvc, m_parent);
      log << MSG::INFO << "Cannot read more data. End-of-File reached." << endmsg;
      return MDFDescriptor(0,-1);
    }
    MsgStream log2(m_msgSvc, m_parent);
    log2 << MSG::ERROR << "Cannot allocate buffer to read:" << readSize << " bytes "  << endmsg;
    return MDFDescriptor(0,-1);
  }
  MsgStream log3(m_msgSvc, m_parent);
  log3 << MSG::ERROR << "Cannot read " << readSize << " bytes of uncompressed data." << endmsg;
  return MDFDescriptor(0,-1);
}

MDFDescriptor LHCb::MDFIO::readBanks(void* const ioDesc, bool dbg)   {
  MDFHeader h;
  int rawSize = sizeof(MDFHeader);
  StatusCode sc = readBuffer(ioDesc, &h, rawSize);
  if ( sc.isSuccess() )  {
    bool velo_patch = h.subheaderLength()==sizeof(int);
    bool vsn1_hdr   = !velo_patch && h.headerVersion()==1;
    if ( velo_patch || vsn1_hdr )  {
      return readLegacyBanks(h,ioDesc,dbg);
    }
    return readBanks(h,ioDesc,dbg);
  }
  else if ( sc == 0 ) {
    MsgStream log(m_msgSvc, m_parent);
    log << MSG::INFO << "Cannot read more data. End-of-File reached." << endmsg;
    return MDFDescriptor(0,-1);
  }
  MsgStream log(m_msgSvc, m_parent);
  log << MSG::DEBUG << "Cannot read " << rawSize << " bytes of header data." << endmsg;
  return MDFDescriptor(0,-1);
}

/// Read raw char buffer from input stream
StatusCode 
LHCb::MDFIO::readBuffer(void* const /* ioDesc */, void* const /* data */, size_t /* len */)  {
  throw std::runtime_error("LHCb::MDFIO::readBuffer: "\
    "This is a default implementation which should never be called!");
  return StatusCode::FAILURE;
}

StatusCode 
LHCb::MDFIO::writeBuffer(void* const /* ioDesc */, const void* /* data */, size_t /* len */)  {
  throw std::runtime_error("LHCb::MDFIO::writeBuffer: "\
    "This is a default implementation which should never be called!");
  return StatusCode::FAILURE;
}

// Pass raw banks to RawEvent object
StatusCode LHCb::MDFIO::adoptBanks(RawEvent* evt,
                                   const std::vector<RawBank*>& bnks,
                                   bool copy_banks)  
{
  if ( evt )  {
    // MsgStream log(m_msgSvc, m_parent);
    for(std::vector<RawBank*>::const_iterator k=bnks.begin(); k!=bnks.end(); ++k) {
      // log << MSG::DEBUG << "Adopt bank:" << RawEventPrintout::bankHeader(*k) << endmsg;
      evt->adoptBank(*k, copy_banks);
    }
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

