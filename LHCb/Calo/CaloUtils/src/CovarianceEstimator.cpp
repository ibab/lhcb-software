// $Id: CovarianceEstimator.cpp,v 1.6 2002-05-23 11:07:09 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#define CALOUTILS_COVARIANCEESTIMATOR_CPP 1 
// ============================================================================
/// Include files
/// STD & STL
#include<cmath>
/// CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Matrix/SymMatrix.h"
/// GaudiKernel
#include "GaudiKernel/SmartRef.h"
/// CaloDet
#include "CaloDet/DeCalorimeter.h"
/// CaloEvent
#include "Event/CaloCluster.h"
/// local
#include "CaloUtils/CovarianceEstimator.h"

// ============================================================================
/** @file CovarianceEstimator.cpp
 * 
 *  Implementation file for class : CovarianceEstimator
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 06/07/2001 
 */
// ============================================================================

// ============================================================================
/** explicit standard constructor
 *
 *  @param  det pointer to DeCalorimeter object 
 *  @param  ares  calorimeter resolution (stochastic) per sqrt(GeV) 
 *  @param  corr should one take into account shower correlations?
 *  @param  flsf safety factor to be applied to intrinsic shower fluctuation
 *  @param  insf safety factor to be apply to incoherent noise flustuation
 *  @param  cnsf safety factor to be apply to coherent noise fluctuation
 */
// ============================================================================
CovarianceEstimator::CovarianceEstimator
( const DeCalorimeter* Detector ,
  const double         ResA     ,
  const double         GainS    ,
  const double         NoiseIn  ,  
  const double         NoiseCo  )
  : m_detector     ( Detector             ) 
  , m_a2GeV        ( ResA    * ResA * GeV )
  , m_s2gain       ( GainS   * GainS      )
  , m_s2incoherent ( NoiseIn * NoiseIn    )  
  , m_s2coherent   ( NoiseCo * NoiseCo    )
{};

// ============================================================================
// (virtual) destructor 
// ============================================================================
CovarianceEstimator::~CovarianceEstimator(){}

// ============================================================================
/** main method (operator)
    @param pointer to cluster 
    @return status code 
*/
// ============================================================================
StatusCode CovarianceEstimator::operator()( CaloCluster* cluster ) const 
{
  // ignore trivial cases 
  if( 0 == cluster               ) { return StatusCode::SUCCESS ; }
  if( cluster->entries().empty() ) { return StatusCode::SUCCESS ; }
  // the detector information is not available
  if( 0 == detector()            ) { return StatusCode(221)     ; }
  
  std::cout << " before " 
            << *cluster << std::endl ;
  
  // avoid long names 
  typedef CaloCluster::Entries::iterator       iterator;
  typedef CaloCluster::Entries::const_iterator const_iterator;
  
  CaloCluster::Entries& entries = cluster->entries();
  const unsigned int size = entries.size() ;
  // auxillary arrays 
  std::vector<bool>   use  ( size , false ); ///< use this cell?
  std::vector<double> x    ( size , 0     ); ///< x-position of cell [i]
  std::vector<double> y    ( size , 0     ); ///< y-position of cell [i]
  std::vector<double> e    ( size , 0     ); ///< energy     of cell [i]
  std::vector<double> gain ( size , 0     ); ///< gain of cell[i]
  std::vector<double> s2e  ( size , 0     ); ///< e-dispersion of cell [i]
  // calculate intermediate values 
  //    eT = sum_i { 1.0  * e(i) }
  //    eX = sum_i { x(i) * e(i) }
  //    eY = sum_i { y(i) * e(i) }
  // and their covariance matrix 
  double eT  = 0 ; 
  double eX  = 0 ; 
  double eY  = 0 ;
  // the matrix:
  double See = 0 ;
  double Sex = 0 ; double Sxx = 0 ;
  double Sey = 0 ; double Sxy = 0 ; double Syy = 0 ;
  ///
  for( unsigned int i = 0 ; i < size ; ++i )
    {
      CaloClusterEntry& entry = entries[i];
      const CaloDigit* digit  = entry.digit() ;
      /// get the status 
      if( 0 != digit && 
          ( entry.status() & CaloDigitStatus::UseForCovariance ) ) 
        {
          use[i] = true ; ///< use this cell!
          entry.addStatus    ( CaloDigitStatus::UseForEnergy   );
          entry.addStatus    ( CaloDigitStatus::UseForPosition );
        }
      else 
        {    
          entry.removeStatus ( CaloDigitStatus::UseForEnergy   );
          entry.removeStatus ( CaloDigitStatus::UseForPosition );
        }
      if( !use[i] )                   { continue; } ///< CONTINUE !
      /// 
      const double       fraction = entry.fraction()  ;
      const double       energy   = digit->e() * fraction   ;
      ///
      const double e_i  =   energy  ;
      // get cell position 
      const HepPoint3D& pos = detector()->cellCenter( digit->cellID() ) ;
      ///
      const double x_i  =   pos.x() ;
      const double y_i  =   pos.y() ;
      // intrinsic resolution 
      double s2 = abs( energy )  * a2GeV() ; 
      //  gain fluctuation
      if( 0 != s2gain () ) { s2 += energy * energy * s2gain() ; }  
      //  noise (both coherent and incoherent) 
      double g = 0;
      if( 0 != s2noise() ) 
        { 
          g = detector()->cellGain( digit->cellID() ); 
          s2 += s2noise() * g * g ; 
        }
      //
      //
      eT +=       e_i ;               
      eX += x_i * e_i ;
      eY += y_i * e_i ;
      //
      const double s_ii = s2  ;
      //
      See +=       s_ii       ;                   
      Sex += x_i * s_ii       ;               
      Sxx += x_i * s_ii * x_i ; 
      Sey += y_i * s_ii       ;               
      Sxy += x_i * s_ii * y_i ;
      Syy += y_i * s_ii * y_i ;
      //
      // second loop if there exist correlations 
      if( 0 == s2coherent() ) { continue ; } ///<  CONTINUE  
      //
      x    [i] = x_i ;
      y    [i] = y_i ;
      e    [i] = e_i ;
      s2e  [i] = s2 ;
      gain [i] = g ;
      // non-diagonal elements 
      for( unsigned int j = 0 ; j < i  ; ++j )
        {
          /// skip unused digits 
          if( !use[j] ) { continue ; }              ///< CONTINUE !!!
          // position of cell "j"
          const double x_j  =   x[j] ;
          const double y_j  =   y[j] ;
          // covariance between cell "i" and "j"
          const double s_ij = 
            s2coherent() * gain[i] * gain[j] ;
          //         
          See += 2.0 *       s_ij                          ;
          Sex +=       x_i * s_ij       + x_j * s_ij       ;
          Sxx += 2.0 * x_i * s_ij * x_j                    ; 
          Sey +=       y_i * s_ij       + y_j * s_ij       ;
          Sxy +=       x_i * s_ij * y_j + x_j * s_ij * y_i ;
          Syy += 2.0 * y_i * s_ij * y_j                    ;
          ///
        } // loop over non-diagonal elements 
    } // end of loop over all digits/diagonal elements 
  
  // does energy have a reasonable value? 
  if( 0 >= eT ) 
    {
      CaloPosition::Parameters& parameters = cluster->position().parameters();
      parameters( 1 ) =  -1 * TeV ;
      parameters( 2 ) =  -1 * km  ;
      parameters( 3 ) =  -1 * km  ;
      return StatusCode(223)     ; 
    }
  
  // The last step: calculate final quantities
  //   Ecl  =  eT 
  //   Xcl  =  eX / eT 
  //   Ycl  =  eY / eT

  const double Ecl = eT      ;
  const double Xcl = eX / eT ;
  const double Ycl = eY / eT ;
  // and their covariance matrix:  
  const double CovEE =           See           ;
  const double CovEX =           Sex / eT                            
    -                      Xcl * See / eT      ;
  const double CovXX =           Sxx / eT / eT 
    +                Xcl * Xcl * See / eT / eT  
    -                2.0 * Xcl * Sex / eT / eT ;
  const double CovEY =           Sey / eT                              
    -                      Ycl * See / eT      ;
  const double CovXY =           Sxy / eT / eT 
    +                Xcl * Ycl * See / eT / eT  
    -                      Ycl * Sex / eT / eT 
    -                      Xcl * Sey / eT / eT ;
  const double CovYY =           Syy / eT / eT 
    +                Ycl * Ycl * See / eT / eT  
    -                2.0 * Ycl * Sey / eT / eT ;
  
  // update cluster patameters  
  CaloPosition::Parameters& parameters = cluster->position().parameters();
  parameters( 1 ) = Ecl ;   // E 
  parameters( 2 ) = Xcl ;   // X 
  parameters( 3 ) = Ycl ;   // Y 

  // update cluster matrix   
  CaloPosition::Covariance& covariance = cluster->position().covariance();
  covariance.fast( 1 , 1 ) = CovEE ;
  covariance.fast( 2 , 1 ) = CovEX ;
  covariance.fast( 2 , 2 ) = CovXX ;
  covariance.fast( 3 , 1 ) = CovEY ;
  covariance.fast( 3 , 2 ) = CovXY ;
  covariance.fast( 3 , 3 ) = CovYY ;
  
  std::cout << " before " 
            << *cluster << std::endl ;
  
  return StatusCode::SUCCESS;
  
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================


