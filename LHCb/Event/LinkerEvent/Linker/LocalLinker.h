// $Id: LocalLinker.h,v 1.1 2008-03-10 14:41:45 ocallot Exp $
#ifndef LINKER_LOCALLINKER_H 
#define LINKER_LOCALLINKER_H 1

// Include files
// Include files
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "Event/LinksByKey.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

/** @class LocalLinker LocalLinker.h Linker/LocalLinker.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2008-03-10
 */
template <class TARGET, class SOURCE > 
class LocalLinker {
public: 

  /// Standard constructor
  LocalLinker( IDataProviderSvc* evtSvc ) {
    m_eventSvc = evtSvc;
    m_links.setTargetClassID( TARGET::classID() );
    m_links.setSourceClassID( SOURCE::classID() );
    m_curReference.setNextIndex( -1 );
    m_curReference.setWeight( 0. );
  }; 

  virtual ~LocalLinker( ); ///< Destructor

  void link( const SOURCE* source,
             const TARGET* dest, 
             double weight = 1. ) {
    if ( NULL == source ) return;
    if ( NULL == dest   ) return;
    
    int srcIndex   = source->index ();
    int srcLinkID  = m_links.linkID( source->parent() );
    int destIndex  = dest->index();
    int destLinkID = m_links.linkID( dest->parent() );
    
    m_links.addReference( srcIndex, srcLinkID, destIndex, destLinkID, weight );
  }
  
  void setIncreasingWeight() { m_links.setIncreasing(); }
  void setDecreasingWeight() { m_links.setDecreasing(); }

   /** returns the first target related to the specified source.
   */
  TARGET* first( const SOURCE* source ) {
    bool status = m_links.firstReference ( source->index(), source->parent(), m_curReference );
    if ( !status ) {
      m_curReference.setNextIndex( -1 );
      return NULL;
    }
    return currentTarget();
  }
  
  /** returns the next target. The source of the relation was specified by a 
      previous call to 'first'
   */
   TARGET* next( ) {
    bool status = m_links.nextReference( m_curReference );
    if ( !status ) {
      m_curReference.setNextIndex( -1 );
      return NULL;
    }
    return currentTarget();
  }
  /** returns the weight of the previously defined relation */
  double weight()   { return m_curReference.weight(); }

 SOURCE* firstSource( const TARGET* target ) {
    m_curReference.setLinkID( -1 );
    m_wantedKey = target->index() ;
    //== check that the target's container is known.
    const DataObject* container = target->parent();
    LinkManager::Link* link = m_links.linkMgr()->link( container );
    if ( 0 == link ) {  // try with name, and store pointer if OK
      link = m_links.linkMgr()->link( container->registry()->identifier() ); 
      if ( 0 != link )  link->setObject( container );
    }
    if ( 0 == link ) return NULL;
    //== Define the target's linkID and Index
    m_curReference.setLinkID( short(link->ID()) );
    m_curReference.setObjectKey( target->index() );
    int index = m_links.firstSource( m_curReference, m_srcIterator );
    if ( m_wantedKey != m_curReference.objectKey() ) return NULL;
    return currentSource( index );
  };

  SOURCE* nextSource( ) {
    int index = m_links.nextSource( m_curReference, m_srcIterator );
    if ( m_wantedKey != m_curReference.objectKey() ) return NULL;
    return currentSource( index );
  };

protected:

  TARGET* currentTarget() {
    int myLinkID = m_curReference.linkID();
    LinkManager::Link* link = m_links.linkMgr()->link( myLinkID );
    if ( 0 == link->object() ) {
      SmartDataPtr<DataObject> tmp( m_eventSvc, link->path() );
      link->setObject( tmp );
      if ( 0 == tmp ) return NULL;
    }
    ObjectContainerBase* parent = dynamic_cast<ObjectContainerBase*>(link->object() );
    if ( 0 != parent ) {
      TARGET* myObj = (TARGET*)parent->containedObject( m_curReference.objectKey() );
      return myObj;
    }
    return NULL;
  }
  
  SOURCE* currentSource( int index ) {
    int myLinkID = m_curReference.srcLinkID();
    LinkManager::Link* link = m_links.linkMgr()->link( myLinkID );
    if ( 0 == link->object() ) {
      SmartDataPtr<DataObject> tmp( m_eventSvc, link->path() );
      link->setObject( tmp );
      if ( 0 == tmp ) return NULL;
    }
    ObjectContainerBase* parent = dynamic_cast<ObjectContainerBase*>(link->object() );
    if ( 0 != parent ) {
      return (SOURCE*)parent->containedObject( index );
    }
    return NULL;
  }

private:
  IDataProviderSvc*   m_eventSvc;
  LHCb::LinksByKey    m_links;
  LHCb::LinkReference m_curReference;
  int                 m_wantedKey;
  std::vector<std::pair<int,int> >::const_iterator m_srcIterator;
};
#endif // LINKER_LOCALLINKER_H
