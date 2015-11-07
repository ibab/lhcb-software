#ifndef LINKER_ALLLINKS_H 
#define LINKER_ALLLINKS_H 1

// STL
#include <sstream>

// Include files
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/ObjectContainerBase.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/GaudiException.h"
#include "Event/LinksByKey.h"

/** @class AllLinks AllLinks.h Linker/AllLinks.h
 *  Templated interface to the LinksByKey, direct version
 *
 *  @author Olivier Callot
 *  @date   2004-01-06
 */

template < class TARGET, class SOURCE=ContainedObject >
class AllLinks {
public: 
  
  /// Standard constructor
  AllLinks( IDataProviderSvc* eventSvc,
            IMessageSvc* msgSvc,
            std::string containerName ) {
    m_eventSvc = eventSvc;          
    std::string name = "Link/" + containerName;
    if ( containerName.compare(0,7,"/Event/") == 0 ) {
      name = "Link/" + containerName.substr(7);
    }
    SmartDataPtr<LHCb::LinksByKey> links( eventSvc, name );
    if ( !links ) {
      if ( msgSvc ) {
        MsgStream msg( msgSvc, "AllLinks::"+containerName );
        msg << MSG::ERROR << "*** Link container " << name
            << " not found." << endmsg;
      }
    } else {
      //== Check proper template, only if specified. 
      if ( links->sourceClassID() != SOURCE::classID() &&
           CLID_ContainedObject   != SOURCE::classID()  ) {
        std::ostringstream message;
        message << "Incompatible SOURCE type for location " << containerName
                << " : Template classID is " << SOURCE::classID() << " expected " << links->sourceClassID();
        throw GaudiException( message.str(), "AllLinks", StatusCode::FAILURE );
      }
      if ( links->targetClassID() != TARGET::classID() ) {
        std::ostringstream message;
        message << "Incompatible TARGET type for location " << containerName
                << " : Template classID is " << TARGET::classID() << " expected " << links->targetClassID();
        throw GaudiException( message.str(), "AllLinks", StatusCode::FAILURE );
      }
    }
    m_links = links;
    m_curReference.setNextIndex( -1 );
    m_curReference.setWeight( 0. );
  }; 
            
  virtual ~AllLinks( ) {}; ///< Destructor
  
  bool notFound() const { return !m_links; }

  TARGET* first( ) {
    if ( !m_links ) return nullptr;
    m_iter = m_links->keyIndex().begin();
    if ( m_links->keyIndex().end() == m_iter ) return nullptr;
    m_curReference.setNextIndex( (*m_iter).second );
    return next();
  }
    
  TARGET* next( ) {
    if ( !m_links ) return nullptr;
    int nn = m_curReference.nextIndex();
    while ( 0 > nn ) {
      if ( m_links->keyIndex().end() == m_iter ) return nullptr;
      m_iter++;
      if ( m_links->keyIndex().end() == m_iter ) return nullptr;
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
    if ( !m_links ) return nullptr;
    int myLinkID = m_curReference.linkID();
    LinkManager::Link* link = m_links->linkMgr()->link( myLinkID );
    if ( 0 == link->object() ) {
      SmartDataPtr<DataObject> tmp( m_eventSvc, link->path() );
      link->setObject( tmp );
      if ( !tmp ) return nullptr;
    }
    ObjectContainerBase* parent = dynamic_cast<ObjectContainerBase*>(link->object() );
    return parent ?  (TARGET*)parent->containedObject( m_curReference.objectKey() ) : nullptr;
  }  

 SOURCE* currentSource( int key ) {
    if ( !m_links ) return nullptr;
    int myLinkID = m_curReference.srcLinkID();
    if ( 0 > myLinkID ) return nullptr;
    LinkManager::Link* link = m_links->linkMgr()->link( myLinkID );
    if ( 0 == link->object() ) {
      SmartDataPtr<DataObject> tmp( m_eventSvc, link->path() );
      link->setObject( tmp );
      if ( !tmp ) return nullptr;
    }
    ObjectContainerBase* parent = dynamic_cast<ObjectContainerBase*>(link->object() );
    return  parent ? (SOURCE*)parent->containedObject( key ) : nullptr;
  }  

private:
  IDataProviderSvc*   m_eventSvc;
  LHCb::LinksByKey*   m_links;
  LHCb::LinkReference m_curReference;
  std::vector<std::pair<int,int> >::const_iterator m_iter;
};
#endif // LINKER_ALLLINKS_H
