// $Id: CaloClustersMCTruth5Alg.h,v 1.1 2003-04-02 14:46:54 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef CALOASSOCIATORS_CALOCLUSTERSMCTRUTH4ALG_H 
#define CALOASSOCIATORS_CALOCLUSTERSMCTRUTH4ALG_H 1
// Include files
// from STL
#include <string>
// from Gaudi
#include "CaloKernel/CaloAlgorithm.h"

/** @class CaloClustersMCTruth5Alg CaloClustersMCTruth5Alg.h
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
class CaloClustersMCTruth5Alg
  : public CaloAlgorithm
{
  // friend factory for instantiation 
  friend class AlgFactory<CaloClustersMCTruth5Alg>; 
public:


  /** standard execution of the algorithm
   *  @see CaloAlgorithm 
   *  @see     Algorithm 
   *  @see    IAlgorithm 
   *  @return StatusCode
   */
  virtual StatusCode execute    () ;

protected:
  
  /** Standard constructor
   *  @param name name of the algorithm
   *  @param svc  service locator
   */
  CaloClustersMCTruth5Alg
  ( const std::string& name , 
    ISvcLocator*       svc  );
  
  /// destructor (virtual and protected)
  virtual ~CaloClustersMCTruth5Alg();
  
private:
  
  /// default  constructor is private
  CaloClustersMCTruth5Alg();
  /// copy     constructor is private
  CaloClustersMCTruth5Alg
  ( const CaloClustersMCTruth5Alg& );
  /// assignement operator is private
  CaloClustersMCTruth5Alg& operator=
  ( const CaloClustersMCTruth5Alg& );
  
private:

  double      m_threshold1 ;
  double      m_threshold2 ;
  
  typedef     std::vector<std::string> Inputs ;
  Inputs      m_clusters  ;
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // CALOCLUSTERSMCTRUTH3ALG_H
// ============================================================================
