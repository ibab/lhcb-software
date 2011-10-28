// $Id: HistoStats.h,v 1.3 2007/09/19 08:16:59 marcocle Exp $
// ============================================================================
#ifndef ANALYSIS_HSTATS_H
#define ANALYSIS_HSTATS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <utility>
// ============================================================================
// Gaudi
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// LHCMath
// ============================================================================
#include "LHCbMath/ValueWithError.h"
// ============================================================================
// forward declarations
// ============================================================================
class TH1 ; // ROOT 
// ============================================================================
namespace Gaudi
{
  // ==========================================================================
  namespace Utils
  {
    // ========================================================================
    /** @class HStats HStats.h Analysis/HStats.h
     *  The collection of trivial functions to access  the
     *  statistical information for the histograms
     *  @see Gaudi::Utils::HistoStats 
     *  @see Gaudi::Utils::HStats
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-10-28
     */
    class GAUDI_API HStats
    {
    public :
      // ======================================================================
      /** get the "bin-by-bin"-moment around the specified  "value"
       *  @param histo histogram
       *  @param order the moment parameter
       *  @param value central value
       *  @return the evaluated moment
       */
      static double moment
      ( const TH1*                histo     ,
        const unsigned int        order     ,
        const double              value = 0 ) ;
      // ======================================================================
      /** evaluate the uncertanty for 'bin-by-bin'-moment
       *  @param histo histogram
       *  @param order the moment parameter
       *  @param value central value
       *  @return the evaluated uncertanty in the moment
       */
      static double momentErr
      ( const TH1*                histo ,
        const unsigned int        order ) ;
      // ======================================================================
      /** evaluate the 'bin-by-bin'-central moment (around the mean value)
       *  @param histo histogram
       *  @param order the moment parameter
       *  @param value central value
       *  @return the evaluated central moment
       */
      static double centralMoment
      ( const TH1*                histo ,
        const unsigned int        order ) ;
      // ======================================================================
      /** evaluate the uncertanty for 'bin-by-bin'-central moment
       *  (around the mean value)
       *  ( the uncertanty is calculated with O(1/n2) precision)
       *  @param histo histogram
       *  @param order the moment parameter
       *  @param value central value
       *  @return the evaluated uncertanty in the central moment
       */
      static double centralMomentErr
      ( const TH1*                histo ,
        const unsigned int        order ) ;
      // ======================================================================
      /// get the skewness for the histogram
      static double skewness    ( const TH1* histo ) ;
      // ======================================================================
      /// get the error in skewness for the histogram
      static double skewnessErr ( const TH1* histo ) ;
      // ======================================================================
      /// get the kurtosis for the histogram
      static double kurtosis    ( const TH1* histo ) ;
      // ======================================================================
      /// get the error in kurtosis for the histogram
      static double kurtosisErr ( const TH1* histo ) ;
      // ======================================================================
      /// get the mean value for the histogram  (just for completeness)
      static double mean        ( const TH1* histo ) ;
      // ======================================================================
      /// get an error in the mean value
      static double meanErr     ( const TH1* histo ) ;
      // ======================================================================
      /// get the rms value for the histogram  (just for completeness)
      static double rms         ( const TH1* histo ) ;
      // ======================================================================
      /// get an error in the rms value
      static double rmsErr      ( const TH1* histo ) ;
      // ======================================================================
      /// get the effective entries   (just for completeness)
      static double nEff        ( const TH1* histo ) ;
      // ======================================================================
    } ;
    // ========================================================================
  } // end of namespace Gaudi::Utils
  // ==========================================================================
} // end of namespace Gaudi
// ============================================================================
// The END
// ============================================================================
#endif // ANALYSIS_HSTATS_H
// ============================================================================
