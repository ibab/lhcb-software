// $Id: AllLinks.h,v 1.2 2005-01-27 10:53:59 ocallot Exp $
#ifndef LINKER_ALLLINKS_H 
#define LINKER_ALLLINKS_H 1

// Include files
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/KeyedObject.h"
#include "Event/LinksByKey.h"

/** @class AllLinks AllLinks.h Linker/AllLinks.h
 *  Templated interface to the LinksByKey, direct version
 *
 *  @author Olivier Callot
 *  @date   2004-01-06
 */

template < class TARGET, 
           class SOURCE=KeyedObject<int>,
           class TARGETCONTAINER=KeyedContainer<TARGET>,
           class SOURCECONTAINER=KeyedContainer<SOURCE> > 
class AllLinks {
public: 
  
  /// Standard constructor
  AllLinks( IDataProviderSvc* eventSvc,
            IMessageSvc* msgSvc,
            std::string containerName ) {
    m_eventSvc = eventSvc;          
    std::string name = "Link/" + containerName;
    if ( "/Event/" == containerName.substr(0,7) ) {
      name = "Link/" + containerName.substr(7);
    }
    SmartDataPtr<LinksByKey> links( eventSvc, name );
    if ( 0 == links ) {
      if ( 0 != msgSvc ) {
        MsgStream msg( msgSvc, "AllLinks::"+containerName );
        msg << MSG::ERROR << "*** Link container " << name
            << " not found." << endreq;
      }
    }
    m_links = links;
    m_curReference.setNextIndex( -1 );
    m_curReference.setWeight( 0. );
  }; 
            
  virtual ~AllLinks( ) {}; ///< Destructor
  
  bool notFound() const { return (0 == m_links); }

  TARGET* first( ) {
    if ( NULL == m_links ) return NULL;
    m_iter = m_links->keyIndex().begin();
    if ( m_links->keyIndex().end() == m_iter ) return NULL;
    m_curReference.setNextIndex( (*m_iter).second );
    return next();
  }
    
  TARGET* next( ) {
    if ( NULL == m_links ) return NULL;
    int nn = m_curReference.nextIndex();
    while ( 0 > nn ) {
      if ( m_links->keyIndex().end() == m_iter ) return NULL;
      m_iter++;
      if ( m_links->keyIndex().end() == m_iter ) return NULL;
      nn = (*m_iter).second;
    }
    m_curReference = m_links->linkReference()[nn] ;
    return currentTarget();
  }
  
  double weight()   { return m_curReference.weight(); }
  int    key()      { return (*m_iter).first; }
  SOURCE* source()  { return currentSource(  (*m_iter).first ); }

protected:

  TARGET* currentTarget() {
    if ( NULL == m_links ) return NULL;
    int myLinkID = m_curReference.linkID();
    LinkManager::Link* link = m_links->linkMgr()->link( myLinkID );
    if ( 0 == link->object() ) {
      SmartDataPtr<DataObject> tmp( m_eventSvc, link->path() );
      link->setObject( tmp );
      if ( 0 == tmp ) return NULL;
    }
   TARGETCONTAINER* parent = dynamic_cast< TARGETCONTAINER* >(link->object() );
    if ( 0 != parent ) {
      TARGET* myObj = parent->object( m_curReference.objectKey() );
      return myObj;
    }
    return NULL;
  }  

 SOURCE* currentSource( int key ) {
    if ( NULL == m_links ) return NULL;
    int myLinkID = m_curReference.srcLinkID();
    if ( 0 > myLinkID ) return NULL;
    LinkManager::Link* link = m_links->linkMgr()->link( myLinkID );
    if ( 0 == link->object() ) {
      SmartDataPtr<DataObject> tmp( m_eventSvc, link->path() );
      link->setObject( tmp );
      if ( 0 == tmp ) return NULL;
    }
    SOURCECONTAINER* parent = dynamic_cast< SOURCECONTAINER* >(link->object() );
    if ( 0 != parent ) {
      return parent->object( key );
    }
    return NULL;
  }  

private:
  IDataProviderSvc* m_eventSvc;
  LinksByKey*      m_links;
  LinkReference    m_curReference;
  std::vector<std::pair<int,int> >::const_iterator m_iter;
};
#endif // LINKER_ALLLINKS_H
