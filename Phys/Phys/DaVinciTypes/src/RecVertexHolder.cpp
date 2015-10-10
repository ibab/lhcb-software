// $Id$
// ============================================================================
// Inclide file 
// ============================================================================
// RecEvent
// ============================================================================
#include "Event/RecVertex.h"
// ============================================================================
// local
// ============================================================================
#include "Kernel/RecVertexHolder.h"
// ============================================================================
/** @file
 *  Implementation of class LHCb::RecVertexHolder
 *  @see LHcb::RecVertexHolder
 *  @author Vanya BELYAEV Ivan.BElyaev@cern.ch
 *  @date 2010-12-02
 */
// ============================================================================
// constructor from the vertex 
// ============================================================================
LHCb::RecVertexHolder::RecVertexHolder 
( const LHCb::RecVertex* vertex )
  : m_vertex ( vertex ) 
{}
// ============================================================================
/*  copy constructor
 *  @attention "desctructive copy semantics!"
 *  @param right object to be copied (invalid after copy!!!)
 */
// ============================================================================
LHCb::RecVertexHolder::RecVertexHolder
( const LHCb::RecVertexHolder& right ) 
  : m_vertex ( right.m_vertex ) 
{}
// =============================================================================
/*  destructor 
 *  @attention object, not registered in TES will be  deleted!
 */
// =============================================================================
LHCb::RecVertexHolder::~RecVertexHolder() 
{
  // do not delete the objects registred in TES 
  if ( 0 != m_vertex.get() && 0 != m_vertex->parent() ) 
  { m_vertex.release () ; }
}
// ============================================================================
// The END 
// ============================================================================
