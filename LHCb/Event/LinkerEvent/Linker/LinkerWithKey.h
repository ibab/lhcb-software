// $Id: LinkerWithKey.h,v 1.7 2005-12-15 07:26:02 cattanem Exp $
#ifndef LINKER_LINKERWITHKEY_H 
#define LINKER_LINKERWITHKEY_H 1

// Include files
#include "GaudiKernel/IDataProviderSvc.h"
#include "Event/LinksByKey.h"
#include "GaudiKernel/KeyedObject.h"

/** @class LinkerWithKey LinkerWithKey.h Linker/LinkerWithKey.h
 *  This is a helper for the new type of keyed relations
 *
 *  @author Olivier Callot
 *  @date   2004-01-06
 */
template <class TARGET, class SOURCE=KeyedObject<int> > 
class LinkerWithKey {
protected:
  typedef typename SOURCE::key_type             _sKEY ;
  typedef typename Containers::key_traits<_sKEY> SKEY ;  
  typedef typename TARGET::key_type             _tKEY ;
  typedef typename Containers::key_traits<_tKEY> TKEY ;
public: 
  /// Standard constructor
  LinkerWithKey(IDataProviderSvc* eventSvc,
                IMessageSvc* msgSvc,
                std::string containerName ) {
    m_msgSvc        = msgSvc;
    std::string name = "Link/" + containerName;
    if ( "/Event/" == containerName.substr(0,7) ) {
      name = "Link/" + containerName.substr(7);
    }

    //== If it exists, just append to it.

    SmartDataPtr<LHCb::LinksByKey> links( eventSvc, name );
    if ( 0 != links ) {
      m_links = links;
    } else {
      m_links = new LHCb::LinksByKey();
      StatusCode sc = eventSvc->registerObject( name, m_links );
      if ( !sc ) {
        MsgStream msg( msgSvc, "LinkerWithKey::"+containerName );
        msg << MSG::ERROR << "*** Link container " << name
            << " not registered, Status " << sc << endreq;
      }
    }
  }; 

  virtual ~LinkerWithKey( ) {}; ///< Destructor

  void reset() { m_links->reset(); } 

  void link( const SOURCE* source,
             const TARGET* dest, 
             double weight = 1. ) {
    if ( NULL == source ) return;
    if ( NULL == dest   ) return;
    
    int srcKey     = SKEY::identifier ( source -> key () ) ;
    int srcLinkID  = m_links->linkID( source->parent() );
    int destKey    = TKEY::identifier ( dest   -> key () ) ;
    int destLinkID = m_links->linkID( dest->parent() );
    
    m_links->addReference( srcKey, srcLinkID, destKey, destLinkID, weight );
  }
  
  void link( int key ,
             const TARGET* dest , 
             double weight = 1. ) {
    if ( NULL == dest   ) return;
    int destKey    = TKEY::identifier ( dest -> key () ) ;
    int destLinkID = m_links->linkID( dest->parent() );
    m_links->addReference( key, -1, destKey, destLinkID, weight );
  }
  
  void setIncreasingWeight() { m_links->setIncreasing(); }
  void setDecreasingWeight() { m_links->setDecreasing(); }
  
protected:

private:
  LHCb::LinksByKey* m_links;
  IMessageSvc*      m_msgSvc;
};
#endif // LINKER_LINKERWITHKEYNEW_H
