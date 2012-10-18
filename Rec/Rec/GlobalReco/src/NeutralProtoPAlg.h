// ============================================================================
#ifndef GLOBALRECO_NEUTRALPROTOPALG_H 
#define GLOBALRECO_NEUTRALPROTOPALG_H 1
// ============================================================================
// Include files
// ============================================================================
// Event
#include "Event/CaloHypo.h"
#include "Event/ProtoParticle.h"
// Calo
#include "CaloUtils/CaloAlgUtils.h"
#include "CaloInterfaces/ICaloHypoEstimator.h"
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

private:// data
  // ==========================================================================
  std::string               m_protoLocation     ;
  std::vector<std::string>  m_hyposLocations    ;
  double                    m_caloTrMatch_bad   ;
  double                    m_caloDepositID_bad ;
  double                    m_showerShape_bad   ;
  double                    m_clusterMass_bad   ;
  double                    m_photonID_bad      ;
  /// flag to indicate "light/calibration" mode 
  bool  m_light_mode ;
  ICaloHypoEstimator* m_estimator ;
};
#endif // GLOBALRECO_NEUTRALPROTOPALG_H

