// $Id$ 
// ============================================================================
// Include files 
// ============================================================================
// AnalysisPython
// ============================================================================
#include "Analysis/PySelectorWithCuts.h"
#include "Analysis/Formula.h"
// ============================================================================
/** @file 
 *  Implementation file for class Analysis::SelectorWithCuts
 * 
 *  @date 2013-05-06 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *
 *                    $Revision$
 *  Last modification $Date$
 *  by                $Author$
 */
// ============================================================================
// constructor 
// ============================================================================
Analysis::SelectorWithCuts::SelectorWithCuts
( const std::string& cuts , 
  TTree*             tree , 
  PyObject*          self ) 
  : Analysis::Selector ( tree , self ) 
  , fMycuts             ( cuts        ) 
  , fMyformula          ( 0           ) 
{
  if ( 0 != tree ) 
  { fMyformula = new Analysis::Formula ( "" , fMycuts , tree ) ; }
}
// ============================================================================
// virtual destructor 
// ============================================================================
Analysis::SelectorWithCuts::~SelectorWithCuts ()
{ if ( 0 != fMyformula ) { delete fMyformula ; fMyformula = 0 ; } }
// ============================================================================
// notify 
// ============================================================================
Bool_t Analysis::SelectorWithCuts::Notify() 
{
  if ( 0 != fMyformula ) { fMyformula->Notify() ; }
  return TPySelector::Notify () ;
}
// ============================================================================
// init 
// ============================================================================
void Analysis::SelectorWithCuts::Init ( TTree* tree ) 
{
  //
  if ( 0 != fMyformula ) { delete fMyformula ; fMyformula = 0 ; }
  fMyformula = new Analysis::Formula ( "" , fMycuts , tree ) ;
  //
  TPySelector::Init ( tree ) ;
}
// ============================================================================
//  begin 
// ============================================================================
void Analysis::SelectorWithCuts::Begin ( TTree* tree ) 
{
  TPySelector::Begin ( tree ) ;
}
// ============================================================================
// slave begin 
// ============================================================================
void Analysis::SelectorWithCuts::SlaveBegin ( TTree* tree ) 
{
  //
  if ( 0 != fMyformula ) { delete fMyformula ; fMyformula = 0 ; }
  fMyformula = new Analysis::Formula ( "" , fMycuts , tree ) ;
  //
  TPySelector::SlaveBegin ( tree ) ;
}
// ============================================================================
// process 
// ============================================================================
Bool_t Analysis::SelectorWithCuts::Process      ( Long64_t entry ) 
{
  //
  if ( 0 != fMyformula && fMyformula->GetNdim() && !fMyformula ->evaluate() ) 
  { return false ; }
  //
  return TPySelector::Process ( entry ) ;
}
// ============================================================================
// is formula OK?
// ============================================================================
bool Analysis::SelectorWithCuts::ok () const // is formula OK ? 
{ return 0 != fMyformula && fMyformula->ok () ; }


// ============================================================================
// The END 
// ============================================================================
