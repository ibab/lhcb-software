// $Id: HistoBook.cpp,v 1.4 2009-03-22 17:55:24 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/ToStream.h"
#include "GaudiKernel/IAlgContextSvc.h"
#include "GaudiKernel/ISvcLocator.h"
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/IHistoTool.h"
#include "GaudiAlg/GaudiHistos.h"
#include "GaudiAlg/GaudiHistos.icpp"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiAlg/GaudiHistoTool.h"
#include "GaudiAlg/GetAlgs.h"
#include "GaudiAlg/Fill.h"
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
/*  helper function to book 1D-histogram
 *  @param svc pointer to Histogram Service 
 *  @param path full path in Histogram Data Store 
 *  @param hist histogram desctription 
 *  @param histogram service 
 *  @return booked histogram 
 */ 
// ============================================================================ 
AIDA::IHistogram1D* LoKi::HistoBook::book
( const std::string&       path ,
  const Gaudi::Histo1DDef& hist , 
  IHistogramSvc*           svc  ) 
{
  // get the service from LoKi 
  if ( 0 == svc ) {
    const LoKi::Services& svcs = LoKi::Services::instance() ;
    svc = svcs.histoSvc();  
  }
  // 
  if ( 0 == svc )
  {
    LoKi::Report::Error
      ( "LoKi::HistoBook::book("
        + Gaudi::Utils::toString ( path ) + "," 
        + Gaudi::Utils::toString ( hist ) + "): invalid IHistogramSvc") ;
    return 0 ;
  }
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
        + path + "',"
        + Gaudi::Utils::toString ( hist ) + "): invalid AIDA::IHistogramID") ;
    return 0 ;
  }
  //
  LoKi::Report::Print
    ( "LoKi::HistoBook::book("
      + Gaudi::Utils::toString ( path ) + ","
      + Gaudi::Utils::toString ( hist ) + ")"
      + " success using IHistogramSvc," 
      + " THS path: '" 
      + Gaudi::Utils::Histos::path 
      (  Gaudi::Utils::Histos::toBase ( histo ) ) + "'" ,
      StatusCode::SUCCESS , MSG::DEBUG ) ;
  //
  return histo ;
}
// ============================================================================ 
/*  helper function to book 1D-histogram
 *  @param svc  pointer to Histogram Service 
 *  @param dir  directory path in Histogram Data Store 
 *  @param id   historgam identifier 
 *  @param hist histogram desctription
 *  @param histogram service 
 *  @return booked histogram 
 */ 
// ============================================================================ 
AIDA::IHistogram1D* LoKi::HistoBook::book
( const std::string&       dir  ,
  const GaudiAlg::ID&      id   ,
  const Gaudi::Histo1DDef& hist , 
  IHistogramSvc*           svc  )  
{
  // get the service form LoKi 
  if ( 0 == svc ) 
  {
    const LoKi::Services& svcs = LoKi::Services::instance() ;
    svc = svcs.histoSvc();  
  }
  // 
  if ( 0 == svc )
  {
    LoKi::Report::Error
      ( "LoKi::HistoBook::book("
        + Gaudi::Utils::toString ( dir  ) + ","
        + Gaudi::Utils::toString ( id   ) + ","
        + Gaudi::Utils::toString ( hist ) + "): invalid IHistogramSvc") ;
    return 0 ;
  }
  // check the existing histogram:
  AIDA::IHistogram1D* histo = 0 ;
  StatusCode sc = 
    id.literal () ? 
    svc->retrieveObject ( dir , id.literalID   () , histo ) :
    id.numeric () ? 
    svc->retrieveObject ( dir , id.numericID   () , histo ) :
    svc->retrieveObject ( dir , id.idAsString  () , histo ) ;
  //
  if ( sc.isSuccess() && 0 != histo ) { return histo ; }      // RETURN
  // book new histogram:
  histo = 
    id.literal () ? 
    Gaudi::Histos::book ( svc , dir , id.literalID  ()  , hist ) :
    id.numeric () ? 
    Gaudi::Histos::book ( svc , dir , id.numericID  ()  , hist ) :
    Gaudi::Histos::book ( svc , dir , id.idAsString ()  , hist ) ;
  //
  if ( 0 == histo )
  {
    LoKi::Report::Error
      ( "LoKi::HistoBook::book("
        + Gaudi::Utils::toString ( dir  ) + ","
        + Gaudi::Utils::toString ( id   ) + ","
        + Gaudi::Utils::toString ( hist ) + "): invaild AIDA::IHistogramID") ;
    return 0 ;
  }
  //
  LoKi::Report::Print
    ( "LoKi::HistoBook::book("
      + Gaudi::Utils::toString ( dir  ) + ","
      + Gaudi::Utils::toString ( id   ) + ","
      + Gaudi::Utils::toString ( hist ) + ")"
      + " success using IHistogramSvc,"
      + " THS path: '" 
      + Gaudi::Utils::Histos::path 
      (  Gaudi::Utils::Histos::toBase ( histo ) ) + "'" ,
      StatusCode::SUCCESS , MSG::DEBUG ) ;
  //
  return histo ; 
}
// ============================================================================ 
/*  helper function to book 1D-histogram
 *  @param svc pointer to Histogram Service 
 *  @param dir directory path in Histogram Data Store 
 *  @param id  historgam identifier 
 *  @param hist histogram desctription
 *  @param histogram service 
 *  @return booked histogram 
 */ 
// ============================================================================ 
AIDA::IHistogram1D* LoKi::HistoBook::book
( const std::string&       dir  ,
  const int                id   ,
  const Gaudi::Histo1DDef& hist , 
  IHistogramSvc*           svc  ) 
{ return book ( dir , GaudiAlg::ID( id ) , hist , svc ) ; }
// ============================================================================ 
/*  helper function to book 1D-histogram
 *  @param svc pointer to Histogram Service 
 *  @param dir directory path in Histogram Data Store 
 *  @param id  historgam identifier 
 *  @param hist histogram desctription
 */ 
// ============================================================================ 
AIDA::IHistogram1D* LoKi::HistoBook::book
( const std::string&       dir  ,
  const std::string&       id   ,
  const Gaudi::Histo1DDef& hist ,
  IHistogramSvc*           svc  ) 
{ return book ( dir , GaudiAlg::ID ( id ) , hist , svc ) ; }
// ============================================================================
/*  helper function to book 1D-histogram
 *  @param alg  the algorithm, whcih owns the histogram
 *  @param id   histogram identifier 
 *  @param hist histogram desctription
 */ 
// ============================================================================
AIDA::IHistogram1D* LoKi::HistoBook::book
( const GaudiHistoAlg*     alg  , 
  const std::string&       id   ,
  const Gaudi::Histo1DDef& hist ) 
{ return book ( alg , GaudiAlg::ID ( id ) , hist ) ; }
// ============================================================================
/*  helper function to book 1D-histogram
 *  @param tool the tool, which owns the histogram
 *  @param dir  directory path in Histogram Data Store 
 *  @param id   histogram identifier 
 *  @param hist histogram desctription
 */ 
// ============================================================================
AIDA::IHistogram1D* LoKi::HistoBook::book
( const GaudiHistoTool*    tool , 
  const std::string&       id   ,
  const Gaudi::Histo1DDef& hist ) 
{ return book ( tool , GaudiAlg::ID ( id ) , hist ) ; }
// ============================================================================
/*  helper function to book 1D-histogram
 *  @param tool the tool, which owns the histogram
 *  @param dir  directory path in Histogram Data Store 
 *  @param id   histogram identifier 
 *  @param hist histogram desctription
 */ 
// ============================================================================
AIDA::IHistogram1D* LoKi::HistoBook::book
( const IHistoTool*        tool , 
  const std::string&       id   ,
  const Gaudi::Histo1DDef& hist ) 
{ return book ( tool , GaudiAlg::ID ( id ) , hist ) ; }
// ============================================================================
/*  helper function to book 1D-histogram
 *  @param alg  the algorithm, whcih owns the histogram
 *  @param id   histogram identifier 
 *  @param hist histogram desctription
 */ 
// ============================================================================
AIDA::IHistogram1D* LoKi::HistoBook::book
( const GaudiHistoAlg*     alg  , 
  const GaudiAlg::ID&      id   ,
  const Gaudi::Histo1DDef& hist ) 
{
  if ( 0 == alg ) 
  { 
    LoKi::Report::Error
      ( "LoKi::HistoBook::book("
        + Gaudi::Utils::toString ( id   ) + ","
        + Gaudi::Utils::toString ( hist ) + "): invalid GaudiHistoAlg") ;
    return 0 ;
  }
  //
  if ( !alg->produceHistos() ) 
  {
    LoKi::Report::Warning
      ( "LoKi::HistoBook::book("
        + Gaudi::Utils::toString ( id   ) + ","
        + Gaudi::Utils::toString ( hist ) + "),"
        + " histos are disabled for GaudiHistoAlg: '" + alg->name() + "'") ;
    return 0 ;
  }
  // book the histogram:
  AIDA::IHistogram1D* histo = 
    alg -> book ( id               , 
                  hist.title    () , 
                  hist.lowEdge  () , 
                  hist.highEdge () , hist.bins() ) ;
  //
  if ( 0 == histo ) 
  { 
    LoKi::Report::Error
      ( "LoKi::HistoBook::book("
        + Gaudi::Utils::toString ( id   ) + ","
        + Gaudi::Utils::toString ( hist ) + "): invalid AIDA::IHistogram1D") ;
    return 0 ;
  }
  //
  LoKi::Report::Print
    ( "LoKi::HistoBook::book("
      + Gaudi::Utils::toString ( id   ) + ","
      + Gaudi::Utils::toString ( hist ) + ")"
      + " success using GaudiHistoAlg: '" + alg->name() + "'" 
      + " THS path: '" 
      + Gaudi::Utils::Histos::path 
      (  Gaudi::Utils::Histos::toBase ( histo ) ) + "'" ,
      StatusCode::SUCCESS , MSG::DEBUG  ) ;
  //
  return histo ;
}
// ============================================================================
/*  helper function to book 1D-histogram
 *  @param tool the tool, which owns the histogram
 *  @param dir  directory path in Histogram Data Store 
 *  @param id   histogram identifier 
 *  @param hist histogram desctription
 */ 
// ============================================================================
AIDA::IHistogram1D* LoKi::HistoBook::book
( const GaudiHistoTool*    tool , 
  const GaudiAlg::ID&      id   ,
  const Gaudi::Histo1DDef& hist ) 
{
  if ( 0 == tool ) 
  { 
    LoKi::Report::Error
      ( "LoKi::HistoBook::book("
        + Gaudi::Utils::toString ( id   ) + ","
        + Gaudi::Utils::toString ( hist ) + "): invalid GaudiHistoTool") ;
    return 0 ;
  }
  //
  if ( !tool->produceHistos() ) 
  {
    LoKi::Report::Warning
      ( "LoKi::HistoBook::book("
        + Gaudi::Utils::toString ( id   ) + ","
        + Gaudi::Utils::toString ( hist ) + "),"
        + " histos are disabled for GaudiHistoTool: '" + tool->name() + "'") ;
    return 0 ;
  }
  // book the histogram:
  AIDA::IHistogram1D* histo = 
    tool -> book ( id               , 
                   hist.title    () , 
                   hist.lowEdge  () , 
                   hist.highEdge () , hist.bins() ) ;
  //
  if ( 0 == histo ) 
  { 
    LoKi::Report::Error
      ( "LoKi::HistoBook::book("
        + Gaudi::Utils::toString ( id   ) + ","
        + Gaudi::Utils::toString ( hist ) + "): invalid AIDA::IHistogramID") ;
    return 0 ;
  }
  //
  LoKi::Report::Print
    ( "LoKi::HistoBook::book("
      + Gaudi::Utils::toString ( id   ) + ","
      + Gaudi::Utils::toString ( hist ) + ")"
      + " success using GaudiHistoTool: " + tool->name() + "'" 
      + " THS path: '" 
      + Gaudi::Utils::Histos::path 
      (  Gaudi::Utils::Histos::toBase ( histo ) ) + "'" ,
      StatusCode::SUCCESS , MSG::DEBUG  ) ;
  //
  return histo ;
}
// ============================================================================
/*  helper function to book 1D-histogram
 *  @param tool the tool, which owns the histogram
 *  @param dir  directory path in Histogram Data Store 
 *  @param id   histogram identifier 
 *  @param hist histogram desctription
 */ 
// ============================================================================
AIDA::IHistogram1D* LoKi::HistoBook::book
( const IHistoTool*        tool , 
  const GaudiAlg::ID&      id   ,
  const Gaudi::Histo1DDef& hist ) 
{
  if ( 0 == tool  ) 
  { 
    LoKi::Report::Error
      ( "LoKi::HistoBook::book("
        + Gaudi::Utils::toString ( id   ) + ","
        + Gaudi::Utils::toString ( hist ) + "): invalid IHistoTool") ;
    return 0 ;
  }
  // book the histogram:
  AIDA::IHistogram1D* histo = 
    tool -> book ( id               , 
                   hist.title    () , 
                   hist.lowEdge  () , 
                   hist.highEdge () , hist.bins() ) ;
  //
  if ( 0 == histo ) 
  { 
    LoKi::Report::Error
      ( "LoKi::HistoBook::book("
        + Gaudi::Utils::toString ( id   ) + ","
        + Gaudi::Utils::toString ( hist ) + "): invalid AIDA::IHistogramID") ;
    return 0 ; 
  }
  //
  LoKi::Report::Print
    ( "LoKi::HistoBook::book("
      + Gaudi::Utils::toString ( id   ) + ","
      + Gaudi::Utils::toString ( hist ) + ")"
      + " success using IHistoTool: '" + tool->name() + "'"
      + " THS path: '" 
      + Gaudi::Utils::Histos::path 
      (  Gaudi::Utils::Histos::toBase ( histo ) ) + "'" ,
      StatusCode::SUCCESS , MSG::DEBUG  ) ;
  //
  return histo ;  
}
// ============================================================================
/*  helper function to book 1D-histogram
 *  @param svc the context service 
 *  @param id   histogram identifier 
 *  @param hist histogram desctription
 */ 
// ============================================================================
AIDA::IHistogram1D* LoKi::HistoBook::book
( const IAlgContextSvc*    svc , 
  const GaudiAlg::ID&      id   ,
  const Gaudi::Histo1DDef& hist ) 
{
  if ( 0 == svc ) 
  { 
    LoKi::Report::Error
      ( "LoKi::HistoBook::book("
        + Gaudi::Utils::toString ( id   ) + ","
        + Gaudi::Utils::toString ( hist ) + "): invalid IAlgContextSvc") ;
    return 0 ;
  }
  GaudiHistoAlg* alg = Gaudi::Utils::getHistoAlg ( svc ) ;
  return book ( alg , id , hist ) ;
}
// ============================================================================
/*  helper function to book 1D-histogram
 *  @attention it looks for the appropriate Histo-algorithm within "context"
 *  @param svc the context service 
 *  @param id   histogram identifier 
 *  @param hist histogram desctription
 */ 
// ============================================================================
AIDA::IHistogram1D* LoKi::HistoBook::book
( const Gaudi::Histo1DDef& hist ,
  const GaudiAlg::ID&      id   , 
  const IAlgContextSvc*    svc  )
{
  if ( 0 == svc ) 
  {
    const LoKi::Services& svcs = LoKi::Services::instance() ;
    svc = svcs.contextSvc() ;  
  }
  //
  if ( 0 == svc ) 
  { 
    LoKi::Report::Error
      ( "LoKi::HistoBook::book(" 
        + Gaudi::Utils::toString ( hist ) + ","
        + Gaudi::Utils::toString ( id   ) + "): invalid IAlgContextSvc" ) ;
    return 0 ;                                                    // RETURN 
  }
  return book ( svc , id , hist ) ;
}
// ============================================================================
/*  helper function to book 1D-histogram
 *  @attention it looks for the appropriate Histo-algorithm within "context"
 *  @param svc the context service 
 *  @param id   histogram identifier 
 *  @param hist histogram desctription
 */ 
// ============================================================================
AIDA::IHistogram1D* LoKi::HistoBook::book
( const Gaudi::Histo1DDef& hist ,
  const std::string&       id   ,
  const IAlgContextSvc*    svc  )
{ return book ( hist , GaudiAlg::ID ( id ) , svc ) ; }
// ============================================================================
/*  helper function to book 1D-histogram
 *  @attention it looks for the appropriate Histo-algorithm within "context"
 *  @param svc the context service 
 *  @param id   histogram identifier 
 *  @param hist histogram desctription
 */ 
// ============================================================================
AIDA::IHistogram1D* LoKi::HistoBook::book
( const Gaudi::Histo1DDef& hist ,
  const int                id   ,
  const IAlgContextSvc*    svc  )
{ return book ( hist , GaudiAlg::ID ( id ) , svc ) ; }
// ============================================================================
/** helper function to book 1D-histogram
 *  @param svc the context service 
 *  @param id   histogram identifier 
 *  @param hist histogram desctription
 */ 
// ============================================================================
AIDA::IHistogram1D* LoKi::HistoBook::book
( const IAlgContextSvc*    svc  , 
  const std::string&       id   ,
  const Gaudi::Histo1DDef& hist ) 
{ return book ( svc , GaudiAlg::ID ( id ) , hist ) ; }
// ============================================================================ 
// The END 
// ============================================================================ 
