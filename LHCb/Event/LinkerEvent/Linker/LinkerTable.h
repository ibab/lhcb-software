// $Id: LinkerTable.h,v 1.5 2006-02-14 10:41:52 ocallot Exp $
#ifndef LINKER_LINKERTABLE_H 
#define LINKER_LINKERTABLE_H 1

// Include files
#include "Linker/LinkerRange.h"

/** @class LinkerTable LinkerTable.h Linker/LinkerTable.h
 *  This is a table, the interface to build LinkerRange.
 *
 *  @author Olivier Callot
 *  @date   2005-01-19
 */
template <class SOURCE, class TARGET> class LinkerTable {
public: 

  typedef          LinkerRange<SOURCE,TARGET>           Range;
  typedef typename LinkerRange<SOURCE,TARGET>::iterator iterator;

  /// Standard constructor
  LinkerTable( ) {}; 

  virtual ~LinkerTable( ) {}; ///< Destructor

  /** retrieve all relations in the table
   *  @return       Range with all relations
   */
  Range relations() const {
    Range range;
    if ( NULL == m_links ) return range;
    std::vector<std::pair<int,int> >::const_iterator iter;
    for ( iter = m_links->keyIndex().begin();
          m_links->keyIndex().end() != iter;
          iter++ ) {
      LHCb::LinkReference curReference;
      curReference.setNextIndex( (*iter).second );
      while ( 0 <= curReference.nextIndex()  ) {
        curReference = m_links->linkReference()[curReference.nextIndex()] ;
        SOURCE* src   = currentSource( (*iter).first, curReference );
        if ( 0 != src ) {    // ignore int links
          TARGET* tgt   = currentTarget( curReference );
          double weight = curReference.weight();
          range.addEntry( src, tgt, weight );
        }        
      }
    }
    return range;
  };
  
  /** Returns all the relations in the table for a given object.
   *  @param  reqSrc     Object for which the relations is wanted
      @return Range of relations
   */
  Range relations( const SOURCE* reqSrc ) const {
    Range range;
    if ( NULL == m_links ) return range;
    if ( NULL == reqSrc ) return range;
    LHCb::LinkReference curReference;
    bool status = m_links->firstReference( reqSrc->index(), 
                                           reqSrc->parent(),
                                           curReference   );
    while ( status ) {
      TARGET* tgt   = currentTarget( curReference );
      double weight = curReference.weight();
      range.addEntry( reqSrc, tgt, weight );
      status = m_links->nextReference( curReference );
    }
    return range;
  };
 
  /** Returns the relations with selected weight in the table for a given object.
   *  @param  reqSrc  Object for which the relations is wanted
   *  @param  cut     Value to which the weight is compared
   *  @param  order   Type of comparison true : Weight should be higher than cut, false the reverse
   *  @return Range of selected relations
   */
  Range relations( const SOURCE* reqSrc, double cut, bool order ) const {
    Range range;
    if ( NULL == m_links ) return range;
    if ( NULL == reqSrc ) return range;
    LHCb::LinkReference curReference;
    bool status = m_links->firstReference( reqSrc->index(), 
                                           reqSrc->parent(),
                                           curReference   );
    while ( status ) {
      double weight = curReference.weight();
      if ( ( order && weight >= cut ) ||
           (!order && weight<= cut ) ) {
        TARGET* tgt   = currentTarget( curReference );
        range.addEntry( reqSrc, tgt, weight );
      }
      status = m_links->nextReference( curReference );
    }
    return range;
  };

  /** load the LinksByKeys object
   *  @param links  pointer to the LinksByKey object
   */
  void load( const LHCb::LinksByKey* links ) {
    m_links = links;
  }

protected:
  /** return the pointer to the TARGET object of the specified reference
   *  @param  curReference   reference entry to the object in the LinksByKeys
   *  @return pointer to the TARGET object
   */
  TARGET* currentTarget( LHCb::LinkReference& curReference ) const {
    if ( NULL == m_links ) return NULL;
    int myLinkID = curReference.linkID();
    LinkManager::Link* link = m_links->linkMgr()->link( myLinkID );
    ObjectContainerBase* parent = dynamic_cast<ObjectContainerBase*>(link->object() );
    if ( 0 != parent ) {
      TARGET* myObj = (TARGET*)parent->containedObject( curReference.objectKey() );
      return myObj;
    }
    return NULL;
  }  

  /** return the pointer to the SOURCE object of the specified reference
   *  @param  key            key of the object. Its link is in the curReference argument
   *  @param  curReference   reference entry to the object in the LinksByKeys
   *  @return pointer to the SOURCE object
   */
 SOURCE* currentSource( int key, LHCb::LinkReference curReference ) const {
    if ( NULL == m_links ) return NULL;
    int myLinkID = curReference.srcLinkID();
    if ( 0 > myLinkID ) return NULL;
    LinkManager::Link* link = m_links->linkMgr()->link( myLinkID );
    ObjectContainerBase* parent = dynamic_cast<ObjectContainerBase*>(link->object() );
    if ( 0 != parent ) {
      return (SOURCE*)parent->containedObject( key );
    }
    return NULL;
  }

private:
  const LHCb::LinksByKey*  m_links;
};
#endif // LINKER_LINKERTABLE_H
