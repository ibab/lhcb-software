// $Id: CaloClustersMCTruth2Alg.h,v 1.3 2004-02-17 12:11:33 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2003/01/17 14:15:25  sponce
// support for gcc 3.2
//
// Revision 1.1  2002/06/13 12:28:48  ibelyaev
//  new options and new algorithm
//
// ============================================================================
#ifndef CALOASSOCIATORS_CALOCLUSTERSMCTruth2Alg_H 
#define CALOASSOCIATORS_CALOCLUSTERSMCTruth2Alg_H 1
// Include files
// from STL
#include <string>
// from Gaudi
#include "CaloKernel/CaloAlgorithm.h"

/** @class CaloClustersMCTruth2Alg CaloClustersMCTruth2Alg.h
 *  
 *  The algorithm for building the relations
 *  between CaloClusters and MCParticles
 *
 *  This implementation is "primitive" - it relates
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

class CaloClustersMCTruth2Alg
  : public CaloAlgorithm
{
  // friend factory for instantiation 
  friend class AlgFactory<CaloClustersMCTruth2Alg>; 
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
  CaloClustersMCTruth2Alg
  ( const std::string& name , 
    ISvcLocator*       svc  );
  
  /// destructor (virtual and protected)
  virtual ~CaloClustersMCTruth2Alg();
  
private:
  
  /// default  constructor is private
  CaloClustersMCTruth2Alg();
  /// copy     constructor is private
  CaloClustersMCTruth2Alg
  ( const CaloClustersMCTruth2Alg& );
  /// assignement operator is private
  CaloClustersMCTruth2Alg& operator=
  ( const CaloClustersMCTruth2Alg& );
  
private:
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // CALOCLUSTERSMCTruth2Alg_H
// ============================================================================
