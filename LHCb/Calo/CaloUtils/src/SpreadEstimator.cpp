// $Id: SpreadEstimator.cpp,v 1.4 2002-05-23 11:07:09 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
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
 *
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
  if( 0 == cluster               ) { return StatusCode::SUCCESS ; }
  if( cluster->entries().empty() ) { return StatusCode::SUCCESS ; }
  /// detector?
  if( 0 == detector ()           ) { return StatusCode( 221 )   ; }
  
  /// avoid long names 
  typedef CaloCluster::Entries::const_iterator const_iterator;
  
  ///
  double covxx = 0 ;
  double covyx = 0 ;
  double covyy = 0 ;
  double xmean = 0 ;
  double ymean = 0 ;
  double etot  = 0 ;
  
  unsigned int n = 0 ;

  const CaloCluster::Entries& entries = cluster->entries() ;
  for( const_iterator entry = entries.begin() ;
       entries.end() != entry ; ++entry ) 
    {
      const CaloDigit* digit = entry->digit()  ;
      if( 0 == digit ) { continue ; }
      ///
      ++n ;
      ///
      const double fraction = entry->fraction();
      const double energy   = digit->e() * fraction ;
      ///
      const HepPoint3D& pos =
        detector()->cellCenter( digit->cellID() );
      ///
      const double x =  pos.x() ;
      const double y =  pos.y() ;
      ///
      const double weight = energy ;
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
  if( 0 != n  ) 
    { 
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
      
      CaloPosition::Covariance& spread = cluster->position().spread() ;
      spread( CaloPosition::X , CaloPosition::X ) = covxx ;
      spread( CaloPosition::Y , CaloPosition::X ) = covyx ;
      spread( CaloPosition::Y , CaloPosition::Y ) = covyy ;      
    }
  
  ///
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
