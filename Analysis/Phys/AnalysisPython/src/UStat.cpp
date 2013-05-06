// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD& STL 
// ============================================================================
#include <cmath>
#include <vector>
#include <algorithm>
#include <numeric>
// ============================================================================
// ROOT & RooFit 
// ============================================================================
#include "TIterator.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TMath.h"
#include "TH1.h"
// ============================================================================
#include "RooAbsPdf.h"
#include "RooArgSet.h"
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooGlobalFunc.h"
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/Power.h"
// ============================================================================
// local
// ============================================================================
#include "Analysis/UStat.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/progress.hpp"
// ============================================================================
/** @file
 *  Implementation file for class Analysis::UStat
 *  @see Analysis::Ustat
 *  @date 2011-09-27 
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  double getDistance ( const RooArgSet* x , 
                       const RooArgSet* y )
  {
    if ( 0 == x || 0 == y ) { return -1 ; } // RETURN 
    //
    double result = 0.;
    //
    TIterator* xIter = x->createIterator();
    TIterator* yIter = y->createIterator();
    //
    RooRealVar* xVar = 0 ;
    RooRealVar* yVar = 0 ;
    while ( ( xVar = (RooRealVar*)xIter -> Next() ) && 
            ( yVar = (RooRealVar*)yIter -> Next() )    ) 
    {
      const double val  = xVar->getVal() - yVar->getVal() ;
      result           += val*val ;
    }
    //
    delete xIter ;
    delete yIter ;
    //
    return std::sqrt ( result ) ;
  }
  // ==========================================================================
  /// get the volume of n-ball with unit radius 
  double nBallVolume ( const unsigned int n )
  {
    return 
      0 == n ?  0.0 :  // 0-ball : nothing 
      1 == n ?  2.0 :  // 1-ball : interval 
      2 == n ? M_PI :  // 2-ball : circle 
      2 * M_PI / double ( n ) * nBallVolume ( n - 2 ) ;
  }
  // ==========================================================================
} //                                                 end of anonymous namespace  
// ============================================================================
/*  calculate U-statistics 
 *  @param pdf   (input) PDF
 *  @param data  (input) data 
 *  @param hist  (update) the histogram with U-statistics 
 *  @param args  (input)  the arguments
 *  @param tStat (output,optional) value for T-statistics 
 */
// ============================================================================
StatusCode Analysis::UStat::calculate
( const RooAbsPdf&  pdf   , 
  const RooDataSet& data  ,  
  TH1&              hist  ,
  double&           tStat ,
  RooArgSet*        args  ) 
{
  //
  if ( 0 == args ) { args = pdf.getObservables ( data ) ; }
  //
  if ( 0 == args )             { return StatusCode( InvalidArgs ) ; }
  //
  const unsigned int dim    = args->getSize   () ;
  if ( 1 > dim  ) { return StatusCode( InvalidDims ) ; }
  const double volume = nBallVolume ( dim ) ;
  //
  typedef std::vector<double> TStat ;
  TStat tstat ;
  //
  const RooDataSet*  cloned = (RooDataSet*)data.Clone() ;
  //
  const unsigned int num    = data.numEntries () ;
  boost::progress_display show_progress ( num ) ;
  //
  const RooArgSet * event_x = 0 ;
  const RooArgSet * event_y = 0 ;
  //
  for ( unsigned int i = 0 ; i < num ; ++i ) 
  {
    // display progress 
    ++show_progress ;
    //
    // 1. Get "Event"
    event_x = data . get(i) ;      
    if ( 0 == event_x || 0 == event_x->getSize() ) 
    { return StatusCode ( InvalidItem1 ) ; }             // RETURN 
    const RooArgSet *event_i = (RooArgSet*)event_x->selectCommon( *args );
    if ( 0 == event_i || 0 == event_i->getSize() ) 
    { return StatusCode ( InvalidItem2 ) ; }             // RETURN 
    //
    // 2.Evaluate PDF 
    TIterator  * iter = args->createIterator();
    RooRealVar * var = 0 ;
    while ( (var = (RooRealVar*)iter->Next() ) ) 
    { var->setVal( event_i->getRealValue( var->GetName() ) ); }
    delete iter;
    //
    const double pdfValue = pdf . getVal( args ) ;
    //
    double min_distance  = 1.e+100 ;
    for ( unsigned int j = 0 ; j < num ; ++j ) 
    {
      if ( i == j ) { continue ; }
      //
      event_y = cloned->get(j) ;      
      if ( 0 == event_y || 0 == event_y->getSize() ) 
      { return StatusCode ( InvalidItem1 ) ; }            // RETURN 
      const RooArgSet *event_j = (RooArgSet*)event_y->selectCommon( *args );
      if ( 0 == event_j || 0 == event_j->getSize() )  
      { return StatusCode ( InvalidItem2 ) ; }            // RETURN 
      //
      const double distance = getDistance ( event_i , event_j ) ;
      if ( 0 > distance ) { return StatusCode( InvalidDist ) ; }  // RETURN 
      //
      if ( 0 == j || distance < min_distance ) 
      { min_distance = distance  ; }
      //
      delete event_j ;
    }
    //
    delete event_i ; 
    //
    // volume of n-ball: 
    const double val1 = volume * Gaudi::Math::pow ( min_distance , dim ) ;
    //
    const double value = std::exp ( -val1 * num * pdfValue ) ;
    //
    hist.Fill ( value ) ;
    //
    tstat.push_back ( value ) ; 
    //
  } 
  delete cloned ;
  //
  // calculate T-statistics
  //
  std::sort ( tstat.begin() , tstat.end() ) ;
  double tS = 0 ;
  double nD = tstat.size() ;
  for ( TStat::const_iterator t = tstat.begin() ; tstat.end() != t  ; ++t ) 
  {
    const double e = ( double ( t - tstat.begin() + 1 ) )  / nD ;
    const double d = (*t) - e ;
    //
    tS += d * d ;
  }
  // finally return the value:
  tStat = tS ;
  //
  return StatusCode::SUCCESS ;
}


// ============================================================================
// The END 
// ============================================================================
