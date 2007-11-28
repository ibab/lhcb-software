// $Id: HistoBook.cpp,v 1.2 2007-11-28 14:08:51 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/ToStream.h"
// ============================================================================
// LoKi
// ============================================================================ 
#include "LoKi/Services.h"
#include "LoKi/HistoBook.h"
#include "LoKi/Report.h"
// ============================================================================ 
/** @file 
 *  Implementation file for namespace LoKi::HistoBook
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-11-28
 */
// ============================================================================ 
/* helper function to book 1D-histogram
 *  @param svc pointer to Histogram Service 
 *  @param path full path in Histogram Data Store 
 *  @param hist histogram desctription
 */ 
// ============================================================================ 
AIDA::IHistogram1D* LoKi::HistoBook::book
( const std::string&       path ,
  const Gaudi::Histo1DDef& hist ) 
{
  const LoKi::Services& svcs = LoKi::Services::instance() ;
  IHistogramSvc* svc = svcs.histoSvc();  
  if ( 0 == svc ) { return 0 ; }                              // RETURN 
  // check the existing histogram:
  AIDA::IHistogram1D* histo = 0 ;
  StatusCode sc = svc->retrieveObject ( path , histo ) ;
  if ( sc.isSuccess() && 0 != histo ) { return histo ; }      // RETURN
  // book new histogram:
  histo = Gaudi::Histos::book ( svc , path , hist ) ;
  if ( 0 == histo )
  {
    LoKi::Report::Error
      ( "LoKi::HistoBook::book('"
        + path + "','"
        + Gaudi::Utils::toString ( hist ) + "'): return NULL") ;
    return 0 ;
  }
  return histo ;
}
// ============================================================================ 
/*  helper function to book 1D-histogram
 *  @param svc  pointer to Histogram Service 
 *  @param dir  directory path in Histogram Data Store 
 *  @param id   historgam identifier 
 *  @param hist histogram desctription
 */ 
// ============================================================================ 
AIDA::IHistogram1D* LoKi::HistoBook::book
( const std::string&       dir  ,
  const std::string&       id   ,
  const Gaudi::Histo1DDef& hist ) 
{
  const LoKi::Services& svcs = LoKi::Services::instance() ;
  IHistogramSvc* svc = svcs.histoSvc();  
  if ( 0 == svc ) { return 0 ; }                              // RETURN 
  // check the existing histogram:
  AIDA::IHistogram1D* histo = 0 ;
  StatusCode sc = svc->retrieveObject ( dir , id  , histo ) ;
  if ( sc.isSuccess() && 0 != histo ) { return histo ; }      // RETURN
  // book new histogram:
  histo = Gaudi::Histos::book ( svc , dir , id , hist ) ;
  if ( 0 == histo )
  {
    LoKi::Report::Error
      ( "LoKi::HistoBook::book('"
        + dir  + "','"
        + id   + "','"
        + Gaudi::Utils::toString ( hist ) + "'): return NULL") ;
    return 0 ;
  }
  return histo ; 
}
// ============================================================================ 
/* helper function to book 1D-histogram
 *  @param svc pointer to Histogram Service 
 *  @param dir  directory path in Histogram Data Store 
 *  @param id  historgam identifier 
 *  @param hist histogram desctription
 */ 
// ============================================================================ 
AIDA::IHistogram1D* LoKi::HistoBook::book
( const std::string&       dir  ,
  const int                id   ,
  const Gaudi::Histo1DDef& hist ) 
{
  const LoKi::Services& svcs = LoKi::Services::instance() ;
  IHistogramSvc* svc = svcs.histoSvc();  
  if ( 0 == svc ) { return 0 ; }                              // RETURN 
  // check the existing histogram:
  AIDA::IHistogram1D* histo = 0 ;
  StatusCode sc = svc->retrieveObject ( dir , id  , histo ) ;
  if ( sc.isSuccess() && 0 != histo ) { return histo ; }      // RETURN
  // book new histogram:
  histo = Gaudi::Histos::book ( svc , dir , id , hist ) ;
  if ( 0 == histo )
  {
    LoKi::Report::Error
      ( "LoKi::HistoBook::book('" 
        + dir  + "','"
        + Gaudi::Utils::toString ( id   ) + "','"
        + Gaudi::Utils::toString ( hist ) + "'): return NULL") ;
    return 0 ;
  } 
  return histo ;
}
// ============================================================================


// ============================================================================ 
// The END 
// ============================================================================ 
