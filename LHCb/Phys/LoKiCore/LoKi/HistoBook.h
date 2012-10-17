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
class IHistogramSvc  ;
class IAlgContextSvc ;
// ============================================================================
/** @file LoKi/HistoBook.h
 *
 *  This file is a part of LoKi project -
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2007-11-28
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace HistoBook
  {
    // ========================================================================
    /** helper function to book 1D-histogram
     *  @attention it books the histogram through IHistogramSvc 
     *  @param path full path in Histogram Data Store 
     *  @param hist histogram description
     *  @param svc  histogram service 
     *  @return booked histogram 
     */ 
    AIDA::IHistogram1D* book
    ( const std::string&       path  ,
      const Gaudi::Histo1DDef& hist  , 
      IHistogramSvc*           svc   ) ;
    // ========================================================================
    /** helper function to book 1D-histogram
     *  @attention it books the histogram through IHistogramSvc 
     *  @param dir  directory path in Histogram Data Store 
     *  @param id   histogram identifier 
     *  @param hist histogram description
     *  @param svc  histogram service 
     *  @return booked histogram 
     */ 
    AIDA::IHistogram1D* book
    ( const std::string&       dir      ,
      const std::string&       id       ,
      const Gaudi::Histo1DDef& hist     ,
      IHistogramSvc*           svc  = 0 ) ;
    // ========================================================================
    /** helper function to book 1D-histogram
     *  @attention it books the histogram through IHistogramSvc 
     *  @param dir  directory path in Histogram Data Store 
     *  @param id   histogram identifier 
     *  @param hist histogram desctription
     *  @param svc  histogram service 
     *  @return booked histogram 
     */ 
    AIDA::IHistogram1D* book
    ( const std::string&       dir      ,
      const int                id       ,
      const Gaudi::Histo1DDef& hist     ,
      IHistogramSvc*           svc  = 0 ) ;
    // ========================================================================
    /** helper function to book 1D-histogram
     *  @attention it books the histogram through IHistogramSvc 
     *  @param dir  directory path in Histogram Data Store 
     *  @param id   histogram identifier 
     *  @param hist histogram desctription
     *  @param svc  histogram service 
     *  @return booked histogram 
     */ 
    AIDA::IHistogram1D* book
    ( const std::string&       dir      ,
      const GaudiAlg::ID&      id       ,
      const Gaudi::Histo1DDef& hist     ,
      IHistogramSvc*           svc  = 0 ) ;
    // ========================================================================
    /** helper function to book 1D-histogram
     *  @param alg  the algorithm, whcih owns the histogram
     *  @param id   histogram identifier 
     *  @param hist histogram desctription
     *  @return booked histogram 
     */ 
    AIDA::IHistogram1D* book
    ( const GaudiHistoAlg*     alg      , 
      const std::string&       id       ,
      const Gaudi::Histo1DDef& hist     ) ;
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
     *  @param svc context service 
     *  @return booked histogram 
     */ 
    AIDA::IHistogram1D* book
    ( const Gaudi::Histo1DDef& hist     ,
      const std::string&       id       , 
      const IAlgContextSvc*    svc  = 0 ) ;
    // ========================================================================
    /** helper function to book 1D-histogram
     *  @attention it looks for the appropriate Histo-algorithm within "context"
     *  @param svc the context service 
     *  @param id   histogram identifier 
     *  @param hist histogram desctription
     *  @param svc context service 
     *  @return booked histogram 
     */ 
    AIDA::IHistogram1D* book
    ( const Gaudi::Histo1DDef& hist     ,
      const int                id       , 
      const IAlgContextSvc*    svc  = 0 ) ;
    // ========================================================================
    /** helper function to book 1D-histogram
     *  @attention it looks for the appropriate Histo-algorithm within "context"
     *  @param svc the context service 
     *  @param id   histogram identifier 
     *  @param hist histogram desctription
     *  @param svc context service 
     *  @return booked histogram 
     */ 
    AIDA::IHistogram1D* book
    ( const Gaudi::Histo1DDef& hist     ,
      const GaudiAlg::ID&      id       ,
      const IAlgContextSvc*    svc  = 0 ) ;
    // ========================================================================
  } // end of namespace LoKi::HistoBook
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_HISTOBOOK_H
// ============================================================================
