// $Id: NeutralPPsFromCPsAlg.h,v 1.1 2002-11-20 20:00:24 ibelyaev Exp $
// ============================================================================
// CVS ta $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef NEUTRALPPSFROMCPSALG_H 
#define NEUTRALPPSFROMCPSALG_H 1
// Include files
// LHCbKernel
#include "Relations/IAssociatorWeighted.h"
// from STL
#include <string>
// from CaloInterfaces
#include "CaloInterfaces/ICaloHypoLikelyhood.h"
// from CaloKernel
#include "CaloKernel/CaloAlgorithm.h"

/** @class NeutralPPsFromCPsAlg NeutralPPsFromCPsAlg.h
 *  
 *  Creator of the neutral ProtoParticles from CaloParticles
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-11-20
 */

class NeutralPPsFromCPsAlg : public CaloAlgorithm 
{
  // friend factory for instantiation
  friend class AlgFactory<NeutralPPsFromCPsAlg>;
  
public:
  
  typedef IAssociatorWeighted<CaloCluster,TrStoredTrack,float> IMatch ;
  typedef IMatch::DirectType                                   Table  ;
  typedef Table::Range                                         Range  ;

public:
  
  /** standard initialization method 
   *  @see CaloAlgorithm
   *  @return statsu code 
   */
  virtual StatusCode initialize () ; 

  /** standard execution  method 
   *  @return statsu code 
   */
  virtual StatusCode execute    () ; 
  
  /** standard finalization method 
   *  @see CaloAlgorithm
   *  @return statsu code 
   */
  virtual StatusCode finalize   () ; 
  
public:
  
  /** Standard constructor
   *  @param name algorithm instance name 
   *  @param pSvc service locator 
   */
  NeutralPPsFromCPsAlg( const std::string& name , 
                        ISvcLocator*       pSvc ) ;
  
  /// destructor 
  virtual ~NeutralPPsFromCPsAlg( );

private :
  
  /** helpful technical method to evaluate 
   *  the ProtoParticle::CaloTrMatch estimator
   *  @see ProtoParticle
   *  @param hypo  pointer to the hypo 
   *  @param table pointer to matching relation table 
   *  @return the value of the estimator 
   */
  double caloTrMatch   ( const  CaloHypo* hypo  , 
                         const Table*     table )  const ;
  
  /** helpful technical method to evaluate 
   *  the ProtoParticle::ClusterMass estimator
   *  @see ProtoParticle
   *  @param hypo  pointer to the hypo 
   *  @return the value of the estimator 
   */
  double clusterMass   ( const CaloHypo*  hypo  )  const ;
  
  /** helpful technical method to evaluate 
   *  the ProtoParticle::ShowerShape estimator
   *  @see ProtoParticle
   *  @param hypo  pointer to the hypo 
   *  @return the value of the estimator 
   */
  double showerShape   ( const CaloHypo*  hypo  )  const ;
  
  /** helpful technical method to evaluate 
   *  the ProtoParticle::CaloDepositID estimator
   *  @see ProtoParticle
   *  @param hypo  pointer to the hypo 
   *  @return the value of the estimator 
   */
  double caloDepositID ( const CaloHypo*  hypo  )  const ;
  
private:
  
  // default constructor  is private 
  NeutralPPsFromCPsAlg();
  // copy    constructor  is private 
  NeutralPPsFromCPsAlg           ( const NeutralPPsFromCPsAlg& );
  // assignement operator is private 
  NeutralPPsFromCPsAlg& operator=( const NeutralPPsFromCPsAlg& );
  
private:

  // hypotheses to be processes 
  
  std::vector<long>                        m_hyposLong         ;
  std::vector<CaloHypotheses::Hypothesis>  m_hypos             ;
  
  std::string                              m_matchType         ;
  std::string                              m_matchName         ;
  IMatch*                                  m_match             ;
  
  std::string                              m_spdprsType        ;
  std::string                              m_spdprsName        ;
  ICaloHypoLikelyhood*                     m_spdprs            ;

  double                                   m_caloTrMatch_bad   ;
  double                                   m_caloDepositID_bad ;
  double                                   m_showerShape_bad   ;
  double                                   m_clusterMass_bad   ;
  
  ClusterFunctors::ClusterFromCalo         m_calo              ;
  
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // NEUTRALPPSFROMCPSALG_H
// ============================================================================
