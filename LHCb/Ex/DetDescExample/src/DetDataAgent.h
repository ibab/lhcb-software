#ifndef DDEXAMPLE_DETDATAAGENT_H
#define DDEXAMPLE_DETDATAAGENT_H

#include "GaudiKernel/IDataStoreAgent.h"
#include "GaudiKernel/MsgStream.h"

class IMessageSvc;

/// Data agent class for the Detector Store
class DetDataAgent : virtual public IDataStoreAgent {
public:
  DetDataAgent( IMessageSvc* msgSvc ) : m_msgSvc( msgSvc ) {} 
  virtual ~DetDataAgent() {}
  virtual bool analyse(IRegistry* dir, int level );
private:
  IMessageSvc* m_msgSvc;
};

inline bool DetDataAgent::analyse(IRegistry* dir, int /*level*/ ){
  MsgStream log( m_msgSvc, "detDataAgent" );
  log << MSG::INFO << dir->identifier()
      << ( dir->object() ? " [loaded] " : " " )
      << endmsg; 
  return true;
}

#endif // DDEXAMPLE_DETDATAAGENT_H
