// $Id:$ 
// ============================================================================
// Include files
// ============================================================================
// Local: 
// ============================================================================
#include "Analysis/StatVar.h"
#include "Analysis/Formula.h"
#include "Analysis/Iterator.h"
// ============================================================================
// ROOT 
// ============================================================================
#include "RooDataSet.h"
#include "RooFormulaVar.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
// ============================================================================
// local
// ============================================================================
#include "Analysis/HProject.h"
// ============================================================================
/** @file
 *  Implementation file for class Analysis::HProject
 *  @see Analysis::HProject
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2015-10-08
 * 
 *                    $Revision: 195425 $
 *  Last modification $Date: 2015-10-01 15:33:56 +0200 (Thu, 01 Oct 2015) $
 *                 by $Author: ibelyaev $
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  static_assert (std::numeric_limits<unsigned long>::is_specialized   , 
                 "Numeric_limist<unsigned long> are not specialized!" ) ;
  // ========================================================================== 
  /// get variable by name from RooArgSet
  RooAbsReal* get_var ( const RooArgSet&   aset , 
                        const std::string& name ) 
  {
    RooAbsArg* arg = aset.find ( name.c_str() ) ;
    return 0 != arg ? dynamic_cast<RooAbsReal*> ( arg ) : nullptr ;
  }
  // ==========================================================================
}
// ============================================================================
/** make a projection of RooDataSet into the histogram 
 *  @param data  (INPUT)  input data 
 *  @param histo (UPDATE) histogram 
 *  @param expression (INPUT) expression
 *  @param selection  (INPUT) selection criteria/weight 
 */
// ============================================================================
StatusCode Analysis::HProject::project
( const RooAbsData*   data       , 
  TH1*                histo      ,
  const RooAbsReal&   expression ,
  const RooAbsReal*   selection  ,
  const unsigned long first      ,
  const unsigned long last       ) 
{
  //
  if ( 0 == histo ) { return StatusCode ( 301 ) ; }
  else { histo->Reset() ; } // reset the historgam 
  if ( 0 == data  ) { return StatusCode ( 300 ) ; }
  //
  const unsigned long nEntries = 
    std::min ( last , (unsigned long) data->numEntries() ) ;
  if ( nEntries <= first  ) { return StatusCode::RECOVERABLE ; }
  //
  const bool weighted = data->isWeighted() ;
  //
  for ( unsigned long entry = first ; entry < nEntries ; ++entry )   
  {
    //
    if ( 0 == data->get( entry)  ) { break ; }                    // BREAK
    //
    // calculate the weight 
    const double w = 
      selection && weighted ? selection->getVal() * data->weight() : 
      selection             ? selection->getVal()                  :
      weighted              ?                       data->weight() : 1.0 ;
    //
    // skip null weights 
    if ( !w ) { continue ; }
    //
    // calculate the values (only for non-zero weights)
    const double xvalue = expression.getVal()  ;
    // fill the histogram  (only for non-zero weights)
    histo->Fill ( xvalue , w ) ; 
    //
  }
  //
  return StatusCode::SUCCESS ;  
}
// ============================================================================
/*  make a projection of RooDataSet into the histogram 
 *  @param data  (INPUT)  input data 
 *  @param histo (UPDATE) histogram 
 *  @param xexpression (INPUT) expression for x-axis 
 *  @param yexpression (INPUT) expression for y-axis 
 *  @param selection  (INPUT) selection criteria/weight 
 */
// ============================================================================
StatusCode Analysis::HProject::project2
( const RooAbsData*   data        , 
  TH2*                histo       ,
  const RooAbsReal&   xexpression ,
  const RooAbsReal&   yexpression ,
  const RooAbsReal*   selection   ,
  const unsigned long first       ,
  const unsigned long last        ) 
{
  //
  if ( 0 == histo ) { return StatusCode ( 301 ) ; }
  else { histo->Reset() ; } // reset the historgam 
  if ( 0 == data  ) { return StatusCode ( 300 ) ; }
  //
  const unsigned long nEntries = 
    std::min ( last , (unsigned long) data->numEntries() ) ;
  if ( nEntries <= first  ) { return StatusCode::RECOVERABLE ; }
  //
  const bool weighted = data->isWeighted() ;
  //
  for ( unsigned long entry = first ; entry < nEntries ; ++entry )   
  {
    //
    if ( 0 == data->get( entry)  ) { break ; }                    // BREAK
    //
    // calculate the weight 
    const double w = 
      selection && weighted ? selection->getVal() * data->weight() : 
      selection             ? selection->getVal()                  :
      weighted              ?                       data->weight() : 1.0 ;
    //
    // skip null weights 
    if ( !w ) { continue ; }
    //
    // calculate the values (only for non-zero weights)
    const double xvalue = xexpression.getVal()  ;
    const double yvalue = yexpression.getVal()  ;
    // fill the histogram  (only for non-zero weights)
    histo->Fill ( xvalue , yvalue , w ) ; 
    //
  }
  //
  return StatusCode::SUCCESS ;  
}
// ============================================================================
// ============================================================================
/*  make a projection of RooDataSet into the histogram 
 *  @param data  (INPUT)  input data 
 *  @param histo (UPDATE) histogram 
 *  @param xexpression (INPUT) expression for x-axis 
 *  @param yexpression (INPUT) expression for y-axis 
 *  @param zexpression (INPUT) expression for z-axis 
 *  @param selection  (INPUT) selection criteria/weight 
 */
// ============================================================================
StatusCode Analysis::HProject::project3
( const RooAbsData*   data        , 
  TH3*                histo       ,
  const RooAbsReal&   xexpression ,
  const RooAbsReal&   yexpression ,
  const RooAbsReal&   zexpression ,
  const RooAbsReal*   selection   ,
  const unsigned long first       ,
  const unsigned long last        ) 
{
  //
  if ( 0 == histo ) { return StatusCode ( 301 ) ; }
  else { histo->Reset() ; } // reset the historgam 
  if ( 0 == data  ) { return StatusCode ( 300 ) ; }
  //
  const unsigned long nEntries = 
    std::min ( last , (unsigned long) data->numEntries() ) ;
  if ( nEntries <= first  ) { return StatusCode::RECOVERABLE ; }
  //
  const bool weighted = data->isWeighted() ;
  //
  for ( unsigned long entry = first ; entry < nEntries ; ++entry )   
  {
    //
    if ( 0 == data->get( entry)  ) { break ; }                    // BREAK
    //
    // calculate the weight 
    const double w = 
      selection && weighted ? selection->getVal() * data->weight() : 
      selection             ? selection->getVal()                  :
      weighted              ?                       data->weight() : 1.0 ;
    //
    // skip null weights 
    if ( !w ) { continue ; }
    //
    // calculate the values (only for non-zero weights)
    const double xvalue = xexpression.getVal()  ;
    const double yvalue = yexpression.getVal()  ;
    const double zvalue = zexpression.getVal()  ;
    // fill the histogram  (only for non-zero weights)
    histo->Fill ( xvalue , yvalue , zvalue , w ) ; 
    //
  }
  //
  return StatusCode::SUCCESS ;  
}
// ============================================================================
/* make a projection of RooDataSet into the histogram 
 * @param data  (INPUT)  input data 
 * @param histo (UPDATE) histogram 
 * @param expression (INPUT) expression
 * @param selection  (INPUT) selection criteria/weight 
 */
// ============================================================================
StatusCode Analysis::HProject::project
( const RooAbsData*   data       , 
  TH1*                histo      ,
  const std::string&  expression ,
  const std::string&  selection  ,
  const unsigned long first      ,                                          
  const unsigned long last       ) 
{
  //
  if ( 0 == histo ) { return StatusCode ( 301 ) ; }
  else { histo->Reset() ; } // reset the historgam 
  if ( 0 == data  ) { return StatusCode ( 300 ) ; }
  //
  const unsigned long nEntries = 
    std::min ( last , (unsigned long) data->numEntries() ) ;
  if ( nEntries <= first  ) { return StatusCode::RECOVERABLE ; }
  //
  RooArgList        alst ;
  const RooArgSet*  aset = data->get() ;
  if ( 0 == aset       ) { return  0 ; }                          // RETURN
  Analysis::Iterator iter ( *aset );
  RooAbsArg*   coef = 0 ;
  while ( ( coef = (RooAbsArg*) iter.next() ) ){ alst.add ( *coef ); }
  //
  // convert expressions into RooFormulaVar 
  const bool        with_cuts   = !selection.empty() ;
  const RooAbsReal* cut_var     = 0 ;
  if ( with_cuts ) { cut_var =get_var ( *aset , selection ) ; }
  std::unique_ptr<RooFormulaVar> cuts ;
  if ( with_cuts && 0 == cut_var ) 
  {
    cuts.reset ( new RooFormulaVar( "" , selection.c_str() , alst ) ) ;
    if ( !cuts->ok () ) { return StatusCode(302) ; } //        // RETURN 
  }
  //
  const RooAbsReal* x_var = get_var ( *aset , expression ) ;
  std::unique_ptr<RooFormulaVar> xwhat ;
  if ( 0 == x_var ) 
  {
    xwhat.reset( new RooFormulaVar( "" ,  expression.c_str() , alst ) ) ;
    if ( !xwhat->ok()   ) { return StatusCode(303)  ; }             // RETURN
  }
  //
  return project ( data                                   , 
                   histo                                  , 
                   0 !=   x_var ?   *x_var : *xwhat       , 
                   0 != cut_var ?  cut_var :   cuts.get() , first , last ) ;
}
// ============================================================================
/*  make a projection of RooDataSet into 2D-histogram 
 *  @param data  (INPUT)  input data 
 *  @param histo (UPDATE) histogram 
 *  @param xexpression (INPUT) expression for x-axis 
 *  @param yexpression (INPUT) expression for y-axis 
 *  @param selection  (INPUT) selection criteria/weight 
 */
// ============================================================================
StatusCode Analysis::HProject::project2
( const RooAbsData*   data        , 
  TH2*                histo       ,
  const std::string&  xexpression ,
  const std::string&  yexpression ,
  const std::string&  selection   ,
  const unsigned long first       ,
  const unsigned long last        ) 
{
  //
  if ( 0 == histo ) { return StatusCode ( 301 ) ; }
  else { histo->Reset() ; } // reset the historgam 
  if ( 0 == data  ) { return StatusCode ( 300 ) ; }
  //
  const unsigned long nEntries = 
    std::min ( last , (unsigned long) data->numEntries() ) ;
  if ( nEntries <= first  ) { return StatusCode::SUCCESS ; }
  //
  RooArgList        alst ;
  const RooArgSet*  aset = data->get() ;
  if ( 0 == aset       ) { return  0 ; }                          // RETURN
  Analysis::Iterator iter ( *aset );
  RooAbsArg*   coef = 0 ;
  while ( ( coef = (RooAbsArg*) iter.next() ) ){ alst.add ( *coef ); }
  //
  // convert expressions into RooFormulaVar 
  const bool        with_cuts   = !selection.empty() ;
  const RooAbsReal* cut_var     = 0 ;
  if ( with_cuts ) { cut_var =get_var ( *aset , selection ) ; }
  std::unique_ptr<RooFormulaVar> cuts ;
  if ( with_cuts && 0 == cut_var ) 
  {
    cuts.reset ( new RooFormulaVar( "" , selection.c_str() , alst ) ) ;
    if ( !cuts->ok () ) { return StatusCode(302) ; } //        // RETURN 
  }
  //
  const RooAbsReal* x_var = get_var ( *aset , xexpression ) ;
  std::unique_ptr<RooFormulaVar> xwhat ;
  if ( 0 == x_var ) 
  {
    xwhat.reset( new RooFormulaVar( "" ,  xexpression.c_str() , alst ) ) ;
    if ( !xwhat->ok()   ) { return StatusCode(303)  ; }             // RETURN
  }
  //
  const RooAbsReal* y_var = get_var ( *aset , yexpression ) ;
  std::unique_ptr<RooFormulaVar> ywhat ;
  if ( 0 == y_var ) 
  {
    ywhat.reset( new RooFormulaVar( "" ,  yexpression.c_str() , alst ) ) ;
    if ( !ywhat->ok()   ) { return StatusCode(304)  ; }             // RETURN
  }
  //
  return project2 ( data                                   , 
                    histo                                  , 
                    0 !=   x_var ?   *x_var : *xwhat       , 
                    0 !=   y_var ?   *y_var : *ywhat       , 
                    0 != cut_var ?  cut_var :   cuts.get() , first , last ) ;
}
// ============================================================================
/*  make a projection of RooDataSet into the histogram 
 *  @param data  (INPUT)  input data 
 *  @param histo (UPDATE) histogram 
 *  @param xexpression (INPUT) expression for x-axis 
 *  @param yexpression (INPUT) expression for y-axis 
 *  @param zexpression (INPUT) expression for z-axis 
 *  @param selection  (INPUT) selection criteria/weight 
 */
// ============================================================================
StatusCode Analysis::HProject::project3
( const RooAbsData*   data        , 
  TH3*                histo       ,
  const std::string&  xexpression ,
  const std::string&  yexpression ,
  const std::string&  zexpression ,
  const std::string&  selection   ,
  const unsigned long first       ,
  const unsigned long last        ) 
{
  //
  if ( 0 == histo ) { return StatusCode ( 301 ) ; }
  else { histo->Reset() ; } // reset the historgam 
  if ( 0 == data  ) { return StatusCode ( 300 ) ; }
  //
  const unsigned long nEntries = 
    std::min ( last , (unsigned long) data->numEntries() ) ;
  if ( nEntries <= first  ) { return StatusCode::SUCCESS ; }
  //
  RooArgList        alst ;
  const RooArgSet*  aset = data->get() ;
  if ( 0 == aset       ) { return  0 ; }                          // RETURN
  Analysis::Iterator iter ( *aset );
  RooAbsArg*   coef = 0 ;
  while ( ( coef = (RooAbsArg*) iter.next() ) ){ alst.add ( *coef ); }
  //
  // convert expressions into RooFormulaVar 
  const bool        with_cuts   = !selection.empty() ;
  const RooAbsReal* cut_var     = 0 ;
  if ( with_cuts ) { cut_var =get_var ( *aset , selection ) ; }
  std::unique_ptr<RooFormulaVar> cuts ;
  if ( with_cuts && 0 == cut_var ) 
  {
    cuts.reset ( new RooFormulaVar( "" , selection.c_str() , alst ) ) ;
    if ( !cuts->ok () ) { return StatusCode(302) ; } //        // RETURN 
  }
  //
  const RooAbsReal* x_var = get_var ( *aset , xexpression ) ;
  std::unique_ptr<RooFormulaVar> xwhat ;
  if ( 0 == x_var ) 
  {
    xwhat.reset( new RooFormulaVar( "" ,  xexpression.c_str() , alst ) ) ;
    if ( !xwhat->ok()   ) { return StatusCode(303)  ; }             // RETURN
  }
  //
  const RooAbsReal* y_var = get_var ( *aset , yexpression ) ;
  std::unique_ptr<RooFormulaVar> ywhat ;
  if ( 0 == y_var ) 
  {
    ywhat.reset( new RooFormulaVar( "" ,  yexpression.c_str() , alst ) ) ;
    if ( !ywhat->ok()   ) { return StatusCode(304)  ; }             // RETURN
  }
  //
  const RooAbsReal* z_var = get_var ( *aset , zexpression ) ;
  std::unique_ptr<RooFormulaVar> zwhat ;
  if ( 0 == z_var ) 
  {
    zwhat.reset( new RooFormulaVar( "" ,  zexpression.c_str() , alst ) ) ;
    if ( !zwhat->ok()   ) { return StatusCode(305)  ; }             // RETURN
  }
  //
  return project3 ( data                                   , 
                    histo                                  , 
                    0 !=   x_var ?   *x_var : *xwhat       , 
                    0 !=   y_var ?   *y_var : *ywhat       , 
                    0 !=   z_var ?   *z_var : *zwhat       , 
                    0 != cut_var ?  cut_var :   cuts.get() , first , last ) ;
}
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
