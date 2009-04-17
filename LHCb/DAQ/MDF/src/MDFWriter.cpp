// $Id: MDFWriter.cpp,v 1.30 2009-04-17 13:24:36 cattanem Exp $
//  ====================================================================
//  MDFWriter.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiUtils/IIODataManager.h"
#include "MDF/RawDataConnection.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/MDFWriter.h"
#include "MDF/MDFHeader.h"
#include "Event/RawEvent.h"
#include "TMD5.h"
#include <iomanip>
#include <cstring> // For memcpy with gcc 4.3

using namespace LHCb;
using namespace Gaudi;

/// Extended algorithm constructor
MDFWriter::MDFWriter(MDFIO::Writer_t typ, const std::string& nam, ISvcLocator* pSvc)
: Algorithm(nam, pSvc), MDFIO(typ, nam)
{
  m_inputType = typ;
  construct();
}

/// Standard algorithm constructor
MDFWriter::MDFWriter(const std::string& nam, ISvcLocator* pSvc)
: Algorithm(nam, pSvc), MDFIO(MDFIO::MDF_RECORDS, nam)
{
  m_inputType = MDFIO::MDF_NONE;
  construct();
}

/// Internal setup (may not be called from sub-classes!
void MDFWriter::construct()   {
  m_ioMgr = 0;
  m_connection = 0;
  m_bytesWritten = 0;
  m_md5 = new TMD5();
  m_data.reserve(1024*64);
  declareProperty("Connection",     m_connectParams="");
  declareProperty("Compress",       m_compress=2);        // File compression
  declareProperty("ChecksumType",   m_genChecksum=1);     // Generate checksum
  declareProperty("GenerateMD5",    m_genMD5=true);       // Generate MD5 checksum
  declareProperty("InputDataType",  m_inputType);         // Input data type
  declareProperty("DataType",       m_dataType);          // Output data type
  declareProperty("BankLocation",   m_bankLocation=RawEventLocation::Default);  // Location of the banks in the TES
  declareProperty("DataManager",    m_ioMgrName="Gaudi::IODataManager/IODataManager");
  declareProperty("ForceTAE",       m_forceTAE = false);
  declareProperty("AddSpace",       m_addSpace=1);
}

MDFWriter::~MDFWriter()   {
  if ( m_md5 ) delete m_md5;
}

/// Initialize
StatusCode MDFWriter::initialize()   {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Initialize MDFWriter" << endmsg;
  
  std::string con = getConnection(m_connectParams);
  // Retrieve conversion service handling event iteration
  StatusCode status = service(m_ioMgrName, m_ioMgr);
  if( !status.isSuccess() ) {
    log << MSG::ERROR 
        << "Unable to localize interface IID_IIODataManager from service:" 
        << m_ioMgrName << endmsg;
    return status;
  }
  m_bytesWritten = 0;
  m_connection = new RawDataConnection(this,con);
  status = m_ioMgr->connectWrite(m_connection,IDataConnection::RECREATE,"MDF");
  status.ignore();
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
    log << MSG::INFO << "Size:" << std::setw(8) << m_bytesWritten << " Output:" << m_connectParams
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
MDFIO::MDFDescriptor MDFWriter::getDataSpace(void* const /* ioDesc */, size_t len)  {
  m_data.reserve(len+additionalSpace());
  return MDFDescriptor(m_data.data(), m_data.size());
}

/// Execute procedure
StatusCode MDFWriter::execute()    {
  std::pair<const char*,int> data;
  setupMDFIO(msgSvc(),eventSvc());
  MsgStream log(msgSvc(), name());
  log << MSG::VERBOSE << "Got data as " << m_inputType 
      << " Send as " << m_dataType << endmsg;
  switch(m_inputType)   {
    case MDFIO::MDF_NONE:
      return commitRawBanks(m_compress, m_genChecksum, m_connection, m_bankLocation);
    case MDFIO::MDF_BANKS:
      data = getDataFromAddress();
      if ( data.first )  {
	StatusCode sc = StatusCode::SUCCESS;
	RawBank* b = (RawBank*)data.first;
	switch(m_dataType) {
	case MDFIO::MDF_RECORDS:
	  sc = writeBuffer(m_connection,b->data(), data.second-b->hdrSize());
	  sc.isSuccess() ? ++m_writeActions : ++m_writeErrors;
	  return sc;
	case MDFIO::MDF_BANKS:
	  sc = writeBuffer(m_connection,data.first, data.second);
	  sc.isSuccess() ? ++m_writeActions : ++m_writeErrors;
	  return sc;
	default:
	  break;
	}
      }
      break;
    case MDFIO::MDF_RECORDS:
      data = getDataFromAddress();
      if ( data.first )  {
	StatusCode sc = StatusCode::SUCCESS;
	switch(m_dataType) {
	case MDFIO::MDF_RECORDS:
	  sc = writeBuffer(m_connection,data.first, data.second);
	  sc.isSuccess() ? ++m_writeActions : ++m_writeErrors;
	  return sc;
	case MDFIO::MDF_BANKS:
	  {
	    MDFHeader* h = (MDFHeader*)data.first;
	    m_data.reserve(data.second+100*sizeof(int));
	    RawBank* b = (RawBank*)m_data.data();
	    size_t len = sizeof(MDFHeader)+h->subheaderLength();
	    b->setMagic();
	    b->setType(RawBank::DAQ);
	    b->setSize(len);
	    b->setVersion(DAQ_STATUS_BANK);
	    b->setSourceID(0);
	    ::memcpy(b->data(), data.first, data.second);
	    sc = writeBuffer(m_connection,m_data.data(),data.second+b->hdrSize());
	    sc.isSuccess() ? ++m_writeActions : ++m_writeErrors;
	    return sc;
	  }	    
	default:
	  break;
	}
      }
      break;
  default:
    break;
  }
  return StatusCode::FAILURE;
}

/// Write byte buffer to output stream
StatusCode MDFWriter::writeBuffer(void* const ioDesc, const void* data, size_t len)    {
  Connection* c = (Connection*)ioDesc;
  StatusCode sc = m_ioMgr->write(c, data, len);
  if ( sc.isSuccess() )  {
    m_bytesWritten += len;
    if ( m_genMD5 )  {
      m_md5->Update((const unsigned char*)data, len);
    }
  }
  return sc;
}
