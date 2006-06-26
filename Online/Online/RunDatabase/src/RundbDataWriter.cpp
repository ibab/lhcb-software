// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/RunDatabase/src/RundbDataWriter.cpp,v 1.1 2006-06-26 08:48:12 frankb Exp $
//	====================================================================
//  OnlineDataWriter.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/MsgStream.h"
#include "DataWriter.h"
#include "PyRPC.h"
#include <ctime>

#include "RunDatabase/IRunDatabaseWriter.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,OnlineDataWriter)

/// Standard algorithm constructor
LHCb::OnlineDataWriter::OnlineDataWriter(const std::string& nam, ISvcLocator* pSvc)
: RawDataWriter(nam, pSvc), m_runDb(0)
{
  m_closeTMO = 10;
  m_dataType = MDFIO::MDF_NONE;
}


/// Initialize the algorithm.
StatusCode LHCb::OnlineDataWriter::initialize()   {
  MsgStream log(msgSvc(),name());
  StatusCode sc = RawDataWriter::initialize();
  if ( !sc.isSuccess() )  {
    log << MSG::ERROR << "Failed to initialize base class." << endmsg;
    return sc;
  }
  sc = service("RunDatabase",m_runDb);
  if ( !sc.isSuccess() )  {
    log << MSG::ERROR << "Failed to connect to run database." << endmsg;
    return sc;
  }
  return sc;
}

/// Finalize
StatusCode LHCb::OnlineDataWriter::finalize() {
  if ( m_runDb ) m_runDb->release();
  m_runDb = 0;
  return RawDataWriter::finalize();
}

// Submit information to register file to run database
StatusCode LHCb::OnlineDataWriter::submitRunDbOpenInfo(RawDataFile* f, bool blocking)  {
  IRunDatabaseWriter::Status sc = m_runDb->addFile(f->runNumber(), f->name(), m_stream);
  if ( !sc.ok() )  {
    MsgStream log(msgSvc(),name());
    log << MSG::ERROR << "Failed register file:" << f->name() << " to run database." << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

// Submit information about closed file to run database
StatusCode LHCb::OnlineDataWriter::submitRunDbCloseInfo(RawDataFile* f, bool blocking)  {
  typedef PyRPC::Param _P;
  IRunDatabaseWriter::Options opts(_P("FileState","Closed"),
                                   _P("Enddate",time(0)),
                                   _P("MD5Sum",f->md5Sum()),
                                   _P("EventStat",f->eventCounter()),
                                   _P("FileSize",f->bytesWritten()));
  IRunDatabaseWriter::Status sc = m_runDb->modifyFile(f->runNumber(), f->name(), opts);
  if ( !sc.ok() )  {
    MsgStream log(msgSvc(),name());
    log << MSG::ERROR << "Failed update file information for:" << f->name() << " to run database." << endmsg;
    return StatusCode::FAILURE;
  }
  delete f;
  return StatusCode::SUCCESS;
}
