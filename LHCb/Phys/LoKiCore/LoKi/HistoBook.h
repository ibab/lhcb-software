// $Id: HistoBook.h,v 1.3 2009-03-20 19:11:05 ibelyaev Exp $
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
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/HistoID.h"
// ============================================================================
// forward decalrations 
// ============================================================================
class GaudiHistoAlg  ;
class GaudiHistoTool ;
class IHistoTool     ;
class IAlgContextSvc ;
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
     *  @attention it books the histogram through IHistogramSvc 
     *  @param path full path in Histogram Data Store 
     *  @param hist histogram description
     */ 
    AIDA::IHistogram1D* book
    ( const std::string&       path ,
      const Gaudi::Histo1DDef& hist ) ;
    // ========================================================================
    /** helper function to book 1D-histogram
     *  @attention it books the histogram through IHistogramSvc 
     *  @param dir  directory path in Histogram Data Store 
     *  @param id   histogram identifier 
     *  @param hist histogram description
     */ 
    AIDA::IHistogram1D* book
    ( const std::string&       dir  ,
      const std::string&       id   ,
      const Gaudi::Histo1DDef& hist ) ;
    // ========================================================================
    /** helper function to book 1D-histogram
     *  @attention it books the histogram through IHistogramSvc 
     *  @param dir  directory path in Histogram Data Store 
     *  @param id   histogram identifier 
     *  @param hist histogram desctription
     */ 
    AIDA::IHistogram1D* book
    ( const std::string&       dir  ,
      const int                id   ,
      const Gaudi::Histo1DDef& hist ) ;
    // ========================================================================
    /** helper function to book 1D-histogram
     *  @attention it books the histogram through IHistogramSvc 
     *  @param dir  directory path in Histogram Data Store 
     *  @param id   histogram identifier 
     *  @param hist histogram desctription
     */ 
    AIDA::IHistogram1D* book
    ( const std::string&       dir  ,
      const GaudiAlg::ID&      id   ,
      const Gaudi::Histo1DDef& hist ) ;
    // ========================================================================
    /** helper function to book 1D-histogram
     *  @param alg  the algorithm, whcih owns the histogram
     *  @param id   histogram identifier 
     *  @param hist histogram desctription
     */ 
    AIDA::IHistogram1D* book
    ( const GaudiHistoAlg*     alg  , 
      const std::string&       id   ,
      const Gaudi::Histo1DDef& hist ) ;
    // ========================================================================
    /** helper function to book 1D-histogram
     *  @param tool the tool, which owns the histogram
     *  @param dir  directory path in Histogram Data Store 
     *  @param id   histogram identifier 
     *  @param hist histogram desctription
     */ 
    AIDA::IHistogram1D* book
    ( const GaudiHistoTool*    tool , 
      const std::string&       id   ,
      const Gaudi::Histo1DDef& hist ) ;
    // ========================================================================
    /** helper function to book 1D-histogram
     *  @param tool the tool, which owns the histogram
     *  @param dir  directory path in Histogram Data Store 
     *  @param id   histogram identifier 
     *  @param hist histogram desctription
     */ 
    AIDA::IHistogram1D* book
    ( const IHistoTool*        tool , 
      const std::string&       id   ,
      const Gaudi::Histo1DDef& hist ) ;
    // ========================================================================
    /** helper function to book 1D-histogram
     *  @param alg  the algorithm, whcih owns the histogram
     *  @param id   histogram identifier 
     *  @param hist histogram desctription
     */ 
    AIDA::IHistogram1D* book
    ( const GaudiHistoAlg*     alg  , 
      const GaudiAlg::ID&      id   ,
      const Gaudi::Histo1DDef& hist ) ;
    // ========================================================================
    /** helper function to book 1D-histogram
     *  @param tool the tool, which owns the histogram
     *  @param id   histogram identifier 
     *  @param hist histogram desctription
     */ 
    AIDA::IHistogram1D* book
    ( const GaudiHistoTool*    tool , 
      const GaudiAlg::ID&      id   ,
      const Gaudi::Histo1DDef& hist ) ;
    // ========================================================================
    /** helper function to book 1D-histogram
     *  @param tool the tool, which owns the histogram
     *  @param dir  directory path in Histogram Data Store 
     *  @param id   histogram identifier 
     *  @param hist histogram desctription
     */ 
    AIDA::IHistogram1D* book
    ( const IHistoTool*        tool , 
      const GaudiAlg::ID&      id   ,
      const Gaudi::Histo1DDef& hist ) ;
    // ========================================================================
    /** helper function to book 1D-histogram
     *  @param svc the context service 
     *  @param id   histogram identifier 
     *  @param hist histogram desctription
     */ 
    AIDA::IHistogram1D* book
    ( const IAlgContextSvc*    svc  , 
      const GaudiAlg::ID&      id   ,
      const Gaudi::Histo1DDef& hist ) ;
    // ========================================================================
    /** helper function to book 1D-histogram
     *  @param svc the context service 
     *  @param id   histogram identifier 
     *  @param hist histogram desctription
     */ 
    AIDA::IHistogram1D* book
    ( const IAlgContextSvc*    svc  , 
      const std::string&       id   ,
      const Gaudi::Histo1DDef& hist ) ;
    // ========================================================================
    /** helper function to book 1D-histogram
     *  @attention it looks for the appropriate Histo-algorithm within "context"
     *  @param svc the context service 
     *  @param id   histogram identifier 
     *  @param hist histogram desctription
     */ 
    AIDA::IHistogram1D* book
    ( const Gaudi::Histo1DDef& hist ,
      const std::string&       id   ) ;
    // ========================================================================
    /** helper function to book 1D-histogram
     *  @attention it looks for the appropriate Histo-algorithm within "context"
     *  @param svc the context service 
     *  @param id   histogram identifier 
     *  @param hist histogram desctription
     */ 
    AIDA::IHistogram1D* book
    ( const Gaudi::Histo1DDef& hist ,
      const GaudiAlg::ID&      id   ) ;
    // ========================================================================
  } // end of namespace LoKi::HistoBook
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_HISTOBOOK_H
// ============================================================================
