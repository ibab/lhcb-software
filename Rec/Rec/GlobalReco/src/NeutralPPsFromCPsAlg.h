// $Id: NeutralPPsFromCPsAlg.h,v 1.4 2004-03-17 16:27:56 ibelyaev Exp $
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
#include "CaloInterfaces/ICaloHypoLikelihood.h"
// from CaloKernel
#include "CaloKernel/CaloAlgorithm.h"

/** @class NeutralPPsFromCPsAlg NeutralPPsFromCPsAlg.h
 *  
 *  Creator of the neutral ProtoParticles from CaloParticles
 *
 *  The current version fill following estimators for ProtoParticle
 *  
 *  <li>  <c>CaloTrMatch</c>     as <b>minimal</b> of this estimator for all 
 *        linked <c>CaloHypo</c> objects. The value is extracted from 
 *        the relation table/associator as a relation weigth between 
 *        <c>CaloCluster</c> and <c>TrStoredTrack</c> objects </li>
 *  <li>  <c>CaloDepositID</c>   as <b>maximal</b> of this estimator for all 
 *        linked <c>CaloHypo</c> objects using Spd/Prs estimator tool 
 *        written by Frederic Machefert </li>
 *  <li>  <c>CaloShowerShape</c> as <b>maximal</b> of the estimator for 
 *        all linked <c>CaloHypo</c> objects. Estimator is equal to the 
 *        sum of diagonal elements of cluster spread matrix (2nd order 
 *        moments of the cluster) </li>
 *  <li>  <c>ClusterMass</c>     as <b>maximal</b> of the estimator of 
 *        cluster mass using smart algorithm by Olivier Deschamp </li>
 *  <li>  <c>PhotonID</c>        as the estimator of PhotonID
 *        using nice identifiaction tool 
 *        CaloPhotonEstimatorTool by Frederic Machefert * 
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

  /** helpful technical method to evaluate 
   *  the ProtoParticle::PhotonID estimator
   *  @see ProtoParticle
   *  @param hypo  pointer to the hypo 
   *  @return the value of the estimator 
   */
  double photonID      ( const CaloHypo*  hypo  )  const ;
  
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
  ICaloHypoLikelihood*                     m_spdprs            ;

  std::string                              m_photonIDType      ;
  std::string                              m_photonIDName      ;
  ICaloHypoLikelihood*                     m_photonID          ;

  double                                   m_caloTrMatch_bad   ;
  double                                   m_caloDepositID_bad ;
  double                                   m_showerShape_bad   ;
  double                                   m_clusterMass_bad   ;
  double                                   m_photonID_bad      ;
  
  ClusterFunctors::ClusterFromCalo         m_calo              ;
  
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // NEUTRALPPSFROMCPSALG_H
// ============================================================================
