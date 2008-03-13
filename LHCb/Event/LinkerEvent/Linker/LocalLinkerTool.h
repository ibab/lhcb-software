// $Id: LocalLinkerTool.h,v 1.1 2008-03-13 17:00:27 ocallot Exp $
#ifndef LINKER_LOCALLINKERTOOL_H 
#define LINKER_LOCALLINKERTOOL_H 1

// Include files
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/ObjectContainerBase.h"
#include "GaudiKernel/GaudiException.h"

#include "Event/LinksByKey.h"

#include "Linker/LinkerWithKey.h"
#include "Linker/LinkerTable.h"

/** @class LocalLinkerTool LocalLinkerTool.h Linker/LocalLinkerTool.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2008-03-11
 */
template <class SOURCE, class TARGET> class LocalLinkerTool {

public: 
    typedef LinkerTable<SOURCE,TARGET>    DirectType;
    typedef LinkerTable<TARGET,SOURCE>    InverseType;

  /** Standard constructor
   *  @param  svc             event service
   *  @param  containerName   name of the container of links.
   */
  LocalLinkerTool( ) {
    newEvent();
  };

  ~LocalLinkerTool( ) {}; ///< Destructor

  void newEvent() {
    m_links.reset();
    m_invLink.reset();
  }

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

  /** retrieve the direct relation
   *  @return     The direct table of relation.
   */

  DirectType* direct ( ) {
    m_table.load( &m_links );
    return &m_table;
  }

  /** retrieve the inverse relation, build it if not yet done
   *  @return     The inverse table of relation.
   */

  InverseType* inverse ( ) {
    //== Invert the table...
    typedef typename DirectType::Range    Range;
    typedef typename DirectType::iterator iterator;
    m_invLink.reset();
    const DirectType* tmp = direct();
    Range rd = tmp->relations();
    for ( iterator it = rd.begin(); rd.end() != it; ++it ) {
      int srcIndex   = it->to()->index ();
      int srcLinkID  = m_invLink.linkID( it->to()->parent() );
      int destIndex  = it->from()->index();
      int destLinkID = m_invLink.linkID( it->from()->parent() );
      double weight = it->weight();
      m_invLink.addReference( srcIndex, srcLinkID, destIndex, destLinkID, weight );
    }
    m_invTable.load( &m_invLink );
    return &m_invTable;
  }

protected:

private:
  LHCb::LinksByKey m_links;
  LHCb::LinksByKey m_invLink;
  DirectType       m_table;
  InverseType      m_invTable;
};
#endif // LINKER_LOCALLINKERTOOL_H
