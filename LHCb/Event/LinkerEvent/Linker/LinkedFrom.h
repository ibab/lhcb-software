// $Id: LinkedFrom.h,v 1.3 2004-02-05 08:46:29 ocallot Exp $
#ifndef LINKER_LINKEDFROM_H 
#define LINKER_LINKEDFROM_H 1

// Include files
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/KeyedObject.h"
#include "Event/LinksByKey.h"

/** @class LinkedFrom LinkedFrom.h Linker/LinkedFrom.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2004-01-06
 */
template <class SOURCE, 
          class TARGET=KeyedObject<int> ,
          class SOURCECONTAINER=KeyedContainer<SOURCE> > class LinkedFrom {
public: 
  /// Standard constructor
  LinkedFrom(  IDataProviderSvc* eventSvc,
                   IMessageSvc* msgSvc,
                   std::string containerName ) {
    std::string name = "Link/" + containerName;
    if ( "/Event/" == containerName.substr(0,7) ) {
      name = "Link/" + containerName.substr(7);
    }
    SmartDataPtr<LinksByKey> links( eventSvc, name );
    if ( 0 == links ) {
      if ( 0 != msgSvc ) {
        MsgStream msg( msgSvc, "LinkedFrom::"+containerName );
        msg << MSG::ERROR << "*** Link container Link/" << name
            << " not found." << endreq;
      }
    } else {
      links->resolveLinks( eventSvc );
    }
    m_links = links;
    m_curReference.setNextIndex( -1 );
    m_curReference.setWeight( 0. );
  };  

  virtual ~LinkedFrom( ) {}; ///< Destructor

  bool notFound() const { return (0 == m_links); }

  SOURCE* first( const TARGET* target ) {
    if ( NULL == m_links ) return NULL;
    m_curReference.setLinkID( -1 );
    m_wantedKey = target->key();
    //== check that the target's container is known.
    const DataObject* container = target->parent();
    LinkManager::Link* link = m_links->linkMgr()->link( container );
    if ( 0 == link ) return NULL;
    //== Define the target's linkID and key
    m_curReference.setLinkID( link->ID() );
    m_curReference.setObjectKey( target->key() );
    int key = m_links->firstSource( m_curReference, m_srcIterator );
    if ( m_wantedKey != m_curReference.objectKey() ) return NULL;
    return currentSource( key );
  };

  SOURCE* next( ) {
    if ( NULL == m_links ) return NULL;
    int key = m_links->nextSource( m_curReference, m_srcIterator );
    if ( m_wantedKey != m_curReference.objectKey() ) return NULL;
    return currentSource( key );
  };

  double weight()   { return m_curReference.weight(); }

protected:
  SOURCE* currentSource( int key ) {
    if ( NULL == m_links ) return NULL;
    int myLinkID = m_curReference.srcLinkID();
    LinkManager::Link* link = m_links->linkMgr()->link( myLinkID );
    SOURCECONTAINER* parent = dynamic_cast< SOURCECONTAINER* >(link->object() );
    if ( 0 != parent ) {
      return parent->object( key );
    }
    return NULL;
  }  

private:
  LinksByKey*                        m_links;
  LinkReference                      m_curReference;
  std::map<int,int>::const_iterator m_srcIterator;
  int                                m_wantedKey;
};
#endif // LINKER_LINKEDFROM_H
