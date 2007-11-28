// $Id: HistoBook.h,v 1.1 2007-11-28 13:56:33 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_HISTOBOOK_H 
#define LOKI_HISTOBOOK_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/HistoDef.h"
// ============================================================================
/** @file 
 *  set of simple functions to retrieve/book the histograms 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-11-28
 */
// ============================================================================
namespace LoKi 
{
  namespace HistoBook
  {
    // ========================================================================
    /** helper function to book 1D-histogram
     *  @param svc pointer to Histogram Service 
     *  @param path full path in Histogram Data Store 
     *  @param hist histogram desctription
     */ 
    AIDA::IHistogram1D* book
    ( const std::string&       path ,
      const Gaudi::Histo1DDef& hist ) ;
    // ========================================================================
    /** helper function to book 1D-histogram
     *  @param svc  pointer to Histogram Service 
     *  @param dir  directory path in Histogram Data Store 
     *  @param id   historgam identifier 
     *  @param hist histogram desctription
     */ 
    AIDA::IHistogram1D* book
    ( const std::string&       dir  ,
      const std::string&       id   ,
      const Gaudi::Histo1DDef& hist ) ;
    // ========================================================================
    /** helper function to book 1D-histogram
     *  @param svc pointer to Histogram Service 
     *  @param dir  directory path in Histogram Data Store 
     *  @param id  historgam identifier 
     *  @param hist histogram desctription
     */ 
    AIDA::IHistogram1D* book
    ( const std::string&       dir  ,
      const int                id   ,
      const Gaudi::Histo1DDef& hist ) ;
    // ========================================================================
  } // end of namespace LoKi::HistoBook
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_HISTOBOOK_H
// ============================================================================
