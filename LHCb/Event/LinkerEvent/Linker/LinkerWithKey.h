// $Id: LinkerWithKey.h,v 1.1.1.1 2004-01-08 12:24:33 ocallot Exp $
#ifndef LINKER_LINKERWITHKEY_H 
#define LINKER_LINKERWITHKEY_H 1

// Include files
#include "GaudiKernel/IDataProviderSvc.h"
#include "Event/LinksByKey.h"
#include "Event/KeyedObject.h"

/** @class LinkerWithKey LinkerWithKey.h Linker/LinkerWithKey.h
 *  This is a helper for the new type of keyed relations
 *
 *  @author Olivier Callot
 *  @date   2004-01-06
 */
template <class TARGET, class SOURCE=KeyedObject<int> > 
class LinkerWithKey {
public: 
  /// Standard constructor
  LinkerWithKey(IDataProviderSvc* eventSvc,
                IMessageSvc* msgSvc,
                std::string containerName ) {
    m_msgSvc        = msgSvc;
    m_containerName = containerName;
    m_links = new LinksByKey();
    StatusCode sc = eventSvc->registerObject( "MC/"+containerName, m_links );
    if ( !sc ) {
      MsgStream msg( msgSvc, "LinkerToMCP::"+containerName );
      msg << MSG::ERROR << "*** Link container MC/" << containerName 
          << " not registered, Status " << sc << endreq;
    }
  }; 

  virtual ~LinkerWithKey( ) {}; ///< Destructor

  void link( const SOURCE* source,
             const TARGET* dest, 
             double weight = 1. ) {
    int srcKey     = source->key();
    int srcLinkID  = m_links->linkID( source->parent() );
    int destKey    = dest->key();
    int destLinkID = m_links->linkID( dest->parent() );
    
    m_links->addReference( srcKey, srcLinkID, destKey, destLinkID, weight );
  }

  void link( int key,
             const TARGET* dest, 
             double weight = 1. ) {
    int destKey    = dest->key();
    int destLinkID = m_links->linkID( dest->parent() );
    m_links->addReference( key, -1, destKey, destLinkID, weight );
  }
  
  void setIncreasingWeight() { m_links->setIncreasing(); }
  void setDecreasingWeight() { m_links->setDecreasing(); }
  
protected:

private:
  std::string    m_containerName;
  LinksByKey*    m_links;
  IMessageSvc*   m_msgSvc;
};
#endif // LINKER_LINKERWITHKEY_H
