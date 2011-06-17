// $Id: RootDatabaseCnv.cpp,v 1.6 2010-09-27 15:43:53 frankb Exp $
//------------------------------------------------------------------------------
//
// Implementation of class :  RootDatabaseCnv
//
// Author :                   Markus Frank
//
//------------------------------------------------------------------------------
// Framework include files
#include "RootCnv/RootDatabaseCnv.h"
#include "RootCnv/RootDataConnection.h"

#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/NTuple.h"

#if 0
#include "GaudiKernel/CnvFactory.h"
PLUGINSVC_FACTORY_WITH_ID( RootDatabaseCnv,
                          ConverterID(ROOT_StorageType,CLID_StatisticsFile),
                          IConverter*(long, CLID, ISvcLocator*) );
#endif

using namespace Gaudi;
using namespace std;

// Standard Constructor
RootDatabaseCnv::RootDatabaseCnv(long typ, const CLID& cl, ISvcLocator* svc, RootCnvSvc* mgr)
: RootDirectoryCnv(typ, cl, svc, mgr)
{
}

// Standard destructor
RootDatabaseCnv::~RootDatabaseCnv()   {
}

// Create database object
StatusCode
RootDatabaseCnv::createObj(IOpaqueAddress* pAddr, DataObject*& refpObj)  {
  StatusCode status = StatusCode::FAILURE;
  if ( pAddr ) {
    RootDataConnection* con = 0;
    IRegistry* pReg = pAddr->registry();
    const unsigned long* ipars = pAddr->ipar();
    const string*   spars = pAddr->par();
    char mode = char(ipars[1]);
    string fname   = spars[0];
    string oname   = pReg->name();
    bool recrea = mode == 'R';
    bool create = mode == 'N';
    bool update = mode == 'U';
    bool read   = mode == 'O';
    const CLID& clid = objType();
    status = StatusCode::SUCCESS;
    string cntName = containerName(pReg);
    if ( create ) {
      m_dbMgr->connectDatabase(fname,IDataConnection::CREATE,&con).ignore();
      status = saveDescription(fname, cntName, "File containing statistics results.","", clid);
      if ( status.isSuccess() )  {
        log() << MSG::INFO << "Opened NEW Database file:"
          << fname << " as " << oname << endmsg;
      }
    }
    else if ( update )  {
      m_dbMgr->connectDatabase(fname,IDataConnection::UPDATE,&con).ignore();
      status = saveDescription(fname, cntName, "File containing statistics results.","", clid);
      if ( status.isSuccess() )  {
        log() << MSG::INFO << "Connect to existing Database file:"
          << fname << " as " << oname << " for UPDATE" << endmsg;
      }
    }
    else if ( read ) {
      status = m_dbMgr->connectDatabase(fname,IDataConnection::READ,&con);
      if ( status.isSuccess() )  {
        log() << MSG::INFO << "Connect to existing Database file:"
          << fname << " as " << oname << " for READ" << endmsg;
      }
    }
    else if ( recrea )  {
      m_dbMgr->connectDatabase(fname,IDataConnection::RECREATE,&con).ignore();
      status = saveDescription(fname, cntName, "File containing statistics results.","", clid);
      if ( status.isSuccess() )  {
        log() << MSG::INFO << "Recreate Database file:" << fname << " as " << oname << endmsg;
      }
    }
    else  {
      log() << MSG::ERROR << "Don't know what to do:" << fname << endmsg;
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

