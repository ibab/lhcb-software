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
( TTree*             tree , 
  const std::string& cuts , 
  PyObject*          self ) 
  : Analysis::Selector ( tree , self ) 
  , m_cuts             ( cuts        ) 
  , m_formula          ( 0           ) 
{
  if ( 0 != tree ) 
  { m_formula = new Analysis::Formula ( "" , m_cuts , tree ) ; }
}
// ============================================================================
// virtual destructor 
// ============================================================================
Analysis::SelectorWithCuts::~SelectorWithCuts ()
{ if ( 0 != m_formula ) { delete m_formula ; m_formula = 0 ; } }
// ============================================================================
// notify 
// ============================================================================
Bool_t Analysis::SelectorWithCuts::Notify() 
{
  if ( 0 != m_formula ) { m_formula->Notify() ; }
  return TPySelector::Notify () ;
}
// ============================================================================
// init 
// ============================================================================
void Analysis::SelectorWithCuts::Init ( TTree* tree ) 
{
  //
  if ( 0 != m_formula ) { delete m_formula ; m_formula = 0 ; }
  m_formula = new Analysis::Formula ( "" , m_cuts , tree ) ;
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
  if ( 0 != m_formula ) { delete m_formula ; m_formula = 0 ; }
  m_formula = new Analysis::Formula ( "" , m_cuts , tree ) ;
  //
  TPySelector::SlaveBegin ( tree ) ;
}
// ============================================================================
// process 
// ============================================================================
Bool_t Analysis::SelectorWithCuts::Process      ( Long64_t entry ) 
{
  //
  if ( 0 != m_formula && m_formula->GetNdim() && !m_formula ->evaluate() ) 
  { return false ; }
  //
  return TPySelector::Process ( entry ) ;
}
// ============================================================================
// is formula OK?
// ============================================================================
bool Analysis::SelectorWithCuts::ok () const // is formula OK ? 
{ return 0 != m_formula && m_formula->ok () ; }


// ============================================================================
// The END 
// ============================================================================
