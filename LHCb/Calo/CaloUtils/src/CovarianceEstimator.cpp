// $Id: CovarianceEstimator.cpp,v 1.16 2007-01-15 07:53:00 cattanem Exp $ 
// ============================================================================
#define CALOUTILS_COVARIANCEESTIMATOR_CPP 1 
// ============================================================================
/// Include files
#include<cmath>
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/MsgStream.h"
#include "CaloDet/DeCalorimeter.h"
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
  const double         NoiseCo  ,
  const double         ResB     )
  : m_detector     ( Detector             ) 
  , m_a2GeV        ( ResA    * ResA * Gaudi::Units::GeV )
  , m_b2           ( ResB    * ResB       )
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
StatusCode CovarianceEstimator::operator()( LHCb::CaloCluster* cluster ) const 
{
  // ignore trivial cases 
  if( 0 == cluster               ) { return StatusCode::SUCCESS ; }
  if( cluster->entries().empty() ) { return StatusCode::SUCCESS ; }
  // the detector information is not available
  if( 0 == detector()            ) { return StatusCode(221)     ; }
  
  // avoid long names 
  typedef LHCb::CaloCluster::Entries::iterator       iterator;
  typedef LHCb::CaloCluster::Entries::const_iterator const_iterator;
  
  LHCb::CaloCluster::Entries entries = cluster->entries();
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
      LHCb::CaloClusterEntry& entry = entries[i];
      const LHCb::CaloDigit* digit  = entry.digit() ;
      /// get the status 
      if( 0 != digit && 
          ( entry.status() & LHCb::CaloDigitStatus::UseForCovariance ) ) 
        {
          use[i] = true ; ///< use this cell!
          entry.addStatus    ( LHCb::CaloDigitStatus::UseForEnergy   );
          entry.addStatus    ( LHCb::CaloDigitStatus::UseForPosition );
        }
      else 
        {    
          entry.removeStatus ( LHCb::CaloDigitStatus::UseForEnergy   );
          entry.removeStatus ( LHCb::CaloDigitStatus::UseForPosition );
        }
      if( !use[i] )                   { continue; } ///< CONTINUE !
      /// 
      const double       fraction = entry.fraction()  ;
      const double       energy   = digit->e() * fraction   ;
      ///
      const double e_i  =   energy  ;
      // get cell position 
      const Gaudi::XYZPoint& pos = detector()->cellCenter( digit->cellID() ) ;
      ///
      const double x_i  =   pos.x() ;
      const double y_i  =   pos.y() ;
      // intrinsic resolution 
      double s2 = fabs( energy )  * a2GeV() ; 
      if( 0 != b2     () ) { s2 += energy * energy * b2     () ; }  
      //  gain fluctuation
      if( 0 != s2gain () ) { s2 += energy * energy * s2gain () ; }  
      //  noise (both coherent and incoherent) 
      double g = 0;
      if( 0 != s2noise() ) 
        { 
          g   = detector()->cellGain( digit->cellID() ); 
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
      LHCb::CaloPosition::Parameters& parameters = cluster->position().parameters();
      parameters( 1 ) =  -1 * Gaudi::Units::TeV ;
      parameters( 2 ) =  -1 * Gaudi::Units::km  ;
      parameters( 3 ) =  -1 * Gaudi::Units::km  ;
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
  LHCb::CaloPosition::Parameters& parameters = cluster->position().parameters();
  parameters( LHCb::CaloPosition::E ) = Ecl ;   // E 
  parameters( LHCb::CaloPosition::X ) = Xcl ;   // X 
  parameters( LHCb::CaloPosition::Y ) = Ycl ;   // Y 

  // update cluster matrix   
  LHCb::CaloPosition::Covariance& covariance = cluster->position().covariance();
  covariance( LHCb::CaloPosition::X , LHCb::CaloPosition::X ) = CovXX ;
  covariance( LHCb::CaloPosition::Y , LHCb::CaloPosition::X ) = CovXY ;
  covariance( LHCb::CaloPosition::E , LHCb::CaloPosition::X ) = CovEX ;
  covariance( LHCb::CaloPosition::Y , LHCb::CaloPosition::Y ) = CovYY ;
  covariance( LHCb::CaloPosition::E , LHCb::CaloPosition::Y ) = CovEY ;
  covariance( LHCb::CaloPosition::E , LHCb::CaloPosition::E ) = CovEE ;

  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
/** printout to standard gaudi stream 
 *  @see MsgStream 
 *  @param  log the reference to the standard stream 
 *  @return the reference to the standard stream 
 */
// ============================================================================
MsgStream& CovarianceEstimator::printOut ( MsgStream& log ) const 
{
  log << " Cluster Covariance Estimator: " 
      << " Detector is " <<  ( 0 == m_detector ? "INVALID" : "VALID" )
      << endreq 
      << "   Resolution       is " << ( sqrt( a2GeV() / Gaudi::Units::GeV ) ) 
      << endreq 
      << "   Sigma Gain       is " << ( sqrt( s2gain       ()       ) ) 
      << endreq 
      << "   Coherent Noise   is " << ( sqrt( s2coherent   ()       ) ) 
      << endreq 
      << "   InCoherent Noise is " << ( sqrt( s2incoherent ()       ) )
      << endreq ;
  ///
  return log ;
};
// ============================================================================


// ============================================================================
/** printout to standard gaudi stream 
 *  @see MsgStream 
 *  @param  log the reference to the standard stream 
 *  @return the reference to the standard stream 
 */
// ============================================================================
std::ostream& CovarianceEstimator::printOut ( std::ostream& log ) const 
{
  log << " Cluster Covariance Estimator: " 
      << " Detector is " <<  ( 0 == m_detector ? "INVALID" : "VALID" )
      << std::endl 
      << "   Resolution       is " << ( sqrt( a2GeV() /  Gaudi::Units::GeV ) ) 
      << std::endl 
      << "   Sigma Gain       is " << ( sqrt( s2gain       ()       ) ) 
      << std::endl 
      << "   Coherent Noise   is " << ( sqrt( s2coherent   ()       ) ) 
      << std::endl 
      << "   InCoherent Noise is " << ( sqrt( s2incoherent ()       ) )
      << std::endl ;
  ///
  return log ;
};
// ============================================================================



// ============================================================================
// The End 
// ============================================================================


