// $Id: NeutralPPMCTruthAlg.h,v 1.2 2002-09-07 11:01:49 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/09/04 15:16:14  ibelyaev
//  add new associator and it's algorithm
//
// Revision 1.1  2002/06/26 19:03:56  ibelyaev
//  add one more new algorithm for MCTruth
//
// Revision 1.2  2002/06/13 12:28:48  ibelyaev
//  new options and new algorithm
//
// ============================================================================
#ifndef CALOASSOCIATORS_NeutralPPMCTruthAlg_H 
#define CALOASSOCIATORS_NeutralPPMCTruthAlg_H 1
// Include files
// from STL
#include <string>
// from Gaudi
#include "CaloKernel/CaloAlgorithm.h"

/** @class NeutralPPMCTruthAlg NeutralPPMCTruthAlg.h
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
class NeutralPPMCTruthAlg
  : public CaloAlgorithm
{
  // friend factory for instantiation 
  friend AlgFactory<NeutralPPMCTruthAlg>;

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
  NeutralPPMCTruthAlg
  ( const std::string& name , 
    ISvcLocator*       svc  );
  
  /// destructor (virtual and protected)
  virtual ~NeutralPPMCTruthAlg();
  
private:
  
  /// default  constructor is private
  NeutralPPMCTruthAlg();
  /// copy     constructor is private
  NeutralPPMCTruthAlg
  ( const NeutralPPMCTruthAlg& );
  /// assignement operator is private
  NeutralPPMCTruthAlg& operator=
  ( const NeutralPPMCTruthAlg& );
  
private:
  
  typedef IAssociatorWeighted<CaloCluster,MCParticle,float> MCAsct;
  
  /// type of the underlying  MC associator 
  std::string m_asctType ;
  /// name of the underlying  MC associator 
  std::string m_asctName ;
  /// MC associator itself  
  MCAsct*     m_asct     ;
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // NeutralPPMCTruthAlg_H
// ============================================================================
