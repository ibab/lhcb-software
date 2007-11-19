// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/src/MDFWriter.cpp,v 1.10 2007-11-19 19:27:32 frankb Exp $
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
#include "GaudiUtils/IIODataManager.h"
#include "MDF/RawDataConnection.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/MDFWriter.h"
#include "MDF/MDFHeader.h"
#include "Event/RawEvent.h"
#include "TMD5.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,MDFWriter)

using namespace LHCb;
using namespace Gaudi;

/// Extended algorithm constructor
MDFWriter::MDFWriter(MDFIO::Writer_t typ, const std::string& nam, ISvcLocator* pSvc)
: Algorithm(nam, pSvc), MDFIO(typ, nam)
{
  construct();
}

/// Standard algorithm constructor
MDFWriter::MDFWriter(const std::string& nam, ISvcLocator* pSvc)
: Algorithm(nam, pSvc), MDFIO(MDFIO::MDF_RECORDS, nam)
{
  construct();
}

/// Internal setup (may not be called from sub-classes!
void MDFWriter::construct()   {
  m_ioMgr = 0;
  m_connection = 0;
  m_md5 = new TMD5();
  m_data.reserve(1024*64);
  declareProperty("Connection",     m_connectParams="");
  declareProperty("Compress",       m_compress=2);        // File compression
  declareProperty("ChecksumType",   m_genChecksum=1);     // Generate checksum
  declareProperty("GenerateMD5",    m_genMD5=false);      // Generate MD5 checksum
  declareProperty("DataType",       m_dataType=MDFIO::MDF_NONE); // Input data type
	declareProperty("BankLocation",		m_bankLocation=RawEventLocation::Default);  // Location of the banks in the TES
  declareProperty("DataManager",    m_ioMgrName="IODataManager");
}

MDFWriter::~MDFWriter()   {
  if ( m_md5 ) delete m_md5;
}

/// Initialize
StatusCode MDFWriter::initialize()   {
  MsgStream log(msgSvc(), name());
  std::string con = getConnection(m_connectParams);
  // Retrieve conversion service handling event iteration
  StatusCode status = service(m_ioMgrName, m_ioMgr);
  if( !status.isSuccess() ) {
    log << MSG::ERROR 
        << "Unable to localize interface IID_IIODataManager from service:" 
        << m_ioMgrName << endreq;
    return status;
  }
  m_connection = new RawDataConnection(this,con);
  status = m_ioMgr->connectWrite(m_connection,IDataConnection::RECREATE,"MDF");
  if ( m_connection->isConnected() )
    log << MSG::INFO << "Received event request connection." << endmsg;
  else
    log << MSG::ERROR << "FAILED receiving event request connection." << endmsg;
  return (m_connection) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

/// Finalize
StatusCode MDFWriter::finalize() {
  if ( m_genMD5 )  {
    MsgStream log(msgSvc(), name());
    m_md5->Final();
    log << MSG::INFO << "Output:" << m_connectParams
      << " MD5 sum:" << m_md5->AsString() << endmsg;
  }
  if ( m_ioMgr )  {
    m_ioMgr->disconnect(m_connection).ignore();
    delete m_connection;
    m_ioMgr->release();
    m_ioMgr = 0;
  }
  return StatusCode::SUCCESS;
}

/// Allocate space for IO buffer
std::pair<char*,int> MDFWriter::getDataSpace(void* const /* ioDesc */, size_t len)  {
  m_data.reserve(len);
  return std::pair<char*,int>(m_data.data(), m_data.size());
}

/// Execute procedure
StatusCode MDFWriter::execute()    {
  setupMDFIO(msgSvc(),eventSvc());
  MsgStream log(msgSvc(), name());
  switch(m_dataType) {
    case MDF_NONE:
      return commitRawBanks(m_compress, m_genChecksum, m_connection, m_bankLocation);
    case MDF_BANKS:
    case MDF_RECORDS:
      return commitRawBuffer(m_dataType, m_compress, m_genChecksum, m_connection);
    default:
      break;
  }
  return StatusCode::FAILURE;
}

/// Write byte buffer to output stream
StatusCode MDFWriter::writeBuffer(void* const /* ioDesc */, const void* data, size_t len)    {
  StatusCode sc = m_ioMgr->write(m_connection, data, len);
  if ( sc.isSuccess() )  {
    if ( m_genMD5 )  {
      m_md5->Update((const unsigned char*)data, len);
    }
  }
  return sc;
}
