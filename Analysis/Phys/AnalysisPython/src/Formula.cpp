// $Id$
// ============================================================================
// Include files 
// ============================================================================
// AnalysisPython
// ============================================================================
#include "Analysis/Formula.h"
// ============================================================================
// ROOT 
// ============================================================================
#include "TTree.h"
// ============================================================================
/** Implementation file for class Analysis::Formula
 *  @see Analysis::Formula
 *  @see TTreeFormula
 *  @date 2013-05-06 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
// constructor from name, expression and the tree 
// ============================================================================
Analysis::Formula::Formula
( const std::string& name       , 
  const std::string& expression ,
  TTree*             tree       ) 
  : TTreeFormula ( name.c_str() , expression.c_str() , tree )
{}
// ============================================================================
// destructor 
// ============================================================================
Analysis::Formula::~Formula()
{
  TTree* tree = GetTree() ;
  if ( 0 != tree && this == tree->GetNotify() ) { tree -> SetNotify ( 0 ) ; }
}
// ============================================================================
// evaluate the formula 
// ============================================================================
double Analysis::Formula::evaluate () // evaluate the formula 
{ return EvalInstance () ; }
// ============================================================================
// The END 
// ============================================================================
