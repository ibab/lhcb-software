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
  virtual bool analyse(IDataDirectory* dir, int level );
private:
  IMessageSvc* m_msgSvc;
};

inline bool DetDataAgent::analyse(IDataDirectory* dir, int level ){
  MsgStream log( m_msgSvc, "detDataAgent" );
  log << MSG::INFO << dir->fullpath() << ( dir->object() ? " [loaded] " : " " ) << endreq; 
  return true;
}

#endif // DDEXAMPLE_DETDATAAGENT_H
