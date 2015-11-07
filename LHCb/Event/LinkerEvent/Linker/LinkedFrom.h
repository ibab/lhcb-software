#ifndef LINKER_LINKEDFROM_H 
#define LINKER_LINKEDFROM_H 1

// STL
#include <sstream>

// Include files
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/ObjectContainerBase.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/GaudiException.h"
#include "Event/LinksByKey.h"

/** @class LinkedFrom LinkedFrom.h Linker/LinkedFrom.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2004-01-06
 */

template <class SOURCE, class TARGET=ContainedObject, class KEY=int>
class LinkedFrom {
public: 
  //== Typedefs to please Matt
  typedef typename std::vector<SOURCE*>                  LRange;
  typedef typename std::vector<SOURCE*>::const_iterator  LRangeIt;

  /// Standard constructor
  LinkedFrom(  IDataProviderSvc* eventSvc,
               IMessageSvc* msgSvc,
               std::string containerName ) {
    m_eventSvc = eventSvc;
    std::string name = "Link/" + containerName;
    if ( "/Event/" == containerName.substr(0,7) ) {
      name = "Link/" + containerName.substr(7);
    }
    SmartDataPtr<LHCb::LinksByKey> links( eventSvc, name );
    if ( !links ) {
      if ( msgSvc ) {
        MsgStream msg( msgSvc, "LinkedFrom::"+containerName );
        msg << MSG::ERROR << "*** Link container " << name
            << " not found." << endmsg;
      }
    } else {
      //== Check proper template, only if specified. 
      if ( links->sourceClassID() != SOURCE::classID() &&
           CLID_ContainedObject   != SOURCE::classID() ) {
        std::ostringstream message;
        message << "Incompatible SOURCE type for location " << containerName
                << " : Template classID is " << SOURCE::classID() << " expected " << links->sourceClassID();
        throw GaudiException( message.str(), "LinkedFrom", StatusCode::FAILURE );
      }
      if ( links->targetClassID() != TARGET::classID() &&
           CLID_ContainedObject   != TARGET::classID() ) {
        std::ostringstream message;
        message << "Incompatible TARGET type for location " << containerName
                << " : Template classID is " << TARGET::classID() << " expected " << links->targetClassID();
        throw GaudiException( message.str(), "LinkedFrom", StatusCode::FAILURE );
      }
    }
    m_links = links;
    m_curReference.setNextIndex( -1 );
    m_curReference.setWeight( 0. );
    m_wantedKey = -1;
  };  

  virtual ~LinkedFrom( ) {}; ///< Destructor

  bool notFound() const { return (0 == m_links); }

  SOURCE* first( const TARGET* target ) {
    if ( !m_links ) return nullptr;
    m_curReference.setLinkID( -1 );
    m_wantedKey = target->index() ;
    //== check that the target's container is known.
    const DataObject* container = target->parent();
    LinkManager::Link* link = m_links->linkMgr()->link( container );
    if ( !link ) {  // try with name, and store pointer if OK
      link = m_links->linkMgr()->link( container->registry()->identifier() ); 
      if ( link )  link->setObject( container );
    }
    if ( !link ) return nullptr;
    //== Define the target's linkID and Index
    m_curReference.setLinkID( short(link->ID()) );
    m_curReference.setObjectKey( target->index() );
    int index = m_links->firstSource( m_curReference, m_srcIterator );
    if ( m_wantedKey != m_curReference.objectKey() ) return nullptr;
    return currentSource( index );
  };

  SOURCE* next( ) {
    if ( !m_links ) return nullptr;
    int index = m_links->nextSource( m_curReference, m_srcIterator );
    if ( m_wantedKey != m_curReference.objectKey() ) return nullptr;
    return currentSource( index );
  };

  double weight()   { return m_curReference.weight(); }

  /** returns a vector of targets, onto which STL functions can be used.
   */
  LRange& range( const TARGET* target ) {
    m_vect.clear();
    auto tmp = first( target );
    while ( tmp ) {
      m_vect.push_back( tmp );
      tmp = next();
    }
    return m_vect;
  }

  /** returns a vector of keys, for int linked to TARGET. */
  std::vector<KEY>& keyRange( const TARGET* target ) {
    m_keys.clear();
    if ( !m_links ) return m_keys;
    m_curReference.setLinkID( -1 );
    m_wantedKey = target->index();
    //== check that the target's container is known.
    const DataObject* container = target->parent();
    LinkManager::Link* link = m_links->linkMgr()->link( container );
    if ( !link ) {  // try with name, and store pointer if OK
      link = m_links->linkMgr()->link( container->registry()->identifier() ); 
      if ( link )  link->setObject( container );
    }
    if ( !link ) return m_keys;
    //== Define the target's linkID and index
    m_curReference.setLinkID( short(link->ID()) );
    m_curReference.setObjectKey( target->index() );
    int key = m_links->firstSource( m_curReference, m_srcIterator );
    while ( m_wantedKey == m_curReference.objectKey() ) {
      m_keys.emplace_back( key );
      key = m_links->nextSource( m_curReference, m_srcIterator );
    }
    return m_keys;
  };

  LRangeIt beginRange()   { return m_vect.begin(); }
  LRangeIt endRange()     { return m_vect.end(); }

protected:
  SOURCE* currentSource( int index ) {
    if ( !m_links ) return nullptr;
    int myLinkID = m_curReference.srcLinkID();
    LinkManager::Link* link = m_links->linkMgr()->link( myLinkID );
    if ( 0 == link->object() ) {
      SmartDataPtr<DataObject> tmp( m_eventSvc, link->path() );
      link->setObject( tmp );
      if ( !tmp ) return nullptr;
    }
    ObjectContainerBase* parent = dynamic_cast<ObjectContainerBase*>(link->object() );
    return parent ? static_cast<SOURCE*>(parent->containedObject( index )) : nullptr;
  }  

private:
  IDataProviderSvc*                  m_eventSvc;
  LHCb::LinksByKey*                  m_links;
  LHCb::LinkReference                m_curReference;
  std::vector<std::pair<int,int> >::const_iterator m_srcIterator;
  int                                m_wantedKey;
  LRange                             m_vect;
  std::vector<KEY>                   m_keys;
};
#endif // LINKER_LINKEDFROM_H
