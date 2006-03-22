// $Id: SpreadEstimator.h,v 1.5 2006-03-22 18:25:06 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2005/11/07 11:57:13  odescham
// v5r0 - Adapt to the new Track Event Model
//
// Revision 1.3  2004/05/27 13:40:03  cattanem
// v4r7
//
// Revision 1.2  2002/11/13 20:43:36  ibelyaev
//  few bugs are fixed
//
// Revision 1.1  2001/11/22 16:02:33  ibelyaev
//  new utilities
// 
// ============================================================================
#ifndef CALOUTILS_SPREADESTIMATOR_H 
#define CALOUTILS_SPREADESTIMATOR_H 1
// Include files
#include <functional> 
#include "GaudiKernel/StatusCode.h"
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
  public std::unary_function<LHCb::CaloCluster*,StatusCode>
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
   *  @param  cluster  pointer to cluster object 
   *  @return status code
   */
  StatusCode operator()
    ( LHCb::CaloCluster* cluster ) const ;
  
  /** calculate spread for cluster
   *  @param  cluster  pointer to cluster
   *  @return status code
   */
  inline StatusCode calculateSpread 
    ( LHCb::CaloCluster* cluster ) const
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
  
  const DeCalorimeter*  m_detector ;

  mutable unsigned long m_invalid  ;
  
};

// ============================================================================
#endif // CALOUTILS_SPREADESTIMATOR_H
// ============================================================================
