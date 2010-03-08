// $Id: NeutralProtoPAlg.h,v 1.9 2010-03-08 01:46:40 odescham Exp $
// ============================================================================
#ifndef GLOBALRECO_NEUTRALPROTOPALG_H 
#define GLOBALRECO_NEUTRALPROTOPALG_H 1
// ============================================================================
// Include files
// ============================================================================
#include "Relations/IRelationWeighted.h"
#include "Relations/IRelation.h"
// Event
#include "Event/Track.h"
#include "Event/CaloHypo.h"
#include "Event/CaloCluster.h"
#include "Event/ProtoParticle.h"
// Calo
#include "CaloUtils/ClusterFunctors.h"
#include "CaloUtils/CaloAlgUtils.h"
#include "CaloInterfaces/ICaloHypo2Calo.h"
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
/** @class NeutralProtoPAlg NeutralProtoPAlg.h
 *
 *  Creator of the neutral ProtoParticles from CaloHypos
 *
 *  The current version fills the following estimators for ProtoParticle
 *
 *  <ul>
 *  <li>  <i>CaloTrMatch</i>     as <b>minimal</b> of this estimator for all
 *        linked <i>CaloHypo</i> objects. The value is extracted from
 *        the relation table/associator as a relation weigth between
 *        <i>CaloCluster</i> and <i>TrStoredTrack</i> objects </li>
 *  <li>  <i>CaloDepositID</i>   as <b>maximal</b> of this estimator for all
 *        linked <i>CaloHypo</i> objects using Spd/Prs estimator tool
 *        written by Frederic Machefert </li>
 *  <li>  <i>CaloShowerShape</i> as <b>maximal</b> of the estimator for
 *        all linked <i>CaloHypo</i> objects. Estimator is equal to the
 *        sum of diagonal elements of cluster spread matrix (2nd order
 *        moments of the cluster) </li>
 *  <li>  <i>ClusterMass</i>     as <b>maximal</b> of the estimator of
 *        cluster mass using smart algorithm by Olivier Deschamp </li>
 *  <li>  <i>PhotonID</i>        as the estimator of PhotonID
 *        using nice identifiaction tool
 *        CaloPhotonEstimatorTool by Frederic Machefert *
 *  </ul>
 *
 *
 *  @author Olivier Deschamps
 *  @date   2006-06-09
 *  Adapted from NeutralPPsFromCPsAlg class (Vanya Belyaev Ivan.Belyaev@itep.ru)
 */
class NeutralProtoPAlg : public GaudiAlgorithm{
  // ==========================================================================
public: 
  // ==========================================================================  
  /// Standard constructor
  NeutralProtoPAlg ( const std::string& name, ISvcLocator* pSvcLocator );
  
  virtual ~NeutralProtoPAlg( ); ///< Destructor
  
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  
protected:
  /// use"light" mode? ( suitable fo recalibration purposes)
  inline bool lightMode() const { return m_light_mode ; }
private:// method  
  double caloTrMatch   ( const LHCb::CaloHypo*  hypo  , const LHCb::Calo2Track::IClusTrTable*     table )  const ;
  double clusterMass   ( const LHCb::CaloHypo*  hypo  )  const ;
  double showerShape   ( const LHCb::CaloHypo*  hypo  )  const ;
  double caloDepositID ( const LHCb::CaloHypo*  hypo  )  const ;
  double neutralID     ( const LHCb::CaloHypo*  hypo  , const LHCb::Calo2Track::IHypoEvalTable* table )  const ;
  double CaloSpd       ( const LHCb::CaloHypo*  hypo  )  const ;
  double CaloPrs       ( const LHCb::CaloHypo*  hypo  )  const ;
  double CaloEcal      ( const LHCb::CaloHypo*  hypo  )  const ;

private:// data
  // ==========================================================================
  std::string               m_protoLocation     ;
  std::vector<std::string>  m_hyposLocations    ;
  std::string               m_matchLocation     ;
  double                    m_caloTrMatch_bad   ;
  double                    m_caloDepositID_bad ;
  double                    m_showerShape_bad   ;
  double                    m_clusterMass_bad   ;
  double                    m_photonID_bad      ;
  LHCb::ClusterFunctors::ClusterFromCalo         m_calo        ;  
  ICaloHypo2Calo* m_toCalo;
  std::map<std::string,std::string> m_pidLocations;
  std::map<std::string,LHCb::Calo2Track::IHypoEvalTable*> m_idTable;
  /// flag to indicate "light/calibration" mode 
  bool  m_light_mode ;
  bool  m_first;
  // ==========================================================================
};
// ============================================================================
// The END 
// ============================================================================
#endif // GLOBALRECO_NEUTRALPROTOPALG_H
// ============================================================================

