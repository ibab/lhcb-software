// $Id: NeutralProtoPAlg.h,v 1.8 2009-08-21 17:08:03 odescham Exp $
// ============================================================================
#ifndef GLOBALRECO_NEUTRALPROTOPALG_H 
#define GLOBALRECO_NEUTRALPROTOPALG_H 1
// ============================================================================
// Include files
// ============================================================================
#include "Relations/IRelationWeighted.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/Track.h"
#include "Event/CaloHypo.h"
#include "Event/CaloCluster.h"
#include "Event/ProtoParticle.h"
// ============================================================================
// Calo
// ============================================================================
#include "CaloInterfaces/ICaloHypoLikelihood.h"
#include "CaloUtils/ClusterFunctors.h"
#include "CaloUtils/CaloAlgUtils.h"
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
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
class NeutralProtoPAlg : public GaudiAlgorithm 
{
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
  // ==========================================================================
  /// locate the tool 
  inline ICaloHypoLikelihood* spdprs() const 
  {
    if ( 0 == m_spdprs ) 
    { m_spdprs   = tool< ICaloHypoLikelihood>( m_spdprsType   ,  m_spdprsName ) ; }
    return m_spdprs ;
  }
  /// locate the tool 
  inline ICaloHypoLikelihood* photonID() const 
  {
    if ( 0 == m_photonID ) 
    { m_photonID = tool< ICaloHypoLikelihood>( m_photonIDType ,  m_photonIDName ) ; }
    return m_photonID ;  
  }  
  /// use"light" mode? ( suitable fo recalibration purposes)
  inline bool lightMode() const { return m_light_mode ; }
  // ==========================================================================
private:// method
  
  
  double caloTrMatch   ( const LHCb::CaloHypo*  hypo  , const LHCb::Calo2Track::IClusTrTable*     table )  const ;
  double clusterMass   ( const LHCb::CaloHypo*  hypo  )  const ;
  double showerShape   ( const LHCb::CaloHypo*  hypo  )  const ;
  double caloDepositID ( const LHCb::CaloHypo*  hypo  )  const ;
  double photonID      ( const LHCb::CaloHypo*  hypo  )  const ;
  double CaloSpd       ( const LHCb::CaloHypo*  hypo  )  const ;
  double CaloPrs       ( const LHCb::CaloHypo*  hypo  )  const ;
  double CaloEcal      ( const LHCb::CaloHypo*  hypo  )  const ;

private:// data
  // ==========================================================================
  std::string               m_protoLocation     ;
  std::vector<std::string>  m_hyposLocations    ;
  std::string               m_matchLocation     ;
  std::string               m_spdprsType        ;
  std::string               m_spdprsName        ;
  mutable ICaloHypoLikelihood*      m_spdprs            ;
  std::string               m_photonIDType      ;
  std::string               m_photonIDName      ;
  mutable ICaloHypoLikelihood*      m_photonID          ;
  double                    m_caloTrMatch_bad   ;
  double                    m_caloDepositID_bad ;
  double                    m_showerShape_bad   ;
  double                    m_clusterMass_bad   ;
  double                    m_photonID_bad      ;
  LHCb::ClusterFunctors::ClusterFromCalo         m_calo        ;  

  /// flag to indicate "light/calibration" mode 
  bool  m_light_mode ;
  // ==========================================================================
};
// ============================================================================
// The END 
// ============================================================================
#endif // GLOBALRECO_NEUTRALPROTOPALG_H
// ============================================================================

