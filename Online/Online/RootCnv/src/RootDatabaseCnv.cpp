// $Id: RootDatabaseCnv.cpp,v 1.2 2010-03-03 14:30:47 frankb Exp $
//------------------------------------------------------------------------------
//
// Implementation of class :  RootDatabaseCnv
//
// Author :                   Markus Frank
//
//------------------------------------------------------------------------------
// Include files
#include "RootDatabaseCnv.h"
#include "RootDataConnection.h"

#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/CnvFactory.h"

using namespace Gaudi;

#if 0
PLUGINSVC_FACTORY_WITH_ID( RootDatabaseCnv,
                           ConverterID(ROOT_StorageType,CLID_StatisticsFile),
                           IConverter*(long, CLID, ISvcLocator*) );
#endif

/// Standard Constructor
RootDatabaseCnv::RootDatabaseCnv(long typ, const CLID& cl, ISvcLocator* svc, RootCnvSvc* mgr)
: RootDirectoryCnv(typ, cl, svc, mgr)
{
}

/// Standard destructor
RootDatabaseCnv::~RootDatabaseCnv()   {
}

// Create database object
StatusCode
RootDatabaseCnv::createObj(IOpaqueAddress* pAddr, DataObject*& refpObj)  {
  StatusCode status = StatusCode::FAILURE;
  MsgStream log(msgSvc(), "RootDatabaseCnv");
  if ( pAddr ) {
    RootDataConnection* con = 0;
    IRegistry* pReg = pAddr->registry();
    const unsigned long* ipars = pAddr->ipar();
    const std::string*   spars = pAddr->par();
    char mode = char(ipars[1]);
    std::string fname   = spars[0];
    std::string oname   = pReg->name();
    bool recrea = mode == 'R';
    bool create = mode == 'N';
    bool update = mode == 'U';
    bool read   = mode == 'O';
    const CLID& clid = objType();
    status = StatusCode::SUCCESS;
    std::string cntName = containerName(pReg);
    if ( create ) {
      m_dbMgr->connectDatabase(fname,IDataConnection::CREATE,&con).ignore();
      status = saveDescription(fname, cntName, "File containing statistics results.","", clid);
      if ( status.isSuccess() )  {
        log << MSG::INFO << "Opened NEW Database file:"
            << fname << " as " << oname << endmsg;
      }
    }
    else if ( update )  {
      m_dbMgr->connectDatabase(fname,IDataConnection::UPDATE,&con).ignore();
      status = saveDescription(fname, cntName, "File containing statistics results.","", clid);
      if ( status.isSuccess() )  {
        log << MSG::INFO << "Connect to existing Database file:"
            << fname << " as " << oname << " for UPDATE" << endmsg;
      }
    }
    else if ( read ) {
      status = m_dbMgr->connectDatabase(fname,IDataConnection::READ,&con);
      if ( status.isSuccess() )  {
        log << MSG::INFO << "Connect to existing Database file:"
	    << fname << " as " << oname << " for READ" << endmsg;
      }
    }
    else if ( recrea )  {
      m_dbMgr->connectDatabase(fname,IDataConnection::RECREATE,&con).ignore();
      status = saveDescription(fname, cntName, "File containing statistics results.","", clid);
      if ( status.isSuccess() )  {
        log << MSG::INFO << "Recreate Database file:" << fname << " as " << oname << endmsg;
      }
    }
    else  {
      log << MSG::ERROR << "Don't know what to do:" << fname << endmsg;
      status = StatusCode::FAILURE;
    }
    // Now create object
    if ( status.isSuccess() )  {
      NTuple::File* pFile = new NTuple::File(objType(), fname, oname);
      pFile->setOpen(false);
      refpObj = pFile;
    }
  }
  return status;
}

