// $Id: CaloClustersMCTruth3Alg.h,v 1.1 2002-06-26 19:03:56 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2002/06/13 12:28:48  ibelyaev
//  new options and new algorithm
//
// ============================================================================
#ifndef CALOASSOCIATORS_CALOCLUSTERSMCTRUTH3ALG_H 
#define CALOASSOCIATORS_CALOCLUSTERSMCTRUTH3ALG_H 1
// Include files
// from STL
#include <string>
// from Gaudi
#include "CaloKernel/CaloAlgorithm.h"

/** @class CaloClustersMCTruth3Alg CaloClustersMCTruth3Alg.h
 *  
 *  The algorithm for building the relations
 *  between CaloClusters and MCParticles
 *
 *  This implementation is "primitive" - it relate
 *  CaloCluster object with MCParticle, which HAS a deposition 
 *  of active energy with exceed of some threshold.
 *  the deposited energy is the "weight" of the relation
 * 
 *  @see CaloCluster
 *  @see MCParticle
 *  @see Relation2D 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   08/04/2002
 */
class CaloClustersMCTruth3Alg
  : public CaloAlgorithm
{
  // friend factory for instantiation 
  friend AlgFactory<CaloClustersMCTruth3Alg>; 
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
  CaloClustersMCTruth3Alg
  ( const std::string& name , 
    ISvcLocator*       svc  );
  
  /// destructor (virtual and protected)
  virtual ~CaloClustersMCTruth3Alg();
  
private:
  
  /// default  constructor is private
  CaloClustersMCTruth3Alg();
  /// copy     constructor is private
  CaloClustersMCTruth3Alg
  ( const CaloClustersMCTruth3Alg& );
  /// assignement operator is private
  CaloClustersMCTruth3Alg& operator=
  ( const CaloClustersMCTruth3Alg& );
  
private:

  double      m_threshold ;
  std::string m_particles ;
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // CALOCLUSTERSMCTRUTH3ALG_H
// ============================================================================
