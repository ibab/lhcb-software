// $Id: $
// ============================================================================
// Include files 
// ============================================================================
// ROOT 
// ============================================================================
#include "TTree.h"
#include "TChain.h"
// ============================================================================
// local
// ============================================================================
#include "Analysis/PySelector.h"
// ============================================================================
/** @file 
 * 
 *  Implementation file for class Analysis::PySelector
 *
 *  @see Analysis::PySelector
 *  @see TPySelector 
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
 *  @date   2011-01-21
 * 
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
/*  helper function to use TTree::Process in python 
 * 
 *  @param tree      root-tree 
 *  @param selector  the selector 
 *  
 *  @see TTree 
 *  @see TTree::Process 
 *  @see TSelector 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
 *  @date   2011-01-21
 */
// ============================================================================
long Analysis::Process::process
( TTree*             tree     ,
  TSelector*         selector )
{
  if ( 0 == tree || 0 == selector ) { return 0 ; }
  return tree->Process ( selector ) ;
}
// ============================================================================
/* helper function to use TChain::Process in python 
 * 
 *  @param chain     root-chain
 *  @param selector  the selector 
 *  
 *  @see TTree 
 *  @see TTree::Process 
 *  @see TSelector 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
 *  @date   2011-01-21
 */
// ============================================================================
long Analysis::Process::process
( TChain*            chain    ,
  TSelector*         selector )
{
  if ( 0 == chain || 0 == selector ) { return 0 ; }
  return chain -> Process ( selector ) ;
}
// ============================================================================
// The END 
// ============================================================================
