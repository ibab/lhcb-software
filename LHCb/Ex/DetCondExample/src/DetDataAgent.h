//$Id: DetDataAgent.h,v 1.5 2006-01-19 18:32:10 marcocle Exp $
#ifndef DETCONDEXAMPLE_DETDATAAGENT_H
#define DETCONDEXAMPLE_DETDATAAGENT_H

#include "GaudiKernel/IDataStoreAgent.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/MsgStream.h"

class IMessageSvc;

///---------------------------------------------------------------------------
/** @class DetDataAgent DetDataAgent.h DetCondExample/DetDataAgent.h

    Simple agent to dump the contents of DetectorDataSvc data store.
    Copied from Ex/DetDescExample.

    @date December 2001
*///--------------------------------------------------------------------------

class DetDataAgent : virtual public IDataStoreAgent {

 public:

  DetDataAgent( IMessageSvc* msgSvc ) : m_msgSvc( msgSvc ) {}

  virtual ~DetDataAgent() {}

  virtual bool analyse(IRegistry* dir, int level );

 private:
  
  IMessageSvc* m_msgSvc;

};

//---------------------------------------------------------------------------

bool DetDataAgent::analyse(IRegistry* dir, int /*level*/ ){
  MsgStream log( m_msgSvc, "detDataAgent" );
  log << MSG::INFO << dir->identifier();
  if ( dir->object() ) {
    log << " [loaded] (clId = " << dir->object()->clID() << ")";
  }
  log << endreq;
  return true;
}

//---------------------------------------------------------------------------

#endif // DETCONDEXAMPLE_DETDATAAGENT_H
