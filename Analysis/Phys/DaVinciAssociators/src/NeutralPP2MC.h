// $Id: NeutralPP2MC.h,v 1.1 2002-09-12 12:16:12 gcorti Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
//  rename when transferign to DaVinciAssociators
// 
// Revision 1.2  2002/09/07 11:01:49  ibelyaev
//  fix the bugs, kindly found by Gloria and Galina
//
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
#ifndef CALOASSOCIATORS_NeutralPP2MC_H 
#define CALOASSOCIATORS_NeutralPP2MC_H 1
// Include files
// from STL
#include <string>
// from Gaudi
#include "CaloKernel/CaloAlgorithm.h"

/** @class NeutralPP2MC NeutralPP2MC.h
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
class NeutralPP2MC
  : public CaloAlgorithm
{
  // friend factory for instantiation 
  friend AlgFactory<NeutralPP2MC>;

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
  NeutralPP2MC
  ( const std::string& name , 
    ISvcLocator*       svc  );
  
  /// destructor (virtual and protected)
  virtual ~NeutralPP2MC();
  
private:
  
  /// default  constructor is private
  NeutralPP2MC();
  /// copy     constructor is private
  NeutralPP2MC
  ( const NeutralPP2MC& );
  /// assignement operator is private
  NeutralPP2MC& operator=
  ( const NeutralPP2MC& );
  
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
#endif // NeutralPP2MC_H
// ============================================================================
