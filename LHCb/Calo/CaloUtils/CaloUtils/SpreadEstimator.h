// $Id: SpreadEstimator.h,v 1.1 2001-11-22 16:02:33 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef CALOUTILS_SPREADESTIMATOR_H 
#define CALOUTILS_SPREADESTIMATOR_H 1
// Include files
// STD & STL 
#include <functional> 
// GaudiKernel
#include "GaudiKernel/StatusCode.h"
// forward declaration 
class DeCalorimeter;    ///< from CaloDet package 

/** @class SpreadEstimator SpreadEstimator.h CaloUtils/SpreadEstimator.h
 *  
 *  simple helper class for estimation of spread of 
 *  CaloCluster object.
 *
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   22/11/2001
 */

class SpreadEstimator :
  public std::unary_function<CaloCluster*,StatusCode>
{
  
 public:
  
  /** standard/default constructor 
   *  @param Det pointer to calorimeter detector
   */ 
  SpreadEstimator( const DeCalorimeter* Det = 0 );
  
  /** destructor 
   */
  virtual ~SpreadEstimator();
  
  /** calculation of cluster spread  
   *
   *  Error codes:
   *   - 221 - invalid source of detector information  
   *
   *  @param pointer to cluster object 
   *  @return status code
   */
  StatusCode operator()
    ( CaloCluster* cluster ) const ;
  
  /** calculate spread for cluster
   *  @param pointer to cluster
   *  @return status code
   */
  inline StatusCode calculateSpread 
    ( CaloCluster* cluster ) const
    { return (*this)( cluster ); }
  
  /** set new value for calorimeter 
   *  @param Det pointer to calorimeter detector 
   */
  inline void setDetector
    ( const DeCalorimeter* Det ) 
    { m_detector = Det ; }
  
  /** simple accessor to DeCalorimeter object
   *  @return pointer to detector 
   */
  inline const DeCalorimeter* detector    () const
    { return m_detector    ; }
  
 private:
  
  const DeCalorimeter* m_detector ;
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOUTILS_SPREADESTIMATOR_H
// ============================================================================
