// $Id$
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
// constructor 
// ============================================================================
Analysis::Selector::Selector
( TTree*    tree , 
  PyObject* self ) 
  : TPySelector ( tree, self )
{}
// ============================================================================
// destructor
// ============================================================================
Analysis::Selector::~Selector(){}
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
/*  helper function to use TTree::Process in python 
 * 
 *  @param tree      root-tree 
 *  @param selector  the selector 
 *  @param events    events to be processed 
 *  
 *  @see TTree 
 *  @see TTree::Process 
 *  @see TSelector 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
 *  @date   2013-02-10
 */
// ============================================================================
long Analysis::Process::process
( TTree*              tree      ,
  TSelector*          selector  , 
  const unsigned long events    ) 
{
  if ( 0 == tree || 0 == selector ) { return 0 ; }
  return tree->Process ( selector , "" , events ) ;
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
/* helper function to use TChain::Process in python 
 * 
 *  @param chain     root-chain
 *  @param selector  the selector 
 *  @param events    events to be processed 
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
( TChain*             chain    ,
  TSelector*          selector ,
  const unsigned long events   ) 
{
  if ( 0 == chain || 0 == selector ) { return 0 ; }
  return chain -> Process ( selector , "" , events ) ;
}
// ============================================================================
// The END 
// ============================================================================
