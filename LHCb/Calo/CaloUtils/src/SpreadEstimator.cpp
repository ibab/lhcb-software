// $Id: SpreadEstimator.cpp,v 1.6 2002-11-13 20:43:37 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2002/06/14 09:49:03  ibelyaev
//  bug fix and version change
//
// Revision 1.4  2002/05/23 11:07:09  ibelyaev
//  see /afs/cern.ch/user/i/ibelyaev/w0/Calo/CaloUtils/v2r0/doc/release.notes
//
// ============================================================================
// Include files
// GaudiKernel
#include "GaudiKernel/SmartRef.h"
// CaloDet 
#include "CaloDet/DeCalorimeter.h"
// CaloEvent 
#include "Event/CaloCluster.h"
// local
#include "CaloUtils/SpreadEstimator.h"

// ============================================================================
/** @file SpreadEstimator.cpp 
 * 
 *  Implementation file for class : SpreadEstimator
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 22/11/2001 
 */
// ============================================================================

// ============================================================================
/** standard/default constructor 
 *  @param Det pointer to calorimeter detector
 */ 
// ============================================================================
SpreadEstimator::SpreadEstimator( const DeCalorimeter* Det )
  : m_detector ( Det ) 
  , m_invalid  ( 0   )
{};

// ============================================================================
/** destructor 
 */
// ============================================================================
SpreadEstimator::~SpreadEstimator() 
{ 
  m_detector = 0 ; 
  if( 0 < m_invalid ) 
    {
      std::cout << " **** File '" << __FILE__  << "'" << std::endl  
                << " \t #of invalid covariances   " << m_invalid << std::endl;
    }
};

// ============================================================================
/** calculation of cluster spread  
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
StatusCode SpreadEstimator::operator()
  ( CaloCluster* cluster ) const 
{
  // ignore trivial cases 
  if( 0 == cluster               ) { return StatusCode::SUCCESS ; }
  if( cluster->entries().empty() ) { return StatusCode::SUCCESS ; }
  /// detector?
  if( 0 == detector ()           ) { return StatusCode( 221 )   ; }
  
  /// avoid long names 
  typedef CaloCluster::Entries::const_iterator const_iterator;
  
  ///
  double covxx         = 0 ;
  double covyx         = 0 ;
  double covyy         = 0 ;
  double xmean         = 0 ;
  double ymean         = 0 ;
  double etot          = 0 ;
  
  unsigned int ncells  = 0   ;
  
  double cellsize      = -10 ;
  
  const CaloCluster::Entries& entries = cluster->entries() ;
  for( const_iterator entry = entries.begin() ;
       entries.end() != entry ; ++entry ) 
    {
      const CaloDigit* digit = entry->digit()  ;
      if( 0 == digit ) { continue ; }
      ///    
      const double fraction = entry->fraction();
      ///  
      const double energy   = digit->e() * fraction ;
      ///
      const HepPoint3D& pos =
        detector()->cellCenter( digit->cellID() );
      ///
      if( entry->status() & CaloDigitStatus::SeedCell )
        { cellsize = detector()->cellSize( digit->cellID() ) ; }
      ///
      const double x =  pos.x() ;
      const double y =  pos.y() ;
      ///
      if( energy <= 0 ) { continue ; }
      const double weight = energy > 0.0 ? energy : 0.0 ;
      ///
      ++ncells ;
      ///
      etot  +=         weight ;
      xmean +=     x * weight ;
      ymean +=     y * weight ;
      //
      covxx += x * x * weight ;
      covyx += y * x * weight ;
      covyy += y * y * weight ;
      ///
    }
  ///
  /// strange combinations 
  if( ncells    <= 0 ) { return StatusCode( 223 ) ; }
  /// energy is not positive  
  if( etot      <= 0 ) { return StatusCode( 224 ) ; }
  /// seed cell was not found
  if( cellsize  <= 0 ) { return StatusCode( 222 ) ; }
  
  const double uniform  = cellsize * cellsize / 12.  ;
  const double cut      = 0.01 * cellsize * cellsize ;
  
  xmean /= etot ;
  ymean /= etot ;
  
  CaloPosition::Parameters& center = cluster->position().center() ;
  center( CaloPosition::X ) = xmean ;
  center( CaloPosition::Y ) = ymean ;
  
  covxx /= etot ;
  covyx /= etot ;
  covyy /= etot ;
  
  covxx -= xmean * xmean ;
  covyx -= ymean * xmean ;
  covyy -= ymean * ymean ;
  
  /// could do nothing for 1 cell "clusters" 
  if( 1 == ncells  ) 
    {
      covxx = uniform ;
      covyy = uniform ;
      covyx = 0.0     ;
    }
  
  
//    if( 0 >= covxx || 0 >= covyy )
//      {
//        ++m_invalid ;
//        std::cout << " **** File '"       << __FILE__ 
//                  << "' : Non-positive  Cov(xx) or Cov(yy) "
//                  << " covxx= " << covxx  << "/" << covxx + xmean * xmean 
//                  << " covyx= " << covyx  << "/" << covyx + ymean * xmean 
//                  << " covyy= " << covyy  << "/" << covyy + ymean * ymean 
//                  << " etot="   << etot  
//                  << " ncell="  << ncells << std::endl ;
//        std::cout << *cluster << std::endl ;
//      }

  
  if( covxx < cut ) { covxx = uniform ; covyx = 0 ; }
  if( covyy < cut ) { covyy = uniform ; covyx = 0 ; }
  
  CaloPosition::Covariance& spread = cluster->position().spread() ;
  spread( CaloPosition::X , CaloPosition::X ) = covxx ;
  spread( CaloPosition::Y , CaloPosition::X ) = covyx ;
  spread( CaloPosition::Y , CaloPosition::Y ) = covyy ;

  ///
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
