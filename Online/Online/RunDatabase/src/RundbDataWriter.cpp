// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/RunDatabase/src/RundbDataWriter.cpp,v 1.2 2006-07-04 17:04:38 frankb Exp $
//	====================================================================
//  RunDbDataWriter.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "RunDatabase/RunDbDataWriter.h"
#include "PyRPC.h"
#include <ctime>

#include "RunDatabase/IRunDatabaseWriter.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,RunDbDataWriter)

/// Standard algorithm constructor
LHCb::RunDbDataWriter::RunDbDataWriter(const std::string& nam, ISvcLocator* pSvc)
: RawDataWriter(nam, pSvc), m_runDb(0)
{
  m_closeTMO = 10;
  m_type = MDFIO::MDF_NONE;
}


/// Initialize the algorithm.
StatusCode LHCb::RunDbDataWriter::initialize()   {
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
StatusCode LHCb::RunDbDataWriter::finalize() {
  if ( m_runDb ) m_runDb->release();
  m_runDb = 0;
  return RawDataWriter::finalize();
}

// Submit information to register file to run database
StatusCode LHCb::RunDbDataWriter::submitRunDbOpenInfo(RawDataFile* f, bool blocking)  {
  IRunDatabaseWriter::Status sc = m_runDb->addFile(f->runNumber(), f->name(), m_stream);
  if ( !sc.ok() )  {
    MsgStream log(msgSvc(),name());
    log << MSG::ERROR << "Failed register file:" << f->name() << " to run database." << endmsg
        << sc.error() << endmsg;
    return StatusCode::FAILURE;
  }
  //return StatusCode::SUCCESS;
  return RawDataWriter::submitRunDbOpenInfo(f,blocking);
}

// Submit information about closed file to run database
StatusCode LHCb::RunDbDataWriter::submitRunDbCloseInfo(RawDataFile* f, bool blocking)  {
  typedef PyRPC::Param _P;
  if ( f )  {
    IRunDatabaseWriter::Options opts(_P("FileStatus","Closed"),
                                     _P("EndDate",   time(0)),
                                     _P("MD5Sum",    f->md5Sum()),
                                     _P("EventStat", f->eventCounter()),
                                     _P("FileSize",  f->bytesWritten()));
    IRunDatabaseWriter::Status sc = m_runDb->modifyFile(f->runNumber(), f->name(), opts);
    if ( !sc.ok() )  {
      MsgStream log(msgSvc(),name());
      log << MSG::ERROR << "Failed update file information for:" << f->name() << " to run database." << endmsg
          << sc.error() << endmsg;
      return StatusCode::FAILURE;
    }
    return RawDataWriter::submitRunDbCloseInfo(f,blocking);
    //return StatusCode::SUCCESS;
  }
  MsgStream log(msgSvc(),name());
  log << MSG::ERROR << "Failed update file information for unknown file [Invalid Descriptor]" << endmsg;
  return StatusCode::FAILURE;
}
