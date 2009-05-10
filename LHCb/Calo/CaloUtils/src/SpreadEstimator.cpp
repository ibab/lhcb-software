// $Id: SpreadEstimator.cpp,v 1.13 2009-05-10 15:17:00 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.13 $
// ============================================================================
#include <cmath>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/GenericVectorTypes.h"
#include "GaudiKernel/GenericMatrixTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "GaudiKernel/SystemOfUnits.h"
// ============================================================================
// CaloDet 
// ============================================================================
#include "CaloDet/DeCalorimeter.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/CaloCluster.h"
// ============================================================================
// LHCbMath 
// ============================================================================
//#include "LHCbMath/Lomont.h"
#include "LHCbMath/LHCbMath.h"
// ============================================================================
// local
// ============================================================================
#include "CaloUtils/SpreadEstimator.h"
// ============================================================================
/** @file 
 * 
 *  Implementation file for class : SpreadEstimator
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 22/11/2001 
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  double round ( const double value , const double step  ) 
  { 
    const double _aux1 = value / step         ;
    const double _aux2 = std::floor ( _aux1 ) ;
    return ( _aux1 - _aux2 < 0.5 ) ? _aux2 * step :  ( _aux2 + 1 ) * step ;
  } 
  // ==========================================================================
}
// ============================================================================
/*  standard/default constructor 
 *  @param Det pointer to calorimeter detector
 */ 
// ============================================================================
SpreadEstimator::SpreadEstimator( const DeCalorimeter* Det )
  : m_detector ( Det ) 
  , m_cells    ()
  , m_ratio    ()
  , m_energy   ()
  , m_et       ( Det )
{}
// ============================================================================
// destructor 
// ============================================================================
SpreadEstimator::~SpreadEstimator() { m_detector = 0 ; } 
// ============================================================================
/*  set new value for calorimeter 
 *  @param Det pointer to calorimeter detector 
 */
// ============================================================================
void SpreadEstimator::setDetector ( const DeCalorimeter* Det ) 
{ 
  m_detector = Det ; 
  m_et = ET ( detector () ) ;
}
// ============================================================================
/*  calculation of cluster spread  
 *
 *  Error codes:
 *
 *   - 221 - invalid source of detector information  
 *   - 222 - the seed cell was not found
 *   - 223 - strange combination
 *   - 224 - energy is not positive 
 *
 *  @param pointer to cluster object 
 *  @return status code
 */
// ============================================================================
StatusCode SpreadEstimator::operator() ( LHCb::CaloCluster* cluster ) const 
{
  // ignore trivial cases 
  if ( 0 == cluster               ) { return StatusCode::SUCCESS ; }
  if ( cluster->entries().empty() ) { return StatusCode::SUCCESS ; }
  /// detector?
  if ( 0 == detector ()           ) { return StatusCode( 221 )   ; }
  
  /// avoid long names 
  typedef LHCb::CaloCluster::Entries::const_iterator const_iterator;
  
  const double x_prec  = 0.2 * Gaudi::Units::mm  ;
  const double x_prec2 = x_prec * x_prec         ;
  const double e_prec  = 0.2 * Gaudi::Units::MeV ;
  
  ///
  double covxx         = 0 ;
  double covyx         = 0 ;
  double covyy         = 0 ;
  double xmean         = 0 ;
  double ymean         = 0 ;
  double etot          = 0 ;
 
  double x0            = 0 ;
  double y0            = 0 ;
  
  unsigned int ncells  = 0     ;
  double eall          = 0     ;
  
  double cellsize      = -10   ;
  bool   first         = true  ;
  
  const LHCb::CaloCluster::Entries& entries = cluster->entries() ;
  for ( LHCb::CaloCluster::Entries::const_iterator entry = entries.begin() ;
        entries.end() != entry ; ++entry ) 
  {
    const LHCb::CaloDigit* digit = entry->digit()  ;
    if ( 0 == digit ) { continue ; }
    ///    
    const double fraction = entry->fraction();
    ///  
    const double energy   = digit->e() * fraction ;
    ///
    const Gaudi::XYZPoint& pos =
      detector()->cellCenter ( digit->cellID() );
    ///
    if ( entry->status() & LHCb::CaloDigitStatus::SeedCell )
    { cellsize = detector()->cellSize( digit->cellID() ) ; }
    ///
    eall += energy ;
    ///
    if ( energy <= 0 ) { continue ; }                             //  CONTINUE! 
    //
    const double weight = energy  ;                               // ATTENTION! 
    if ( weight <= 0 ) { continue ; }                             //  CONTINUE!  
    ///
    ++ncells ;
    ///
    const double x =  pos.x () ;
    const double y =  pos.y () ;
    
    /// accumulate the energy 
    etot  +=           weight ;
    
    // the first good cell? 
    if ( first ) 
    {
      x0    =     x ;  // adjust the bias 
      y0    =     y ;  // adjust the bias 
      first = false ;  // can skip since everything propto dx and/or dy
      continue      ;  // continue
    }
    
    const double dx = x - x0 ;  // temporary shift 
    const double dy = y - y0 ;  // temporary shift 
    
    xmean +=      dx * weight ;
    ymean +=      dy * weight ;
    //
    covxx += dx * dx * weight ;
    covyx += dy * dx * weight ;
    covyy += dy * dy * weight ;
    ///
  }
  
  // strange combinations 
  if ( ncells    <= 0                     ) { return StatusCode ( 223 ) ; }
  // energy is not positive  
  if ( eall      <= 1 * Gaudi::Units::MeV ) { return StatusCode ( 224 ) ; }
  // energy is not positive  
  if ( etot      <= 1 * Gaudi::Units::MeV ) { return StatusCode ( 224 ) ; }
  // seed cell was not found
  if ( cellsize  <= 0                     ) { return StatusCode ( 222 ) ; }
  /// 
  
  const double uniform  = cellsize * cellsize / 12.  ;
  
  xmean /= etot ;
  ymean /= etot ;
  
  covxx /= etot ;
  covyx /= etot ;
  covyy /= etot ;
  
  covxx -= xmean * xmean ;
  covyx -= ymean * xmean ;
  covyy -= ymean * ymean ;
  
  xmean += x0   ;                      // shift back 
  ymean += y0   ;                      // shift back 
  
  LHCb::CaloPosition::Center& center = cluster->position().center() ;
  center ( LHCb::CaloPosition::X ) = xmean ;
  center ( LHCb::CaloPosition::Y ) = ymean ;

  
  /// could do nothing else for 1 cell "clusters" 
  if ( 1 == ncells  ) 
  {
    covxx = uniform ;
    covyy = uniform ;
    covyx = 0.0     ;
  }
  else 
  {  
    const double trace = covxx + covyy             ;    
    const double diff  = covxx - covyy ;
    const double disc  = std::sqrt ( diff * diff + 4 * ( covyx * covyx ) ) ;
    
    // eigen values:
    const double lambda1 = 0.5  * ( trace - disc ) ;
    const double lambda2 = 0.5  * ( trace + disc ) ;
    
    // minimal eigenvalue 
    const double lambdaMin  = std::min ( lambda1 , lambda2 ) / uniform ;
    const double covMin     = std::min ( covxx   , covyy   ) / uniform ;
    
    const double       s_Cut   = 0.5 ;
    const unsigned int s_Cells =   4 ;
    
    if ( s_Cells >= ncells    ||   // small amout of cells wth e>0  
         s_Cut   >= lambdaMin ||   // small eigenvalue  
         s_Cut   >= covMin     )   // small eigenvalue 
    {
      // =======================================================================
      // the matrix must be modified a bit:
      const double eT = round ( m_et ( cluster ) , e_prec ) ;
      m_ratio  += (float) lambdaMin ;
      m_energy += (float) ( eT / Gaudi::Units::GeV ) ;
      m_cells  +=         ncells    ;
      
      // construct the matrix of eigen vectors
      
      const double diff1 = covxx - lambda1 ;
      const double diff2 = covxx - lambda2 ;
      
      const LHCb::Math::abs_less<double> absLess = LHCb::Math::abs_less<double>() ;

      const double phi = 
        // the matrix is numerically diagonal? => trivial eigenvectors  
        absLess ( covyx , x_prec2 ) ? 0.0 : 
        // find "the best non-zero" eigenvector 
        absLess ( diff1 , diff2   ) ? 
        std::atan2 ( covyx ,  diff2 ) : std::atan2 ( diff1 , -covyx ) ;
      
      const double cphi = std::cos ( phi ) ;
      const double sphi = std::sin ( phi ) ;
      
      const double s2 = sphi * sphi ;
      const double c2 = cphi * cphi ;
      const double cs = cphi * sphi ;
      
      const double newLambda1 = std::max ( lambda1 , uniform ) ;
      const double newLambda2 = std::max ( lambda2 , uniform ) ;
      
      // recalculate the matrix using new eigenvalues 
      covxx = c2 *   newLambda1 + s2 * newLambda2   ;
      covyx = cs * ( newLambda1 -      newLambda2 ) ;
      covyy = s2 *   newLambda1 + c2 * newLambda2   ; 
    } 
  }
  
  LHCb::CaloPosition::Spread& spread = cluster->position().spread() ;
  spread ( LHCb::CaloPosition::X , LHCb::CaloPosition::X ) = covxx ;
  spread ( LHCb::CaloPosition::Y , LHCb::CaloPosition::X ) = covyx ;
  spread ( LHCb::CaloPosition::Y , LHCb::CaloPosition::Y ) = covyy ;
  
  ///
  return StatusCode::SUCCESS ;
}
// ============================================================================
// The End 
// ============================================================================
