//$Id: DetDataAgent.h,v 1.3 2001-12-16 16:27:24 andreav Exp $
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

  DetDataAgent( IMessageSvc* msgSvc ) : m_msgSvc( msgSvc ) {};

  virtual ~DetDataAgent() {};

  virtual bool analyse(IRegistry* dir, int level );

 private:
  
  IMessageSvc* m_msgSvc;

};

//---------------------------------------------------------------------------

inline bool DetDataAgent::analyse(IRegistry* dir, int /*level*/ ){
  MsgStream log( m_msgSvc, "detDataAgent" );
  log << MSG::INFO << dir->identifier()
      << ( dir->object() ? " [loaded] " : " " )
      << endreq; 
  return true;
}

//---------------------------------------------------------------------------

#endif // DETCONDEXAMPLE_DETDATAAGENT_H
