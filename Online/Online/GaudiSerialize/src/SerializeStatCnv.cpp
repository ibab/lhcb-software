// $Id: SerializeStatCnv.cpp,v 1.6 2010/09/27 15:43:53 frankb Exp $
//------------------------------------------------------------------------------
//
// Implementation of class :  SerializeStatCnv
//
// Author :                   Markus Frank
//
//------------------------------------------------------------------------------

// Include files
#include "SerializeStatCnv.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "Event/RawEvent.h"

using namespace Gaudi;
using namespace std;

SerializeNTupleFile::~SerializeNTupleFile() {
  if ( data ) delete data;
  data = 0;
}

// Initialize converter object
StatusCode SerializeStatCnv::initialize() {
  StatusCode sc = SerializeBaseCnv::initialize();
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
StatusCode SerializeStatCnv::finalize() {
  if ( m_log ) {
    delete m_log;
    m_log = 0;
  }
  if ( m_dataMgr ) {
    m_dataMgr->release();
    m_dataMgr = 0;
  }
  return SerializeBaseCnv::finalize();
}

// Helper method to issue error messages.
StatusCode SerializeStatCnv::makeError(const std::string& msg, bool throw_exc)  const {
  if ( m_log )  {
    log() << MSG::ERROR << msg << endmsg;
  }
  else {
    MsgStream l(msgSvc(),"SerializeConverter");
    l << MSG::ERROR << msg << endmsg;    
  }
  if ( throw_exc ) {
  }
  return StatusCode::FAILURE;
}

// Retrieve the full path of the file a given object is placed into
const string SerializeStatCnv::topLevel(IRegistry* pReg) const   {
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
