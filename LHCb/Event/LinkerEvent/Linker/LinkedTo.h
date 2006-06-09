// $Id: LinkedTo.h,v 1.16 2006-06-09 09:22:15 cattanem Exp $
#ifndef LINKER_LINKEDTO_H 
#define LINKER_LINKEDTO_H 1

// Include files
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/ObjectContainerBase.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/GaudiException.h"
#include "Event/LinksByKey.h"

/** @class LinkedTo LinkedTo.h Linker/LinkedTo.h
 *  Templated interface to the LinksByKey, direct version
 *
 *  @author Olivier Callot
 *  @date   2004-01-06
 */


template < class TARGET, class SOURCE=ContainedObject>
class LinkedTo {
public: 
  //== Typedefs to please Matt
  typedef typename std::vector<TARGET*>                  LRange;
  typedef typename std::vector<TARGET*>::const_iterator  LRangeIt;

  /// Standard constructor
  LinkedTo( IDataProviderSvc* eventSvc,
            IMessageSvc* msgSvc,
            std::string containerName ) {
    m_eventSvc = eventSvc;
    std::string name = "Link/" + containerName;
    if ( "/Event/" == containerName.substr(0,7) ) {
      name = "Link/" + containerName.substr(7);
    }
    SmartDataPtr<LHCb::LinksByKey> links( eventSvc, name );
    if ( 0 == links ) {
      if ( 0 != msgSvc ) {
        MsgStream msg( msgSvc, "LinkedTo::"+containerName );
        msg << MSG::ERROR << "*** Link container " << name
            << " not found." << endreq;
      }
    } else {
      //== Check proper template, only if specified. 
      if ( links->sourceClassID() != SOURCE::classID() &&
           CLID_ContainedObject   != SOURCE::classID()  ) {
        throw GaudiException( "Incompatible SOURCE type for location " + containerName,
                              "LinkedTo", StatusCode::FAILURE);
      }
      if ( links->targetClassID() != TARGET::classID() ) {
        throw GaudiException( "Incompatible TARGET type for location " + containerName,
                              "LinkedTo", StatusCode::FAILURE);
      }
    }
    m_links = links;
    m_curReference.setNextIndex( -1 );
    m_curReference.setWeight( 0. );
  }; 
            
  virtual ~LinkedTo( ) {}; ///< Destructor
  
  bool notFound() const { return (0 == m_links); }

   /** returns the first target related to the specified source.
   */
  TARGET* first( const SOURCE* source ) {
    if ( NULL == m_links ) return NULL;
    bool status = m_links->firstReference ( source->index(), source->parent(), m_curReference );
    if ( !status ) {
      m_curReference.setNextIndex( -1 );
      return NULL;
    }
    return currentTarget();
  }
  
   /** returns the first target related to the source specified as a key.
   */
  TARGET* first( int key ) {
    if ( NULL == m_links ) return NULL;
    bool status = m_links->firstReference( key, NULL, m_curReference );
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
    if ( NULL == m_links ) return NULL;
    bool status = m_links->nextReference( m_curReference );
    if ( !status ) {
      m_curReference.setNextIndex( -1 );
      return NULL;
    }
    return currentTarget();
  }
  /** returns the weight of the previously defined relation */
  double weight()   { return m_curReference.weight(); }

  /** returns a vector of targets, onto which STL functions can be used.
   */
  LRange& range( const SOURCE* source ) {
    m_vect.clear();
    TARGET* tmp = first( source );
    while ( NULL != tmp ) {
      m_vect.push_back( tmp );
      tmp = next();
    }
    return m_vect;
  }

  /** returns a vector of targets, onto which STL functions can be used.
   */
  LRange& range( int key ) {
    m_vect.clear();
    TARGET* tmp = first( key );
    while ( NULL != tmp ) {
      m_vect.push_back( tmp );
      tmp = next();
    }
    return m_vect;
  }

  LRangeIt beginRange()   { return m_vect.begin(); }
  LRangeIt endRange()     { return m_vect.end(); }

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
    ObjectContainerBase* parent = dynamic_cast<ObjectContainerBase*>(link->object() );
    if ( 0 != parent ) {
      TARGET* myObj = (TARGET*)parent->containedObject( m_curReference.objectKey() );
      return myObj;
    }
    return NULL;
  }  

private:
  IDataProviderSvc*   m_eventSvc;
  LHCb::LinksByKey*   m_links;
  LHCb::LinkReference m_curReference;
  LRange              m_vect;          
};
#endif // LINKER_LINKEDTO_H
