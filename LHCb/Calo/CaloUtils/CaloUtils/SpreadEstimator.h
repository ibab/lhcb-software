// $Id: SpreadEstimator.h,v 1.6 2009-05-10 15:17:00 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
#ifndef CALOUTILS_SPREADESTIMATOR_H 
#define CALOUTILS_SPREADESTIMATOR_H 1
// ============================================================================
// Include files
// ============================================================================
// ST D& STL 
// ============================================================================
#include <functional> 
// ============================================================================
// GaudiKernel 
// ============================================================================
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/StatEntity.h"
// ============================================================================
// RecEvent
// ============================================================================
#include "Event/CaloDataFunctor.h"
#include "Event/CellID.h"
// ============================================================================
/// forward declarations 
// ============================================================================
class DeCalorimeter;    // from CaloDet package 
// ============================================================================
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
  // ==========================================================================
 public:
  // ==========================================================================  
  /** standard/default constructor 
   *  @param Det pointer to calorimeter detector
   */ 
  SpreadEstimator( const DeCalorimeter* Det = 0 );
  // ==========================================================================  
  /// destructor 
  ~SpreadEstimator();
  // ==========================================================================  
  /** calculation of cluster spread  
   *
   *  Error codes:
   *   - 221 - invalid source of detector information  
   *
   *  @param  cluster  pointer to cluster object 
   *  @return status code
   */
  StatusCode operator() ( LHCb::CaloCluster* cluster ) const ;
  // ==========================================================================
  /** calculate spread for cluster
   *  @param  cluster  pointer to cluster
   *  @return status code
   */
  inline StatusCode calculateSpread 
  ( LHCb::CaloCluster* cluster ) const
  { return (*this)( cluster ); }
  // ==========================================================================  
public:
  // ==========================================================================  
  /** set new value for calorimeter 
   *  @param Det pointer to calorimeter detector 
   */
  void setDetector ( const DeCalorimeter* Det ) ;
  // ==========================================================================
  /** simple accessor to DeCalorimeter object
   *  @return pointer to detector 
   */
  inline const DeCalorimeter* detector    () const
    { return m_detector    ; }
  // ==========================================================================  
  /// get the counter for problematic cases 
  const StatEntity& invalidRatio  () const { return m_ratio  ; }
  /// get the counter for problematic cases 
  const StatEntity& invalidCells  () const { return m_cells  ; }
  /// get the counter for problematic cases 
  const StatEntity& invalidEnergy () const { return m_energy ; }
  // ==========================================================================  
private:
  // ==========================================================================
  /// the detector elemenet 
  const DeCalorimeter*  m_detector ;                   // the detector elemenet 
  // ==========================================================================
  /// counter of invalid cells 
  mutable StatEntity m_cells  ;                    // counter for invaild cells 
  /// counter of invalid size ratio  
  mutable StatEntity m_ratio  ;               // counter for invaild size ratio
  /// counter of invalid energy 
  mutable StatEntity m_energy ;                   // counter for invaild energy 
  // ==========================================================================  
  typedef LHCb::CaloDataFunctor::EnergyTransverse<const LHCb::CaloCluster*,
                                                  const DeCalorimeter*> ET ;
  /// transverse energy estimator 
  ET m_et ; // transverse energy estimator
  // ==========================================================================  
};
// ============================================================================
// The END 
// ============================================================================
#endif // CALOUTILS_SPREADESTIMATOR_H
// ============================================================================
