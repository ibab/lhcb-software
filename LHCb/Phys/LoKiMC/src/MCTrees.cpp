// $Id$
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2006/02/18 18:10:57  ibelyaev
//  fix a typo
//
// Revision 1.1.1.1  2006/01/26 16:13:39  ibelyaev
// New Packaage: MC-dependent part of LoKi project 
//
// Revision 1.1  2004/07/07 10:59:15  ibelyaev
//  regular update
// 
// ============================================================================
// Include files
// ============================================================================
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
// ============================================================================
// local
// ============================================================================
#include "LoKi/MCTrees.h"
// ============================================================================

// ============================================================================
/** @file
 *
 *  Implementation file for functions from namespace LoKi::MCTrees
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================

// ============================================================================
/** simple function with evaluates to true 
 *  if MC particle  originates in the decay 
 *  tree of parent veretx  
 *  @param particle pointer to MC particle 
 *  @param parent pointer to the mother veretx  
 *  @return true if particle belongd to decay tree of parent 
 */
// ============================================================================
bool LoKi::MCTrees::fromMCTree 
( const LHCb::MCParticle* particle  , 
  const LHCb::MCVertex*   parent    ) 
{
  if ( 0 == particle || 0 == parent ) { return false ; }
  typedef SmartRefVector<LHCb::MCParticle> Products ;
  const Products& products = parent->products() ;
  for ( Products::const_iterator it = products.begin() ; 
        products.end() != it ; ++it ) 
  {
    const LHCb::MCParticle* mcp = *it ;
    if ( 0 == mcp ) { continue ; }
    if ( fromMCTree ( particle , mcp ) ) { return true ; }   // RETURN
  }
  return false ;
}
// ============================================================================

// ============================================================================
/** simple function with evaluates to true 
 *  if MC particle  originates in the decay 
 *  tree of parent particle 
 *  @param particle pointer to MC particle 
 *  @param parent pointer to the mother particle 
 *  @return true if particle belongd to decay tree of parent 
 */
// ============================================================================
bool LoKi::MCTrees::fromMCTree 
( const LHCb::MCParticle* particle  , 
  const LHCb::MCParticle* parent    ) 
{
  if ( 0 == particle || 0 == parent ) { return false ; }   // RETURN
  
  if (      particle ==      parent ) { return true  ; }   // RETURN 
  
  typedef SmartRefVector<LHCb::MCVertex> EndVertices ;
  const EndVertices& endVertices = parent->endVertices() ;
  for ( EndVertices::const_iterator it = endVertices.begin() ; 
        endVertices.end() != it ; ++it ) 
  {
    const LHCb::MCVertex* mcv = *it ;
    if ( 0 == mcv ) { continue ; }
    if ( fromMCTree( particle , mcv ) ) { return true ; }  // RETURN
  }
  return false ;
}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
