// ============================================================================
/// Include files
#include "GaudiKernel/ToStream.h"
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
 */
// ============================================================================
CovarianceEstimator::CovarianceEstimator( const DeCalorimeter* Detector ): 
  m_detector     ( Detector             ){}

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
  if( nullptr == cluster         ) { return StatusCode::SUCCESS ; }
  if( cluster->entries().empty() ) { return StatusCode::SUCCESS ; }
  if( nullptr == detector()      ) { return StatusCode(221)     ; }
  
  
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

  const LHCb::CaloCellID seedID = cluster->seed();
  // the matrices:
  double See   = s2E( seedID ); // add constant term to global cov(EE)
  double Sex   = 0 ; 
  double Sxx   = s2X( seedID ); // cov(XX)_0
  double Sey   = 0 ; 
  double Sxy   = 0 ; 
  double Syy   = s2Y( seedID ); // cov(YY)_0
  double SeeP  = s2E( seedID );
  double SexEP = 0 ; 
  double SeyEP = 0 ; 
  double SeeEP = s2E( seedID );
  using namespace LHCb::CaloDigitStatus;

  int i=0;
  for( LHCb::CaloCluster::Entries::iterator it = entries.begin() ; entries.end() != it ; ++ it ){    
    const LHCb::CaloDigit* digit  = it->digit() ;    
    /// check the status 

    if( nullptr == digit )continue;
    if((it->status() & LHCb::CaloDigitStatus::UseForEnergy ) || ( it->status() & LHCb::CaloDigitStatus::UseForPosition))
      it->addStatus    ( LHCb::CaloDigitStatus::UseForCovariance   );
    else    
      it->removeStatus ( LHCb::CaloDigitStatus::UseForCovariance   );    
    if( (it->status() & UseForCovariance) == 0 ) { continue; }
    

    const LHCb::CaloCellID id     = digit->cellID();
    const double fraction = it->fraction()  ;
    const double energy   = digit->e() * fraction   ;
    const double e_i  =   energy  ;
    
    // get cell position 
    const Gaudi::XYZPoint& pos = detector()->cellCenter( id ) ;
    const double x_i  =   pos.x() ;
    const double y_i  =   pos.y() ;
    
    // intrinsic resolution 
    double s2 = fabs( energy )  * a2GeV(id ) ;
    //  gain fluctuation
    if( 0 != s2gain (id) ) s2 += energy * energy * s2gain (id) ; 
    
    //  noise (both coherent and incoherent) 
    double g = 0;
    if( 0 != s2noise( id ) ){ 
      g   = detector()->cellGain( digit->cellID() ); 
      s2 += s2noise( id ) * g * g ; 
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
    if( 0 == s2coherent( id ) ) { continue ; } ///<  CONTINUE  
    x    [i] = x_i ;
    y    [i] = y_i ;
    gain [i] = g ;
    int j=0;      
    for( LHCb::CaloCluster::Entries::iterator jt = entries.begin() ; jt < it ; ++jt ){    
      if( jt->digit() == 0 )continue;
      if( (jt->status() & UseForCovariance) == 0 ) continue; 
      
      // position of cell "j"
      const double x_j  =   x[j] ;
      const double y_j  =   y[j] ;
      
      // covariance between cell "i" and "j"
      const double s_ij = s2coherent( id ) * gain[i] * gain[j] ;
      
      
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
  if( 0. >= eTE )parameters( LHCb::CaloPosition::E ) =  0.;
  if( 0. >= eTP ){
    parameters( LHCb::CaloPosition::X ) =  0. ;
    parameters( LHCb::CaloPosition::Y ) =  0. ;
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
      << std::endl 
      << "   Resolution       is [" << ( m_A  ) << "]" 
      << std::endl 
      << "   Sigma Gain       is [" << ( m_GainError              )  << "]" 
      << std::endl 
      << "   Incoherent Noise   is " << ( m_IncoherentNoise      )  << "]"
      << std::endl 
      << "   Coherent Noise is " << ( m_CoherentNoise     )  << "]"
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
      << " Detector is " <<  ( 0 == m_detector ? "INVALID" : "VALID" ) << std::endl 
      << "   Resolution        is " <<  Gaudi::Utils::toString(m_A)               << "" << std::endl 
      << "   Sigma Gain        is " <<  Gaudi::Utils::toString(m_GainError)       << "" << std::endl 
      << "   Coherent Noise    is " <<  Gaudi::Utils::toString(m_CoherentNoise)   << "" << std::endl 
      << "   InCoherent Noise  is " <<  Gaudi::Utils::toString(m_IncoherentNoise) << "" << std::endl ;
  ///
  return log ;
}
// ============================================================================



// ============================================================================
// The End 
// ============================================================================


