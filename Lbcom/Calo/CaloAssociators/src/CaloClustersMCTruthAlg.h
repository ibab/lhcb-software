// $Id: CaloClustersMCTruthAlg.h,v 1.1 2002-04-08 15:53:07 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef CALOASSOCIATORS_CALOCLUSTERSMCTRUTHALG_H 
#define CALOASSOCIATORS_CALOCLUSTERSMCTRUTHALG_H 1
// Include files
// from STL
#include <string>
// from Gaudi
#include "CaloKernel/CaloAlgorithm.h"

/** @class CaloClustersMCTruthAlg CaloClustersMCTruthAlg.h
 *  
 *  The algorithm for building the relations
 *  between CaloClusters and MCParticles
 *
 *  This implementation is "primitive" - it relate
 *  CaloCluster object with MCParticles which has the 
 *  largest active energy deposition into the cluster
 *  The value of deposited active energy is the "weight" 
 *  of the relation
 * 
 *  @see CaloCluster
 *  @see MCParticle
 *  @see Relation2D 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   08/04/2002
 */
class CaloClustersMCTruthAlg
  : public CaloAlgorithm
{
  // friend factory for instantiation 
  friend AlgFactory<CaloClustersMCTruthAlg>; 
public:

  /** standard initialization of the algorithm
   *  @see CaloAlgorithm 
   *  @see     Algorithm 
   *  @see    IAlgorithm 
   *  @return StatusCode
   */
  virtual StatusCode initialize () ;

  /** standard execution of the algorithm
   *  @see CaloAlgorithm 
   *  @see     Algorithm 
   *  @see    IAlgorithm 
   *  @return StatusCode
   */
  virtual StatusCode execute    () ;

  /** standard finalization of the algorithm
   *  @see CaloAlgorithm 
   *  @see     Algorithm 
   *  @see    IAlgorithm 
   *  @return StatusCode
   */
  virtual StatusCode finalize   () ;
  
protected:
  
  /** Standard constructor
   *  @param name name of the algorithm
   *  @param svc  service locator
   */
  CaloClustersMCTruthAlg
  ( const std::string& name , 
    ISvcLocator*       svc  );
  
  /// destructor (virtual and protected)
  virtual ~CaloClustersMCTruthAlg();
  
private:
  
  /// default  constructor is private
  CaloClustersMCTruthAlg();
  /// copy     constructor is private
  CaloClustersMCTruthAlg
  ( const CaloClustersMCTruthAlg& );
  /// assignement operator is private
  CaloClustersMCTruthAlg& operator=
  ( const CaloClustersMCTruthAlg& );
  
private:
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // CALOCLUSTERSMCTRUTHALG_H
// ============================================================================
