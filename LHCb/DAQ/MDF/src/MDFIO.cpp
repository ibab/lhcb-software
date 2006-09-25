// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/src/MDFIO.cpp,v 1.6 2006-09-25 12:48:05 frankb Exp $
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
  switch(m_type)  {
    case MDF_NONE:    // Pure RawEvent structure with MDF Header encoded as bank
      {
        SmartDataPtr<RawEvent> raw(m_evtSvc,RawEventLocation::Default);
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
        return std::pair<const char*,int>((const char*)pA->data(), pA->dataLength());
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
  SmartDataPtr<RawEvent> raw(m_evtSvc,RawEventLocation::Default);
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
    log1 << MSG::ERROR << "Failed to access MDF header information." << endmsg;
    return StatusCode::FAILURE;
  }
  MsgStream log2(m_msgSvc, m_parent);
  log2 << MSG::ERROR << "Failed to retrieve raw event object:" 
       << RawEventLocation::Default << endmsg;
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
        int hdrSize  = h->rawSize()+h->subheaderLength();
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
  int bnkSize   = hdr->totalSize();
  size_t newlen = len-bnkSize;
  int hdrSize   = h->rawSize()+h->subheaderLength();
  int compress  = compTyp;
  int checksum  = chksumTyp;
  char* ptr = data+bnkSize;
  if ( compress )   {
    m_tmp.reserve(len+bnkSize);
    if (compressBuffer(compress,m_tmp.data()+bnkSize,newlen,ptr,newlen,newlen).isSuccess())  {
      ptr = m_tmp.data()+bnkSize;
      int cmp = (len-bnkSize)/newlen - 1;
      compress = (compress&0xF) + ((cmp>0xF ? 0xF : cmp)<<4);
    }
    else  {
      // Bad compression; file uncompressed buffer
      newlen = len-bnkSize;
      compress = 0;
      ptr = data+bnkSize;
    }
  }
  int chk = checksum>0 ? genChecksum(checksum,ptr,newlen) : 0;
  if ( m_type == MDF_RECORDS )  {
    ::memmove(ptr - hdrSize, h, hdrSize);
    h = (MDFHeader*)(ptr-hdrSize);
    h->setSize(newlen);
    h->setChecksum(chk);
    h->setCompression(compress);
    return writeBuffer(ioDesc, ptr-hdrSize, newlen+hdrSize);
  }
  ptr -= bnkSize;
  ::memmove(ptr, hdr, bnkSize);
  h = (MDFHeader*)((RawBank*)ptr)->data();
  h->setSize(newlen);
  h->setChecksum(chk);
  h->setCompression(compress);
  return writeBuffer(ioDesc, ptr, newlen+bnkSize);
}

std::pair<char*,int> 
LHCb::MDFIO::readBanks(void* const ioDesc, bool dbg)   {
  MDFHeader h;
  std::pair<char*,int> result(0,-1);
  int rawSize = MDFHeader::rawSize();
  if ( readBuffer(ioDesc, &h, rawSize).isSuccess() )  {
    int datSize  = h.size();
    int checksum = h.checkSum();
    int compress = h.compression()&0xF;
    int expand   = h.compression()>>4;
    bool velo_patch = h.subheaderLength()==sizeof(int);
    int hdrSize  = velo_patch ? sizeof(MDFHeader::Header0) : h.subheaderLength();
    int readSize = hdrSize+h.size();
    int alloc_len = rawSize+readSize + (compress ? expand*readSize : 0);
    if ( dbg )  {
      MsgStream log(m_msgSvc, m_parent);
      log << MSG::ALWAYS << "Compression:" << compress << " Checksum:" << (checksum != 0) <<  endmsg;
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
          ::memcpy(bptr+rawSize, src, hdrSize);
          src += hdrSize;
          char* ptr = ((char*)data) + bnkSize;
          size_t space_size = space.second - bnkSize;
          if ( checksum )  {
            int chk = genChecksum(1,src,datSize);
            if ( chk != checksum )  {
              MsgStream log(m_msgSvc, m_parent);
              log << MSG::ERROR << "Data corruption. [Invalid checksum] expected:" 
                << (void*)checksum << " got:" << (void*)chk << endmsg;
              return result;
            }
          }
          if ( decompressBuffer(compress,ptr,space_size,src,datSize,new_len).isSuccess()) {
            hdr->setSize(new_len);
            hdr->setCompression(0);
            hdr->setChecksum(0);
            return std::pair<char*, int>(data,bnkSize+new_len);
          }
          MsgStream log0(m_msgSvc, m_parent);
          log0 << MSG::ERROR << "Cannot allocate sufficient space for decompression." << endmsg;
          return result;
        }
        MsgStream log1(m_msgSvc, m_parent);
        log1 << MSG::ERROR << "Cannot read " << readSize << " bytes of compressed data." << endmsg;
        return result;
      }
      // Read uncompressed data file...
      int off = bnkSize - hdrSize;
      if ( readBuffer(ioDesc, data+off, readSize).isSuccess() )  {
        if ( checksum )  {
          int chk = genChecksum(1,data+off+hdrSize,datSize);
          if ( chk != checksum )  {
            MsgStream log(m_msgSvc, m_parent);
            log << MSG::ERROR << "Data corruption. [Invalid checksum] expected:" 
                << (void*)checksum << " got:" << (void*)chk << endmsg;
            return result;
          }
        }
        off -= rawSize + b->hdrSize();
        if ( off != 0 )  {
          ::memmove(bptr+rawSize, bptr+rawSize+off, hdrSize);
        }
        if ( velo_patch )  {
          // Fix for intermediate VELO data
          MDFHeader::Header0* h0 = hdr->subHeader().H0;
          if ( h0->triggerMask()[0] != 0x103 )  {
            MsgStream log(m_msgSvc, m_parent);
            log << MSG::ERROR << "Data corruption. [Velo_Patch]....expect trouble!!!" << endmsg;
          }
          h0->setEventType(hdr->hdr());
          h.setSubheaderLength(sizeof(MDFHeader::Header0));
          h.setHeaderVersion(0);
        }
        return std::pair<char*, int>(data, bnkSize+datSize);
      }
/*
      int payload = hdrSize + rawSize;
      int off = (payload%sizeof(int) ? (payload/sizeof(int) + 1)*sizeof(int) : payload) - hdrSize;
      if ( readBuffer(ioDesc, bptr+off, readSize).isSuccess() )  {
        if ( checksum )  {
          int chk = genChecksum(1,bptr+off+hdrSize,datSize);
          if ( chk != checksum )  {
            MsgStream log(m_msgSvc, m_parent);
            log << MSG::ERROR << "Data corruption. [Invalid checksum] expected:" 
              << (void*)checksum << " got:" << (void*)chk << endmsg;
            return result;
          }
        }
        if ( (off-rawSize) != 0 )  {
          ::memmove(bptr+rawSize, bptr+off, hdrSize);
        }
        return std::pair<char*, int>(data, bnkSize+datSize);
      }
*/
      MsgStream log2(m_msgSvc, m_parent);
      log2 << MSG::ERROR << "Cannot allocate buffer to read:" << readSize << " bytes "  << endmsg;
      return result;
    }
    MsgStream log3(m_msgSvc, m_parent);
    log3 << MSG::ERROR << "Cannot read " << readSize << " bytes of uncompressed data." << endmsg;
    return result;
  }
  MsgStream log4(m_msgSvc, m_parent);
  log4 << MSG::ERROR << "Cannot read " << rawSize << " bytes of header data." << endmsg;
  return result;
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
