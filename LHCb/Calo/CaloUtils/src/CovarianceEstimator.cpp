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
CovarianceEstimator::CovarianceEstimator( const DeCalorimeter* Detector ,
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
{}

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
StatusCode CovarianceEstimator::operator()( LHCb::CaloCluster* cluster ) const{
  // ignore trivial cases 
  if( 0 == cluster               ) { return StatusCode::SUCCESS ; }
  if( cluster->entries().empty() ) { return StatusCode::SUCCESS ; }
  // the detector information is not available
  if( 0 == detector()            ) { return StatusCode(221)     ; }
  
  // avoid long names 
  //  typedef LHCb::CaloCluster::Entries::iterator       iterator;
  //  typedef LHCb::CaloCluster::Entries::const_iterator const_iterator;
  
  LHCb::CaloCluster::Entries entries = cluster->entries();
  const unsigned int size = entries.size() ;

  // auxillary arrays 
  std::vector<double> x    ( size , 0     ); ///< x-position of cell [i]
  std::vector<double> y    ( size , 0     ); ///< y-position of cell [i]
  std::vector<double> gain ( size , 0     ); ///< gain of cell[i]

  // calculate intermediate values 
  //    eT = sum_i { 1.0  * e(i) }
  //    eX = sum_i { x(i) * e(i) }
  //    eY = sum_i { y(i) * e(i) }
  // and their covariance matrix 


  double eTE  = 0 ; 
  double eTP  = 0 ; 
  double eTEP  = 0 ; 
  double eX  = 0 ; 
  double eY  = 0 ;

  // the matrices:
  double See = 0 ;
  double Sex = 0 ; double Sxx = 0 ;
  double Sey = 0 ; double Sxy = 0 ; double Syy = 0 ;
  double SeeP = 0 ;
  double SexEP = 0 ; double SeyEP=0 ; double SeeEP=0;
  using namespace LHCb::CaloDigitStatus;

  int i=0;
  for( LHCb::CaloCluster::Entries::iterator it = entries.begin() ; entries.end() != it ; ++ it ){    
    const LHCb::CaloDigit* digit  = it->digit() ;
    
    
    /// check the status 
    if( 0 == digit )continue;
    if((it->status() & LHCb::CaloDigitStatus::UseForEnergy ) || ( it->status() & LHCb::CaloDigitStatus::UseForPosition))
      it->addStatus    ( LHCb::CaloDigitStatus::UseForCovariance   );
    else    
      it->removeStatus ( LHCb::CaloDigitStatus::UseForCovariance   );
    
    
    if( (it->status() & UseForCovariance) == 0 ) { continue; }
    
    const double       fraction = it->fraction()  ;
    const double       energy   = digit->e() * fraction   ;
    const double e_i  =   energy  ;
    
    // get cell position 
    const Gaudi::XYZPoint& pos = detector()->cellCenter( digit->cellID() ) ;
    const double x_i  =   pos.x() ;
    const double y_i  =   pos.y() ;
    
    // intrinsic resolution 
    double s2 = fabs( energy )  * a2GeV() ; 
    if( 0 != b2     () )s2 += energy * energy * b2     () ;  
    
    //  gain fluctuation
    if( 0 != s2gain () ) s2 += energy * energy * s2gain () ; 
    
    //  noise (both coherent and incoherent) 
    double g = 0;
    if( 0 != s2noise() ){ 
      g   = detector()->cellGain( digit->cellID() ); 
      s2 += s2noise() * g * g ; 
    }
    
    bool forE = (it->status() & UseForEnergy    ) != 0;
    bool forP = (it->status() & UseForPosition  ) != 0;
    bool forEP = forE && forP != 0;
    
    if( forE ) eTE +=       e_i ;               
    if( forP ){
      eTP +=      e_i ;
      eX += x_i * e_i ;
      eY += y_i * e_i ;
    }
    if( forEP ) eTEP +=       e_i ;               
    
    const double s_ii = s2  ;
    
    if( forE  )See +=       s_ii       ;
    if( forP  ){
      SeeP += s_ii;
      Sxx += x_i * s_ii * x_i ; 
      Sxy += x_i * s_ii * y_i ;
      Syy += y_i * s_ii * y_i ;
      Sex += x_i * s_ii       ;               
      Sey += y_i * s_ii       ;               
    }
    if( forEP ){
      SeeEP += s_ii;
      SexEP += x_i * s_ii       ;               
      SeyEP += y_i * s_ii       ;               
    }
    
    // second loop if there exist correlations 
    if( 0 == s2coherent() ) { continue ; } ///<  CONTINUE  
    x    [i] = x_i ;
    y    [i] = y_i ;
    gain [i] = g ;
    int j=0;      
    for( LHCb::CaloCluster::Entries::iterator jt = entries.begin() ; jt < it ; ++jt ){    
      if( jt->digit() == NULL)continue;
      if( (jt->status() & UseForCovariance) == 0 ) continue; 
      
      // position of cell "j"
      const double x_j  =   x[j] ;
      const double y_j  =   y[j] ;
      
      // covariance between cell "i" and "j"
      const double s_ij = s2coherent() * gain[i] * gain[j] ;
      
      
      bool jforE = (jt->status() & UseForEnergy    ) != 0;
      bool jforP = (jt->status() & UseForPosition  ) != 0;
      bool jforEP = jforE && jforP;
      //         
      if( jforE )See += 2.0 *       s_ij                          ;
      if( jforP ){
        SeeP += 2.0 * s_ij;
        Sxx  += 2.0 * x_i * s_ij * x_j                    ; 
        Sxy  +=       x_i * s_ij * y_j + x_j * s_ij * y_i ;
        Syy  += 2.0 * y_i * s_ij * y_j                    ;
        Sex  +=       x_i * s_ij       + x_j * s_ij       ;
        Sey  +=       y_i * s_ij       + y_j * s_ij       ;
        }
      if( jforEP ){
        SeeEP += 2.0 * s_ij;
        SexEP += x_i * s_ij + x_j * s_ij       ;
        SeyEP += y_i * s_ij + y_j * s_ij       ;
      }
      j++;
    } // end of loop over all digits/diagonal elements 
    i++;
  } // loop over entries
  
    // does energy have a reasonable value? 
  LHCb::CaloPosition::Parameters& parameters = cluster->position().parameters();
  if( 0 >= eTE )parameters( LHCb::CaloPosition::E ) =  -1 * Gaudi::Units::TeV ;
  if( 0 >= eTP ){
    parameters( LHCb::CaloPosition::X ) =  -1 * Gaudi::Units::km  ;
    parameters( LHCb::CaloPosition::Y ) =  -1 * Gaudi::Units::km  ;
  }
  if( 0 >= eTE || 0 >= eTP )return StatusCode(223)     ; 
      
  
  // The last step: calculate final quantities
  //   Ecl  =  eT 
  //   Xcl  =  eX / eT 
  //   Ycl  =  eY / eT

  const double Ecl = eTE      ;
  const double Xcl = eX / eTP ;
  const double Ycl = eY / eTP ;

  // and their covariance matrix:  
  const double CovEE = See ;
  const double CovXX = (Sxx + Xcl * Xcl * SeeP  - 2.0 * Xcl * Sex ) / eTP / eTP ;
  const double CovYY = (Syy + Ycl * Ycl * SeeP  - 2.0 * Ycl * Sey ) / eTP / eTP ;
  const double CovXY = (Sxy + Xcl * Ycl * SeeP  - Ycl * Sex  - Xcl * Sey ) / eTP / eTP;
  const double CovEY = SeyEP / eTEP  - Ycl * SeeEP / eTEP      ;
  const double CovEX = SexEP / eTEP  - Xcl * SeeEP / eTEP      ;
  
  // update cluster patameters  
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


  // === printout
  /*
  std::cout << " ==== CLUSTER COVARIANCE ===== " << cluster->seed()  << std::endl;
  std::cout << " ET(E) = " << eTE << "  ET(P) = " << eTP << " ET(C) = " << eTEP << std::endl;
  std::cout << " Parameters  " << cluster->position().parameters()  << std::endl;
  std::cout << " Covariance  " << cluster->position().covariance()   << std::endl;
  for( LHCb::CaloCluster::Entries::iterator it = entries.begin() ; entries.end() != it ; ++ it ){    
    std::cout << " Entry " 
              << it->digit()->cellID()  
              << " E= "<< it->digit()->e() << " f=" << it->fraction() << " mask = " << it->status() 
              << " forE : " << ((it->status() & UseForEnergy) !=0)
              << " forP : " << ((it->status() & UseForPosition) !=0)
              << " forC : " << ((it->status() & UseForCovariance) != 0)
              << std::endl;
  }
  */

  return StatusCode::SUCCESS;
}
// ============================================================================

// ============================================================================
/** printout to standard gaudi stream 
 *  @see MsgStream 
 *  @param  log the reference to the standard stream 
 *  @return the reference to the standard stream 
 */
// ============================================================================
MsgStream& CovarianceEstimator::printOut ( MsgStream& log ) const{
  log << " Cluster Covariance Estimator: " 
      << " Detector is " <<  ( 0 == m_detector ? "INVALID" : "VALID" )
      << endmsg 
      << "   Resolution       is " << ( sqrt( a2GeV() / Gaudi::Units::GeV ) ) 
      << endmsg 
      << "   Sigma Gain       is " << ( sqrt( s2gain       ()       ) ) 
      << endmsg 
      << "   Coherent Noise   is " << ( sqrt( s2coherent   ()       ) ) 
      << endmsg 
      << "   Incoherent Noise is " << ( sqrt( s2incoherent ()       ) )
      << endmsg ;
  ///
  return log ;
}
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
}
// ============================================================================



// ============================================================================
// The End 
// ============================================================================


