// $Id: LinkerWithKey.h,v 1.12 2006-03-15 13:09:16 ocallot Exp $
#ifndef LINKER_LINKERWITHKEY_H 
#define LINKER_LINKERWITHKEY_H 1

// Include files
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "Event/LinksByKey.h"
#include "GaudiKernel/ContainedObject.h"

/** @class LinkerWithKey LinkerWithKey.h Linker/LinkerWithKey.h
 *  This is a helper for the new type of relations
 *
 *  @author Olivier Callot
 *  @date   2004-01-06
 */
template <class TARGET, class SOURCE=ContainedObject > 
class LinkerWithKey {
public: 
  /// Standard constructor
  LinkerWithKey( IDataProviderSvc* evtSvc,
                 IMessageSvc* msgSvc,
                 std::string containerName ) {
    m_msgSvc        = msgSvc;
    std::string name = "Link/" + containerName;
    if ( "/Event/" == containerName.substr(0,7) ) {
      name = "Link/" + containerName.substr(7);
    }

    //== If it exists, just append to it.

    SmartDataPtr<LHCb::LinksByKey> links( evtSvc, name );
    if ( 0 != links ) {
      m_links = links;
    } else {
      m_links = new LHCb::LinksByKey();
      StatusCode sc = evtSvc->registerObject( name, m_links );
      if ( !sc ) {
        MsgStream msg( msgSvc, "LinkerWithKey::"+containerName );
        msg << MSG::ERROR << "*** Link container " << name
            << " not registered, Status " << sc << endreq;
      }
    }

    m_links->setTargetClassID( TARGET::classID() );
    m_links->setSourceClassID( SOURCE::classID() );
  }; 

  virtual ~LinkerWithKey( ) {}; ///< Destructor

  void reset() { m_links->reset(); } 

  void link( const SOURCE* source,
             const TARGET* dest, 
             double weight = 1. ) {
    if ( NULL == source ) return;
    if ( NULL == dest   ) return;
    
    int srcIndex   = source->index ();
    int srcLinkID  = m_links->linkID( source->parent() );
    int destIndex  = dest->index();
    int destLinkID = m_links->linkID( dest->parent() );
    
    m_links->addReference( srcIndex, srcLinkID, destIndex, destLinkID, weight );
  }
  
  void link( int key , const TARGET* dest , double weight = 1. ) {
    if ( NULL == dest   ) return;
    int destIndex  = dest->index ();
    int destLinkID = m_links->linkID( dest->parent() );
    m_links->addReference( key, -1, destIndex, destLinkID, weight );
  }
  
  void setIncreasingWeight() { m_links->setIncreasing(); }
  void setDecreasingWeight() { m_links->setDecreasing(); }
  
protected:
  
private:
  LHCb::LinksByKey*  m_links;
  IMessageSvc* m_msgSvc;
};
#endif // LINKER_LINKERWITHKEYNEW_H
