// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/src/MDFWriter.cpp,v 1.7 2006-09-25 12:32:26 frankb Exp $
//	====================================================================
//  MDFWriter.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/MDFWriter.h"
#include "MDF/MDFHeader.h"
#include "Event/RawEvent.h"
#include "TMD5.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,MDFWriter)

using namespace LHCb;

/// Extended algorithm constructor
LHCb::MDFWriter::MDFWriter(MDFIO::Writer_t typ, const std::string& nam, ISvcLocator* pSvc)
: Algorithm(nam, pSvc), MDFIO(typ, nam)
{
  construct();
}

/// Standard algorithm constructor
LHCb::MDFWriter::MDFWriter(const std::string& nam, ISvcLocator* pSvc)
: Algorithm(nam, pSvc), MDFIO(MDFIO::MDF_RECORDS, nam)
{
  construct();
}

/// Internal setup (may not be called from sub-classes!
void LHCb::MDFWriter::construct()   {
  m_md5 = new TMD5();
  m_data.reserve(1024*64);
  declareProperty("Connection",     m_connectParams="");
  declareProperty("Compress",       m_compress=2);        // File compression
  declareProperty("ChecksumType",   m_genChecksum=1);     // Generate checksum
  declareProperty("GenerateMD5",    m_genMD5=false);      // Generate MD5 checksum
  declareProperty("DataType",       m_dataType=MDFIO::MDF_NONE); // Input data type
}

LHCb::MDFWriter::~MDFWriter()   {
  if ( m_md5 ) delete m_md5;
}

/// Initialize
StatusCode LHCb::MDFWriter::initialize()   {
  MsgStream log(msgSvc(), name());
  std::string con = getConnection(m_connectParams);
  m_connection = Descriptor::connect(con);
  if ( m_connection.ioDesc > 0 )  {
    log << MSG::INFO << "Received event request connection." << endmsg;
  }
  else {
    log << MSG::ERROR << "FAILED receiving event request connection." << endmsg;
  }
  return (m_connection.ioDesc > 0) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

/// Finalize
StatusCode LHCb::MDFWriter::finalize() {
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
StatusCode LHCb::MDFWriter::execute()    {
  setupMDFIO(msgSvc(),eventSvc());
  switch(m_dataType) {
    case MDF_NONE:
      return commitRawBanks(m_compress, m_genChecksum, &m_connection);
    case MDF_BANKS:
    case MDF_RECORDS:
      return commitRawBuffer(m_dataType, m_compress, m_genChecksum, &m_connection);
    default:
      break;
  }
  return StatusCode::FAILURE;
}

/// Write byte buffer to output stream
StatusCode LHCb::MDFWriter::writeBuffer(void* const /* ioDesc */, const void* data, size_t len)    {
  int res = Descriptor::write(m_connection, data, len);
  if ( res )  {
    if ( m_genMD5 )  {
      m_md5->Update((const unsigned char*)data, len);
    }
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}
