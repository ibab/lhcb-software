// $Id: LinkedTo.h,v 1.1.1.1 2004-01-08 12:24:33 ocallot Exp $
#ifndef LINKER_LINKEDTO_H 
#define LINKER_LINKEDTO_H 1

// Include files
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/LinkManager.h"
#include "Event/LinksByKey.h"

/** @class LinkedTo LinkedTo.h Linker/LinkedTo.h
 *  Templated interface to the LinksByKey, direct version
 *
 *  @author Olivier Callot
 *  @date   2004-01-06
 */

template <class TARGET, 
          class SOURCE=KeyedObject<int>,
          class TARGETCONTAINER=KeyedContainer<TARGET> > class LinkedTo {
public: 
  /// Standard constructor
  LinkedTo( IDataProviderSvc* eventSvc,
                IMessageSvc* msgSvc,
                std::string containerName ) {
    m_containerName = containerName;
    SmartDataPtr<LinksByKey> links( eventSvc, "MC/"+containerName );
    if ( 0 == links ) {
      MsgStream msg( msgSvc, "LinkedTo::"+containerName );
      msg << MSG::ERROR << "*** Link container MC/" << containerName 
          << " not found." << endreq;
    }
    m_links = links;
    m_curReference.setNextIndex( -1 );
  }; 

  virtual ~LinkedTo( ) {}; ///< Destructor

  TARGET* first( const SOURCE* source ) {
    bool status = m_links->firstReference( source->key(), 
                                           source->parent(),
                                           m_curReference   );
    if ( !status ) {
      m_curReference.setNextIndex( -1 );
      return NULL;
    }
    return currentTarget();
  }
  
  TARGET* first( int key ) {
    bool status = m_links->firstReference( key, NULL, m_curReference );
    if ( !status ) {
      m_curReference.setNextIndex( -1 );
      return NULL;
    }
    return currentTarget();
  }
  
  TARGET* next( ) {
    bool status = m_links->nextReference( m_curReference );
    if ( !status ) {
      m_curReference.setNextIndex( -1 );
      return NULL;
    }
    return currentTarget();
  }
  
  double weight()   { return m_curReference.weight(); }

protected:

  TARGET* currentTarget() {
    int myLinkID = m_curReference.linkID();
    LinkManager::Link* link = m_links->linkMgr()->link( myLinkID );
    TARGETCONTAINER* parent = dynamic_cast< TARGETCONTAINER* >(link->object() );
    if ( 0 != parent ) {
      TARGET* myObj = parent->object( m_curReference.objectKey() );
      return myObj;
    }
    return NULL;
  }  

private:
  std::string      m_containerName;
  LinksByKey*      m_links;
  LinkReference    m_curReference;
};
#endif // LINKER_LINKEDTO_H
