//$Id: DetDataAgent.h,v 1.6 2006-08-31 13:53:44 marcocle Exp $
#ifndef DETCONDEXAMPLE_DETDATAAGENT_H
#define DETCONDEXAMPLE_DETDATAAGENT_H

#include "GaudiKernel/IDataStoreAgent.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/MsgStream.h"

#include "DetDesc/Condition.h"

class IMessageSvc;

///---------------------------------------------------------------------------
/** @class DetDataAgent DetDataAgent.h DetCondExample/DetDataAgent.h

    Simple agent to dump the contents of DetectorDataSvc data store.
    Copied from Ex/DetDescExample.

    @date December 2001
*///--------------------------------------------------------------------------

class DetDataAgent : virtual public IDataStoreAgent {

 public:

  DetDataAgent( IMessageSvc* msgSvc, bool dumpConditions = false ) :
    m_msgSvc( msgSvc ), m_dumpCond( dumpConditions ) {}

  virtual ~DetDataAgent() {}

  virtual bool analyse(IRegistry* dir, int level );

 private:

  IMessageSvc* m_msgSvc;
  bool m_dumpCond;

};

//---------------------------------------------------------------------------

bool DetDataAgent::analyse(IRegistry* dir, int /*level*/ ){
  MsgStream log( m_msgSvc, "detDataAgent" );
  log << MSG::INFO << dir->identifier();
  if ( dir->object() ) {
    log << " [loaded] (clId = " << dir->object()->clID() << ")";
    if (m_dumpCond) {
      ParamValidDataObject * pvdo = dynamic_cast<ParamValidDataObject *>(dir->object());
      if (pvdo) log << "\n" << pvdo->printParams();
    }
  }
  log << endreq;
  return true;
}

//---------------------------------------------------------------------------

#endif // DETCONDEXAMPLE_DETDATAAGENT_H
