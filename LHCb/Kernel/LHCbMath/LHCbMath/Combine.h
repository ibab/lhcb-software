//  $Id:$ 
// ============================================================================
#ifndef LHCBMATH_COMBINE_H 
#define LHCBMATH_COMBINE_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"
// ============================================================================
// ROOT
// ============================================================================
#include  "Math/SVector.h"
#include  "Math/SMatrix.h"
// ============================================================================
// LHCbMath
// ============================================================================q
#include "LHCbMath/MatrixUtils.h"
#include "LHCbMath/ValueWithError.h"
#include "LHCbMath/SVectorWithError.h"
// ============================================================================
namespace  Gaudi
{
  // ==========================================================================
  namespace Math
  {
    // ========================================================================
    /** @class Combine
     *  Helper utility to combine   correlated measurements 
     *  @see P.Avery "Combining measurements with correlated errors", CBX 95 55
     *  @see http://www.phys.ufl.edu/~avery/fitting/error_correl.ps.gz
     *  @see http://www.researchgate.net.publication/2345482_Combining_Measurements_with_Correlated_Errors
     *  @author  Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-09-28
     */
    template <unsigned int D, class T=double>
    class Combine
    {
    public:
      // ============================================================================
      typedef  ROOT::Math::SVector<T,D>                                 Data        ;
      typedef  ROOT::Math::SMatrix<T,D,D,ROOT::Math::MatRepSym<T,D> >   Covariance  ;
      typedef  std::vector<Covariance>                                  Covariances ;
      // ============================================================================
    public:
      // =======================================================================
      // constructor from the vector of data and cov matrix 
      Combine ( const Data&       data , 
                const Covariance& cov2 ) 
        : m_data ( data ) 
        , m_cov2 ( cov2 ) 
        , m_w    () 
        , m_one  () 
      {
        Gaudi::Math::setToScalar ( m_one , 1.0 ) ;
        m_w = this->weights() ;
      }

      // ======================================================================
    public:
      // ======================================================================
      /// calculate weights
      Data weights() const 
      {
        // the inverse covariance matrix 
        Covariance vxi ;
        const int ifail = Gaudi::Math::inverse ( m_cov2 , vxi ) ;
        if ( ifail ) {  /* do somehting and regularize it later */ }
        //
        return ( vxi * m_one ) / ROOT::Math::Similarity( vxi , m_one ) ;
      }
      /// the main method:  get a combined value using the calculated weights
      Gaudi::Math::ValueWithError result () const 
      { 
        const double r  = ROOT::Math::Dot ( m_data , m_w  ) ;
        const double e2 = ROOT::Math::Similarity ( m_cov2 , m_w ) ;
        return Gaudi::Math::ValueWithError ( r , e2 ) ;
      }
      // ======================================================================
    private:
      // ======================================================================
      /// input data vector 
      Data        m_data  ;                     // input data vector 
      /// the overall covariance matrix 
      mutable Covariance  m_cov2  ;            // the overall covariance matrix
      // ======================================================================
    private:
      // ======================================================================
      /// weights 
      Data m_w            ; // weights 
      /// helper vector of 
      Data m_one          ;
      // ======================================================================
    } ;  
    // ========================================================================
  } //                                         The end of namespace Gaudi::Math
  // ==========================================================================
} //                                                 The end of namespace Gaudi
// ============================================================================
namespace  Gaudi
{
  namespace  Math
  {
    // ========================================================================
    /** combine two measurements <code>x</code> and <code>y</code>
     *  with covarinace matrix <code>cov</code>
     *  @param x (INPUT) the first  measurement 
     *  @param y (INPUT) the second measurement 
     *  @param cov2 (INPUT) covariance matrix 
     *  @return combined result
     *  @author  Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-09-28
     */
    GAUDI_API 
    Gaudi::Math::ValueWithError 
    combine  ( const double               x   , 
               const double               y   , 
               const Gaudi::SymMatrix2x2& cov ) ;
    // ========================================================================
    /** combine three measurements
     *  with covarinace matrix <code>cov</code>
     *  @param x (INPUT) the first  measurement 
     *  @param y (INPUT) the second measurement 
     *  @param z (INPUT) the second measurement 
     *  @param cov2 (INPUT) covariance matrix 
     *  @return combined result
     *  @author  Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-09-28
     */
    GAUDI_API 
    Gaudi::Math::ValueWithError 
    combine  ( const double               x   , 
               const double               y   , 
               const double               z   , 
               const Gaudi::SymMatrix3x3& cov ) ;
    // ========================================================================
    /** combine four measurements
     *  with covarinace matrix <code>cov</code>
     *  @param x1 (INPUT) the first  measurement 
     *  @param x2 (INPUT) the second measurement 
     *  @param x3 (INPUT) the second measurement 
     *  @param x4 (INPUT) the second measurement 
     *  @param cov2 (INPUT) covariance matrix 
     *  @return combined result
     *  @author  Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-09-28
     */
    GAUDI_API 
    Gaudi::Math::ValueWithError 
    combine  ( const double               x1  , 
               const double               x2  , 
               const double               x3  , 
               const double               x4  , 
               const Gaudi::SymMatrix4x4& cov ) ;
    // ========================================================================
    /** combine five measurements
     *  with covarinace matrix <code>cov</code>
     *  @param x1 (INPUT) the first  measurement 
     *  @param x2 (INPUT) the second measurement 
     *  @param x3 (INPUT) the second measurement 
     *  @param x4 (INPUT) the second measurement 
     *  @param x5 (INPUT) the second measurement 
     *  @param cov2 (INPUT) covariance matrix 
     *  @return combined result
     *  @author  Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-09-28
     */
    GAUDI_API 
    Gaudi::Math::ValueWithError 
    combine  ( const double               x1  , 
               const double               x2  , 
               const double               x3  , 
               const double               x4  , 
               const double               x5  , 
               const Gaudi::SymMatrix5x5& cov ) ;
    // ========================================================================
    /** combine two measurements <code>x1</code> and <code>x2</code>
     *  using correlation coefficient <code>rho</code>:  \f$-1\le\rho\le1\f$
     *  @param x1  (INPUT) the first  measurement 
     *  @param x2  (INPUT) the second measurement 
     *  @param rho (INPUT) correlation coefficient 
     *  @return combined result
     *  @author  Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-09-28
     */
    GAUDI_API 
    Gaudi::Math::ValueWithError 
    combine 
    ( const Gaudi::Math::ValueWithError& x1  ,
      const Gaudi::Math::ValueWithError& x2  , 
      const double                       rho ) ;
    // =========================================================================
    /** combine two measurements <code>x1</code> and <code>x2</code>
     *  using theie "statistical" uncertainties (assumed to be uncorrelated) 
     *  and a covariance matrix of "systematic" uncertainnties
     *  @param x1   (INPUT) the first  measurement 
     *  @param x2   (INPUT) the second measurement 
     *  @param syst (INPUT) covariance matrix of systematic uncertainnties  
     *  @return combined result
     *  @author  Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-09-28
     */
    GAUDI_API 
    Gaudi::Math::ValueWithError 
    combine 
    ( const Gaudi::Math::ValueWithError& x1   ,
      const Gaudi::Math::ValueWithError& x2   , 
      const Gaudi::SymMatrix2x2&         syst ) ;
    // =========================================================================
  }
  // ==========================================================================
}
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LHCBMATH_COMBINE_H
// ============================================================================
