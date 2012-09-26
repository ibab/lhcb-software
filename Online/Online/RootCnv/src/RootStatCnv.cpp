// $Id: RootStatCnv.cpp,v 1.6 2010-09-27 15:43:53 frankb Exp $
//------------------------------------------------------------------------------
//
// Implementation of class :  RootStatCnv
//
// Author :                   Markus Frank
//
//------------------------------------------------------------------------------

// Include files
#include "RootCnv/RootRefs.h"
#include "RootCnv/RootStatCnv.h"
#include "RootCnv/RootDataConnection.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "TROOT.h"
#include "TBranch.h"
#include <memory>

using namespace std;
using namespace Gaudi;

// Standard Constructor
RootStatCnv::RootStatCnv (long typ,const CLID& clid, ISvcLocator* svc, RootCnvSvc* mgr) 
: RootConverter(typ, clid, svc, mgr), m_dataMgr(0), m_log(0)
{
}

// Initialize converter object
StatusCode RootStatCnv::initialize() {
  StatusCode sc = RootConverter::initialize();
  if ( sc.isSuccess() ) {
    sc = dataProvider()->queryInterface(IDataManagerSvc::interfaceID(),(void**)&m_dataMgr);
    if ( !sc.isSuccess() ) {
      return makeError("Failed to access IDataManagerSvc interface.");
    }
  }
  if ( m_log ) delete m_log;
  m_log = new MsgStream(msgSvc(),System::typeinfoName(typeid(*this)));
  return sc;
}

// Finalize converter object
StatusCode RootStatCnv::finalize() {
  if ( m_log ) {
    delete m_log;
    m_log = 0;
  }
  if ( m_dataMgr ) {
    m_dataMgr->release();
    m_dataMgr = 0;
  }
  return RootConverter::finalize();
}

// Retrieve the name of the container a given object is placed into
const string RootStatCnv::containerName(IRegistry* pReg) const {
  const string& path = pReg->identifier();
  long loc = path.find('/',1);
  string local = "<local>";
  //local = "";
  if ( loc > 0 )  {
    loc = path.find('/',++loc);
    if ( loc > 0 )  {
      local += path.substr(loc,path.length()-loc);
    }
  }
  //for(size_t i=0; i<local.length();++i)
  //  if ( !isalnum(local[i])) local[i] = '_';
  return local;
}

// Retrieve the name of the file a given object is placed into
const string RootStatCnv::fileName(IRegistry* pReg) const {
  string path = topLevel(pReg);
  DataObject* pObj = 0;
  dataProvider()->retrieveObject(path, pObj);
  if ( pObj )  {
    NTuple::File* fptr = dynamic_cast<NTuple::File*>(pObj);
    if ( fptr )  {
      return fptr->name();
    }
  }
  return "";
}

// Retrieve the full path of the file a given object is placed into
const string RootStatCnv::topLevel(IRegistry* pReg) const   {
  if ( pReg )    {
    string path = pReg->identifier();
    size_t idx = path.find('/', 1);
    if ( idx != string::npos )  {
      idx = path.find('/', idx+1);
      if ( idx != string::npos )  {
        path = path.substr(0, idx);
      }
      return path;
    }
  }
  return "";
}

// Helper method to issue error messages.
StatusCode RootStatCnv::makeError(const std::string& msg, bool throw_exc)  const {
  if ( m_log )  {
    log() << MSG::ERROR << msg << endmsg;
  }
  else {
    MsgStream l(msgSvc(),"RootConverter");
    l << MSG::ERROR << msg << endmsg;    
  }
  if ( throw_exc ) {
  }
  return StatusCode::FAILURE;
}

// Save statistics object description.
StatusCode 
RootStatCnv::saveDescription(const string&  path, 
                             const string&  ident, 
                             const string&  desc,
                             const string&  opt,
                             const CLID&    clid)
{
  RootDataConnection* con = 0;
  StatusCode status = m_dbMgr->connectDatabase(path, IDataConnection::UPDATE, &con);
  if ( status.isSuccess() )  {
    TClass* cl = gROOT->GetClass("Gaudi::RootNTupleDescriptor",kTRUE);
    if ( cl ) {
      RootNTupleDescriptor* ptr;
      auto_ptr<RootNTupleDescriptor> dsc(ptr=new RootNTupleDescriptor());
      TBranch* b = con->getBranch("##Descriptors","GaudiStatisticsDescription",cl,ptr,512,0);
      if ( b ) {
        dsc->description = desc;
        dsc->optional    = opt;
        dsc->container   = ident;
        dsc->clid        = clid;
        b->SetAddress(&ptr);
        int nb = b->Fill();
        if ( nb > 1 ) {
          log() << MSG::DEBUG << "Save description for " << ident << endmsg;
          return StatusCode::SUCCESS;
        }
      }
      return makeError("Failed to access ROOT branch GaudiStatisticsDescription");
    }
    return makeError("Failed to access TClass RootNTupleDescriptor");
  }
  return makeError("Failed to access Tuple file:"+path);
}
