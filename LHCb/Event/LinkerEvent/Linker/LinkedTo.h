// $Id: LinkedTo.h,v 1.3 2004-02-05 08:46:29 ocallot Exp $
#ifndef LINKER_LINKEDTO_H 
#define LINKER_LINKEDTO_H 1

// Include files
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/KeyedObject.h"
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
    std::string name = "Link/" + containerName;
    if ( "/Event/" == containerName.substr(0,7) ) {
      name = "Link/" + containerName.substr(7);
    }
    SmartDataPtr<LinksByKey> links( eventSvc, name );
    if ( 0 == links ) {
      if ( 0 != msgSvc ) {
        MsgStream msg( msgSvc, "LinkedTo::"+containerName );
        msg << MSG::ERROR << "*** Link container " << name
            << " not found." << endreq;
      }
    } else {
      links->resolveLinks( eventSvc );
    }
    m_links = links;
    m_curReference.setNextIndex( -1 );
    m_curReference.setWeight( 0. );
  }; 
            
  virtual ~LinkedTo( ) {}; ///< Destructor
  
  bool notFound() const { return (0 == m_links); }

  TARGET* first( const SOURCE* source ) {
    if ( NULL == m_links ) return NULL;
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
    if ( NULL == m_links ) return NULL;
    bool status = m_links->firstReference( key, NULL, m_curReference );
    if ( !status ) {
      m_curReference.setNextIndex( -1 );
      return NULL;
    }
    return currentTarget();
  }
  
  TARGET* next( ) {
    if ( NULL == m_links ) return NULL;
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
    if ( NULL == m_links ) return NULL;
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
  LinksByKey*      m_links;
  LinkReference    m_curReference;
};
#endif // LINKER_LINKEDTO_H
