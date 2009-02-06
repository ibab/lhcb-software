// $Id: MDFWriterLite.cpp,v 1.1 2009-02-06 09:37:57 frankb Exp $
//  ====================================================================
//  MDFWriterLite.cpp
//  --------------------------------------------------------------------
//
//  Author    : Sai Suman & Niko Neufeld 
//
//  ====================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/MDFWriterLite.h"
#include "MDF/MDFHeader.h"
#include "Event/RawEvent.h"
#include "TMD5.h"

#include <ctime>
#include <clocale>

DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,MDFWriterLite)

namespace {  // Don't clutter global namespace!
  void getDateStr(char *datestring, int strlen)   {
    ::setlocale(LC_ALL, "");
    time_t ctime = ::time(NULL);
    struct tm *currtime = ::localtime(&ctime);
    ::strftime (datestring, strlen, "%EY.%0m.%0d-%0H.%0M.%0S.mdf", currtime);
    return;
  }
}

/// Extended algorithm constructor
LHCb::MDFWriterLite::MDFWriterLite(MDFIO::Writer_t typ, const std::string& nam, ISvcLocator* pSvc)
: Algorithm(nam, pSvc), MDFIO(typ, nam)
{
  construct();
  m_bytesWritten = m_eventsWritten = 0;
}

/// Standard algorithm constructor
LHCb::MDFWriterLite::MDFWriterLite(const std::string& nam, ISvcLocator* pSvc)
: Algorithm(nam, pSvc), MDFIO(MDFIO::MDF_RECORDS, nam)
{
  construct();
}

/// Internal setup (may not be called from sub-classes!
void LHCb::MDFWriterLite::construct()   {
  m_md5 = new TMD5();
  m_data.reserve(1024*64);
  declareProperty("Connection",     m_connectParams="");
  declareProperty("Compress",       m_compress=2);        // File compression
  declareProperty("ChecksumType",   m_genChecksum=1);     // Generate checksum
  declareProperty("GenerateMD5",    m_genMD5=false);      // Generate MD5 checksum
  declareProperty("DataType",       m_dataType=MDFIO::MDF_NONE); // Input data type
  declareProperty("MaxFileSizeKB",  m_maxFileSizeKB=2000000);
  declareProperty("MaxFileEvents",  m_maxFileEvents=1000000);
  declareProperty("BankLocation",    m_bankLocation=LHCb::RawEventLocation::Default);  // Location of the banks in the TES
}

LHCb::MDFWriterLite::~MDFWriterLite()   {
  if ( m_md5 ) delete m_md5;
}

/// Initialize
StatusCode LHCb::MDFWriterLite::initialize()   {
  char dateStr[40];
  MsgStream log(msgSvc(), name());
  getDateStr(dateStr, sizeof(dateStr));
  m_connection = Descriptor::connect(m_connectParams + dateStr);
  if ( m_connection.ioDesc > 0 )  {
    log << MSG::INFO << "Received event request connection." << endmsg;
  }
  else {
    log << MSG::ERROR << "FAILED receiving event request connection." << endmsg;
  }
  return (m_connection.ioDesc > 0) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

/// Finalize
StatusCode LHCb::MDFWriterLite::finalize() {
  if ( m_genMD5 )  {
    MsgStream log(msgSvc(), name());
    m_md5->Final();
    log << MSG::INFO << "Output:" << m_connectParams
      << " MD5 sum:" << m_md5->AsString() << endmsg;
  }
  Descriptor::close(m_connection);
  return StatusCode::SUCCESS;
}

/// Execute procedure
StatusCode LHCb::MDFWriterLite::execute()    {
  if (((m_bytesWritten>>10) > m_maxFileSizeKB) || (m_eventsWritten >= m_maxFileEvents)) {
    Descriptor::close(m_connection);
    char dateStr[40];
    getDateStr(dateStr, sizeof(dateStr));
    ////Time for a new file. Let's create it.
    m_connection = Descriptor::connect(m_connectParams + dateStr); 
    m_bytesWritten = m_eventsWritten = 0;
  }
  setupMDFIO(msgSvc(),eventSvc());
  switch(m_dataType) {
    case MDF_NONE:
      return commitRawBanks(m_compress, m_genChecksum, &m_connection, m_bankLocation);
    case MDF_BANKS:
    case MDF_RECORDS:
      return commitRawBuffer(m_dataType, m_compress, m_genChecksum, &m_connection);
    default:
      break;
  }
  return StatusCode::FAILURE;
}

/// Write byte buffer to output stream
StatusCode LHCb::MDFWriterLite::writeBuffer(void* const /* ioDesc */, const void* data, size_t len)    {
  int res = Descriptor::write(m_connection, data, len);
  if ( res )  {
    if ( m_genMD5 )  {
      m_md5->Update((const unsigned char*)data, len);
    }
    m_bytesWritten += len;
    m_eventsWritten++;
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}
