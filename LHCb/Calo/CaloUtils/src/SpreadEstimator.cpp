// $Id: SpreadEstimator.cpp,v 1.2 2001-11-24 11:43:09 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2001/11/22 16:02:35  ibelyaev
//  new utilities
// 
// ============================================================================
// Include files
// GaudiKernel
#include "GaudiKernel/SmartRef.h"
// CaloDet 
#include "CaloDet/DeCalorimeter.h"
// CaloEvent 
#include "CaloEvent/CaloCluster.h"
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
{};

// ============================================================================
/** destructor 
 */
// ============================================================================
SpreadEstimator::~SpreadEstimator() { m_detector = 0 ; };


// ============================================================================
/** calculation of cluster spread  
 *
 *  Error codes:
 *   - 221 - invalid source of detector information  
 *
 *  @param pointer to cluster object 
 *  @return status code
 */
// ============================================================================
StatusCode SpreadEstimator::operator()
  ( CaloCluster* cluster ) const 
{
  // ignore trivial cases 
  if( 0 == cluster              ) { return StatusCode::SUCCESS ; }
  if( cluster->digits().empty() ) { return StatusCode::SUCCESS ; }
  /// detector?
  if( 0 == detector ()          ) { return StatusCode( 221 )   ; }
  
  /// avoid long names 
  typedef CaloCluster::Digits::const_iterator const_iterator;
  
  ///
  const double xmean = cluster->x () ;
  const double ymean = cluster->y () ;
  const double etot  = cluster->e () ;
  
  double covxx = 0 ;
  double covyx = 0 ;
  double covyy = 0 ;

  unsigned int n = 0 ;

  for( const_iterator idigit = cluster->digits().begin() ;
       cluster->digits().end() != idigit ; ++idigit ) 
    {
      const CaloDigit* digit = idigit->first  ;
      if( 0 == digit ) { continue ; }
      ///
      ++n ;
      ///
      const double fraction = 
        idigit->second.fraction();
      const double energy   = digit->e() * fraction ;
      ///
      const HepPoint3D& pos = 
        detector()->cellCenter( digit->cellID() );
      ///
      const double dx =  pos.x() - xmean ;
      const double dy =  pos.y() - ymean ;
      ///
      const double weight = energy / etot ;
      ///
      covxx += dx * dx * weight ;
      covyx += dy * dx * weight ;
      covyy += dy * dy * weight ;
      ///
    }
  /// 
  if( 0 != n  ) 
    {    
      cluster->position().spread().fast( 1 , 1 ) =  covxx ;
      cluster->position().spread().fast( 2 , 1 ) =  covyx ;
      cluster->position().spread().fast( 2 , 2 ) =  covyy ;
    }
  ///
  return StatusCode::SUCCESS ;
};




// ============================================================================
// The End 
// ============================================================================
