// $Id: MIFWriter.cpp,v 1.12 2009-04-17 13:24:36 cattanem Exp $
//  ====================================================================
//  MIFWriter.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiUtils/IIODataManager.h"
#include "MDF/RawDataConnection.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/RawDataAddress.h"
#include "MDF/MIFWriter.h"
#include "MDF/MIFHeader.h"
#include "Event/RawEvent.h"
#include <cerrno>

using namespace LHCb;
using namespace Gaudi;
typedef std::pair<MIFHeader*,MIFHeader::Event*> MIFDesc;

/// Standard algorithm constructor
MIFWriter::MIFWriter(const std::string& nam, ISvcLocator* svc)
: Algorithm(nam, svc), m_ioMgr(0), m_connection(0)
{
  declareProperty("Connection",  m_connectParams="");
  declareProperty("DataManager", m_ioMgrName="Gaudi::IODataManager/IODataManager");
}

/// Standard Destructor
MIFWriter::~MIFWriter()     {
}

/// Initialize
StatusCode MIFWriter::initialize()   {
  MsgStream log(msgSvc(), name());
  StatusCode status = service(m_ioMgrName, m_ioMgr);
  if( !status.isSuccess() ) {
    log << MSG::ERROR 
        << "Unable to localize interface IID_IIODataManager from service:" 
        << m_ioMgrName << endmsg;
    return status;
  }
  m_connection = new RawDataConnection(this,m_connectParams);
  status = m_ioMgr->connectWrite(m_connection,IDataConnection::RECREATE,"MDF");
  if ( m_connection->isConnected() )
    log << MSG::INFO << "Received event request connection." << endmsg;
  else
    log << MSG::INFO << "FAILED receiving event request connection." << endmsg;
  return m_connection->isConnected() ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

/// Finalize
StatusCode MIFWriter::finalize() {
  if ( m_ioMgr )  {
    m_ioMgr->disconnect(m_connection).ignore();
    delete m_connection;
    m_ioMgr->release();
    m_ioMgr = 0;
  }
  return StatusCode::SUCCESS;
}

// Execute procedure
StatusCode MIFWriter::execute()    {
  SmartDataPtr<DataObject> raw(eventSvc(),"/Event/DAQ/RawEvent");
  if ( raw )  {
    IRegistry* pReg = raw->registry();
    if ( pReg )  {
      RawDataAddress* pA = dynamic_cast<RawDataAddress*>(pReg->address());
      if ( pA )  {
        char memory[1024];
        const std::string& dsn = pA->par()[0];
        int fid = genChecksum(1,dsn.c_str(),dsn.length()+1);
        FidMap::const_iterator i = m_fidMap.find(fid);
  //std::cout << "FID:" << dsn << " " << fid << endl;
        if ( i == m_fidMap.end() )   {
          MsgStream log(msgSvc(),name());
          m_fidMap.emplace(fid,dsn);
          MIFHeader* hdr = MIFHeader::create(memory, dsn, fid);
          if ( !m_ioMgr->write(m_connection, hdr, hdr->totalSize()).isSuccess() )  {
            log << MSG::ERROR << "Failed to write File record for " << dsn << endmsg;
            return StatusCode::FAILURE;
          }
          log << MSG::INFO << "Using FileID " << std::hex << std::setw(8) << fid 
              << " for '" << dsn << "'" << endmsg;
          i = m_fidMap.find(fid);
        }
        if ( dsn != (*i).second )  {
          MsgStream log(msgSvc(),name());
          log << MSG::ERROR << "MDF file name clash! cannot write MIF." << std::endl
            << "FileID:" << std::hex << std::setw(8) << fid << " is hashed by '" << dsn 
            << "' and by '" << (*i).second << "'" << endmsg;
          return StatusCode::FAILURE;
        }
        MIFDesc dsc = MIFHeader::create<MIFHeader::Event>(memory, fid, MIFHeader::MIF_EVENT);
        dsc.second->setOffset(pA->fileOffset());
        return m_ioMgr->write(m_connection, dsc.first, dsc.first->totalSize());
      }
    }
  }
  return StatusCode::FAILURE;
}
