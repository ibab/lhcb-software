//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetCondExample/src/DetDataAgent.h,v 1.1.1.1 2001-09-14 15:45:02 andreav Exp $
#ifndef DETCONDEXAMPLE_DETDATAAGENT_H
#define DETCONDEXAMPLE_DETDATAAGENT_H

#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IDataStoreAgent.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/RegistryEntry.h"

class IMessageSvc;

///---------------------------------------------------------------------------
/** @class DetDataAgent DetDataAgent.h DetCondExample/DetDataAgent.h

    Simple agent to dump the contents of DetDataSvc TDS.
    Copied from Examples/DDexample.

    @author Andrea Valassi 
    @date February 2001
*///--------------------------------------------------------------------------

class DetDataAgent : virtual public IDataStoreAgent                        {

 public:
  
  DetDataAgent( IMessageSvc* msgSvc );
  
  virtual ~DetDataAgent();
  
  virtual bool analyse(IDataDirectory* dir, int level );
  
 private:
  
  IMessageSvc* m_msgSvc;

};

//---------------------------------------------------------------------------

inline DetDataAgent::DetDataAgent( IMessageSvc* msgSvc )
  : m_msgSvc( msgSvc ) {
}

//---------------------------------------------------------------------------

inline DetDataAgent::~DetDataAgent( ) { }

//---------------------------------------------------------------------------

inline bool DetDataAgent::analyse(IDataDirectory* dir, int level ) {
  
  MsgStream log( m_msgSvc, "DetDataAgent" );
  
  if ( level == 0 ) 
    log << MSG::INFO 
	<< "Dumping the contents of the DetectorDataSvc store" << endreq;

  log << MSG::INFO << "(" << level << ")" << dir->fullpath();
  
  if( 0 != dir->object() ) {
    log << " with class ID " << dir->object()->clID() << endreq;
  } else {
    log << endreq;
  }
  
  return true;

}

//---------------------------------------------------------------------------

#endif // DETCONDEXAMPLE_DETDATAAGENT_H
