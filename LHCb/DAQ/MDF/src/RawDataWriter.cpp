// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/src/RawDataWriter.cpp,v 1.1 2006-03-17 17:24:47 frankb Exp $
//	====================================================================
//  RawDataWriter.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IRegistry.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/RawDataAddress.h"
#include "MDF/RawDataWriter.h"
#include "MDF/MDFHeader.h"
#include "Event/RawEvent.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,RawDataWriter)

using namespace LHCb;
enum { KBYTE=1024, MBYTE=1048576, GBYTE=1073741824 };
const unsigned long long int TBYTE=1099511627776;

/// Standard algorithm constructor
LHCb::RawDataWriter::RawDataWriter(const std::string& name, ISvcLocator* pSvcLocator)
: Algorithm(name, pSvcLocator), m_compress(0), m_genChecksum(0), m_bytesWritten(0),
  m_fileNo(0)
{
  m_data.reserve(1024*64);
  declareProperty("Connection",     m_connectParams="");
  declareProperty("Compress",       m_compress);        // File compression
  declareProperty("CreateChecksum", m_genChecksum);     // Generate checksum
  declareProperty("KbytesPerFile",  m_kbytesPerFile);   // kBytes to be written per file
}


/// Initialize the algorithm.
StatusCode LHCb::RawDataWriter::initialize()   {
  size_t idx = m_connectParams.find("%FNO");
  if ( idx != std::string::npos )  {
    m_connectParams.replace(idx,4,"%03d");
  }
  return switchOutputFile();
}

/// Finalize
StatusCode LHCb::RawDataWriter::finalize() {
  Descriptor::close(m_connection);
  return StatusCode::SUCCESS;
}

/// Change file after a given number of bytes were written.
StatusCode LHCb::RawDataWriter::switchOutputFile()  {
  char filePath[FILENAME_MAX];
  if ( m_connectParams.find("%03d") != std::string::npos )  {
    sprintf(filePath, m_connectParams.c_str(), m_fileNo++);
    m_currStream = filePath;
  }
  else  {
    m_currStream = m_connectParams;
  }
  return switchOutputFile(m_connection, m_currStream);
}

/// Change output file
StatusCode LHCb::RawDataWriter::switchOutputFile(Access& connection, const std::string& fname)  {
  MsgStream log(msgSvc(), name());
  if ( connection.ioDesc > 0 )  {  // close existing connection
    Descriptor::close(connection);
  }
  connection = Descriptor::connect(m_currStream);
  if ( m_connection.ioDesc > 0 )  {
    log << MSG::INFO << "Connected new output stream:" << fname << endmsg;
  }
  else {
    log << MSG::ERROR << "FAILED to connected output stream:" << fname << endmsg;
  }
  return (connection.ioDesc > 0) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

/// Write byte buffer to output stream
StatusCode LHCb::RawDataWriter::writeBuffer(const void* data, size_t len)    {
  MsgStream log(msgSvc(), name());
  if ( m_bytesWritten > KBYTE*m_kbytesPerFile)  {
    StatusCode sc = switchOutputFile();
    if ( !sc.isSuccess() ) return sc;
  }
  int res = Descriptor::write(m_connection,data,len);
  if ( res )   {
    m_bytesWritten += len;
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

/// Write MDF record from serialization buffer
StatusCode 
LHCb::RawDataWriter::writeRawBuffer(size_t len, int evType, int trNumber, unsigned int trMask[4])
{
  int hdrType = 0;
  size_t newlen = len;
  const char* ptr = m_data.data()+sizeof(MDFHeader);
  if ( m_compress )   {
    m_tmp.reserve(len+sizeof(MDFHeader));
    char* tmp = m_tmp.data()+sizeof(MDFHeader);
    if ( compressBuffer(m_compress,tmp,len,ptr,len,newlen).isSuccess() )  {
      int chk = m_genChecksum ? genChecksum(1,tmp,newlen) : 0;
      makeMDFHeader(m_tmp.data(), newlen, evType, hdrType, trNumber, trMask, m_compress, chk);
      return writeBuffer(m_tmp.data(),newlen+sizeof(MDFHeader));
    }
    // Bad compression; file uncompressed buffer
  }
  int chk = m_genChecksum ? genChecksum(1,ptr,len) : 0;
  makeMDFHeader(m_data.data(), len, evType, hdrType, trNumber, trMask, 0, chk);
  return writeBuffer(m_data.data(),len+sizeof(MDFHeader));
}

/// Access MDF record information from the data
StatusCode 
LHCb::RawDataWriter::getHeaderInfo(DataObject* pObj, int& evType, int& trNumber, unsigned int trMask[4])
{
  if ( pObj )  {
    IRegistry* pReg = pObj->registry();
    if ( pReg )  {
      RawDataAddress* pA = dynamic_cast<RawDataAddress*>(pReg->address());
      if ( pA )  {
        evType = pA->eventType();
        //trNumber = pA->triggerNumber();
        ::memcpy(trMask, pA->triggerMask(), sizeof(trMask));
        return StatusCode::SUCCESS;
      }
    }
    SmartDataPtr<DataObject> evt(eventSvc(),"/Event");
    if ( evt &&  evt != pObj )  {  // Make sure we do not get into an infinite loop ...
      return getHeaderInfo(evt, evType, trNumber, trMask);
    }
  }
  /// Last chance: get header information from data content.
  ///---> How ???
  return StatusCode::FAILURE;
}


/// Execute procedure
StatusCode LHCb::RawDataWriter::execute()    {
  int evType = 1;
  int trNumber = 0;
  unsigned int trMask[4] = { ~0x0,~0x0,~0x0,~0x0 };
  SmartDataPtr<RawEvent> raw(eventSvc(),RawEventLocation::Default);
  if ( raw )  {
    if ( getHeaderInfo(raw, evType, trNumber, trMask).isSuccess() )  {
      size_t len = rawEventLength(raw);
      m_data.reserve(len+sizeof(MDFHeader));
      encodeRawBanks(raw, m_data.data()+sizeof(MDFHeader),len);
      return writeRawBuffer(len, evType, trNumber, trMask);
    }
    MsgStream log(msgSvc(), name());
    log << MSG::ERROR << "Failed to access MDF header information." << endmsg;
    return StatusCode::FAILURE;
  }
  MsgStream log(msgSvc(), name());
  log << MSG::ERROR << "Failed to retrieve raw event object:" 
      << RawEventLocation::Default << endmsg;
  return StatusCode::FAILURE;
}
