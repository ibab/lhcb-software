// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/src/MDFIO.cpp,v 1.15 2007-03-23 18:54:45 frankb Exp $
//	====================================================================
//  MDFIO.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
//#include "GaudiKernel/IFileCatalogSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/MsgStream.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/RawDataAddress.h"
#include "MDF/MDFHeader.h"
#include "MDF/MDFIO.h"
#include "Event/RawEvent.h"

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

StatusCode 
LHCb::MDFIO::commitRawBanks(RawEvent*         raw,
                            RawBank*          hdr_bank,
                            int               compTyp,
                            int               chksumTyp,
                            void* const       ioDesc)
{
  size_t len = rawEventLength(raw);
  size_t hdrSize = hdr_bank->totalSize();
  std::pair<char*,int> space = getDataSpace(ioDesc, len);
  if ( space.first )  {
    encodeRawBanks(raw, space.first+hdrSize, len, true);
    StatusCode sc = 
      writeDataSpace(compTyp, chksumTyp, ioDesc, hdr_bank, space.first, len);
    if ( sc.isSuccess() ) {
      return sc;
    }
    MsgStream err(m_msgSvc, m_parent);
    err << MSG::ERROR << "Failed write data to output device." << endmsg;
    return sc;
  }
  MsgStream log0(m_msgSvc, m_parent);
  log0 << MSG::ERROR << "Failed allocate output space." << endmsg;
  return StatusCode::FAILURE;
}

StatusCode LHCb::MDFIO::commitRawBanks(int compTyp, int chksumTyp, void* const ioDesc)
{
  SmartDataPtr<RawEvent> raw(m_evtSvc,"/Event/DAQ/RawEvent");
  if ( raw )  {
    typedef std::vector<RawBank*> _V;
    const _V& bnks = raw->banks(RawBank::DAQ);
    for(_V::const_iterator i=bnks.begin(); i != bnks.end(); ++i)  {
      RawBank* b = *i;
      if ( b->version() == DAQ_STATUS_BANK )  {
        return commitRawBanks(raw,b,compTyp,chksumTyp,ioDesc);
      }
    }
    MsgStream log1(m_msgSvc, m_parent);
    unsigned int trMask[] = {~0,~0,~0,~0};
    size_t len = rawEventLength(raw);
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
    raw->adoptBank(hdrBank, true);
    log1 << MSG::INFO << "Adding dummy MDF/DAQ[DAQ_STATUS_BANK] information." << endmsg;
    return commitRawBanks(raw,hdrBank,compTyp,chksumTyp,ioDesc);
    // log1 << MSG::ERROR << "Failed to access MDF header information." << endmsg;
    // return StatusCode::FAILURE;
  }
  MsgStream log2(m_msgSvc, m_parent);
  log2 << MSG::ERROR << "Failed to retrieve raw event object:"
       << "/Event/DAQ/RawEvent" << endmsg;
  return StatusCode::FAILURE;
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
  const char* ptr = (const char*)data;
  switch(type)  {
    case MDF_BANKS:  // data buffer with bank structure
      {
        const RawBank* hdr = (const RawBank*)ptr;
        MDFHeader* h = (MDFHeader*)hdr->data();
        int hdrSize  = sizeof(MDFHeader)+h->subheaderLength();
        int bnkSize  = hdr->totalSize();
        writeBuffer(ioDesc, h, hdrSize);
        return writeBuffer(ioDesc, ptr+bnkSize, len-bnkSize);
      }
    case MDF_RECORDS:  // Already ready to write MDF record
      return writeBuffer(ioDesc, ptr, len);
    default:
      break;
  }
  return StatusCode::FAILURE;
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
  return writeBuffer(ioDesc, ptr, newlen);
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

std::pair<char*,int> 
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
  std::pair<char*,int> space = getDataSpace(ioDesc, alloc_len+sizeof(int)+sizeof(RawBank));
  char* data = space.first;
  if ( data )  {
    RawBank* b = (RawBank*)data;
    b->setMagic();
    b->setType(RawBank::DAQ);
    b->setSize(rawSize+hdrSize);
    b->setVersion(DAQ_STATUS_BANK);
    int bnkSize = b->totalSize();
    ::memcpy(b->data(), &h, rawSize);
    char* bptr = (char*)b->data();
    MDFHeader* hdr = (MDFHeader*)bptr;
    if ( compress != 0 )  {
      m_tmp.reserve(readSize);
      size_t new_len = 0;
      if ( readBuffer(ioDesc, m_tmp.data(), readSize).isSuccess() )  {
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
          return std::pair<char*,int>(0,-1);
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
        return std::pair<char*,int>(0,-1);
      }
      MsgStream log1(m_msgSvc, m_parent);
      log1 << MSG::ERROR << "Cannot read " << readSize << " bytes of compressed data." << endmsg;
      return std::pair<char*,int>(0,-1);
    }
    // Read uncompressed data file...
    int off = bnkSize - hdrSize;
    if ( readBuffer(ioDesc, data+off, readSize).isSuccess() )  {
      if ( m_ignoreChecksum )  {
        hdr->setChecksum(0);
      }
      else if ( !checkSumOk(checksum,data+off+hdrSize,datSize,true) )  {
        return std::pair<char*,int>(0,-1);
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
    MsgStream log2(m_msgSvc, m_parent);
    log2 << MSG::ERROR << "Cannot allocate buffer to read:" << readSize << " bytes "  << endmsg;
    return std::pair<char*,int>(0,-1);
  }
  MsgStream log3(m_msgSvc, m_parent);
  log3 << MSG::ERROR << "Cannot read " << readSize << " bytes of uncompressed data." << endmsg;
  return std::pair<char*,int>(0,-1);
}

std::pair<char*,int> 
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
  std::pair<char*,int> space = getDataSpace(ioDesc, alloc_len+sizeof(int)+sizeof(RawBank));
  char* data = space.first;
  if ( data )  {
    RawBank* b = (RawBank*)data;
    b->setMagic();
    b->setType(RawBank::DAQ);
    b->setSize(rawSize+hdrSize);
    b->setVersion(DAQ_STATUS_BANK);
    int bnkSize = b->totalSize();
    ::memcpy(b->data(), &h, rawSize);
    char* bptr = (char*)b->data();
    MDFHeader* hdr = (MDFHeader*)bptr;
    if ( compress != 0 )  {
      m_tmp.reserve(readSize+rawSize);
      ::memcpy(m_tmp.data(),&h,rawSize); // Need to copy header to get checksum right
      if ( readBuffer(ioDesc,m_tmp.data()+rawSize,readSize).isSuccess() )  {
        ::memcpy(bptr+rawSize, m_tmp.data()+rawSize, hdrSize);
        if ( m_ignoreChecksum )  {
          hdr->setChecksum(0);
        }
        else if ( checksum && checksum != genChecksum(1,m_tmp.data()+4*sizeof(int),chkSize) )  {
          return std::pair<char*,int>(0,-1);
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
        MsgStream log0(m_msgSvc, m_parent);
        log0 << MSG::ERROR << "Cannot allocate sufficient space for decompression." << endmsg;
        return std::pair<char*,int>(0,-1);
      }
      MsgStream log1(m_msgSvc, m_parent);
      log1 << MSG::ERROR << "Cannot read " << readSize << " bytes of compressed data." << endmsg;
      return std::pair<char*,int>(0,-1);
    }
    // Read uncompressed data file...
    if ( readBuffer(ioDesc, bptr+rawSize,readSize).isSuccess() )  {
      if ( m_ignoreChecksum )  {
        hdr->setChecksum(0);
      }
      else if ( checksum && checksum != genChecksum(1,bptr+4*sizeof(int),chkSize) )  {
        return std::pair<char*,int>(0,-1);
      }
      return std::pair<char*, int>(data, bnkSize+h.size());
    }
    MsgStream log2(m_msgSvc, m_parent);
    log2 << MSG::ERROR << "Cannot allocate buffer to read:" << readSize << " bytes "  << endmsg;
    return std::pair<char*,int>(0,-1);
  }
  MsgStream log3(m_msgSvc, m_parent);
  log3 << MSG::ERROR << "Cannot read " << readSize << " bytes of uncompressed data." << endmsg;
  return std::pair<char*,int>(0,-1);
}

std::pair<char*,int> 
LHCb::MDFIO::readBanks(void* const ioDesc, bool dbg)   {
  MDFHeader h;
  int rawSize = sizeof(MDFHeader);
  if ( readBuffer(ioDesc, &h, rawSize).isSuccess() )  {
    bool velo_patch = h.subheaderLength()==sizeof(int);
    bool vsn1_hdr   = !velo_patch && h.headerVersion()==1;
    if ( velo_patch || vsn1_hdr )  {
      return readLegacyBanks(h,ioDesc,dbg);
    }
    return readBanks(h,ioDesc,dbg);
  }
  MsgStream log4(m_msgSvc, m_parent);
  log4 << MSG::ERROR << "Cannot read " << rawSize << " bytes of header data." << endmsg;
  return std::pair<char*,int>(0,-1);
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
    for(std::vector<RawBank*>::const_iterator k=bnks.begin(); k!=bnks.end(); ++k)
      evt->adoptBank(*k, copy_banks);
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

