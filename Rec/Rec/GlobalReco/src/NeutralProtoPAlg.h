// $Id: NeutralProtoPAlg.h,v 1.3 2006-06-27 16:48:41 odescham Exp $
#ifndef GLOBALRECO_NEUTRALPROTOPALG_H 
#define GLOBALRECO_NEUTRALPROTOPALG_H 1

// Include files
#include "Relations/IRelationWeighted.h"
// Event
#include "Event/Track.h"
#include "Event/CaloHypo.h"
#include "Event/CaloCluster.h"
#include "Event/ProtoParticle.h"
// Calo
#include "CaloInterfaces/ICaloHypoLikelihood.h"
#include "CaloUtils/ClusterFunctors.h"
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class NeutralProtoPAlg NeutralProtoPAlg.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2006-06-09
 */
class NeutralProtoPAlg : public GaudiAlgorithm {
public: 
  /// Standard constructor
  NeutralProtoPAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~NeutralProtoPAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:// method
  double caloTrMatch   ( const LHCb::CaloHypo* hypo  , const LHCb::Calo2Track::IClusTrTable*     table )  const ;
  double clusterMass   ( const LHCb::CaloHypo*  hypo  )  const ;
  double showerShape   ( const LHCb::CaloHypo*  hypo  )  const ;
  double caloDepositID ( const LHCb::CaloHypo*  hypo  )  const ;
  double photonID      ( const LHCb::CaloHypo*  hypo  )  const ;

private:// data
  std::string               m_protoLocation;
  std::vector<std::string>  m_hyposLocations ;
  std::string               m_matchLocation  ;
  std::string               m_spdprsType        ;
  std::string               m_spdprsName        ;
  ICaloHypoLikelihood*      m_spdprs            ;
  std::string               m_photonIDType      ;
  std::string               m_photonIDName      ;
  ICaloHypoLikelihood*      m_photonID          ;
  double                    m_caloTrMatch_bad   ;
  double                    m_caloDepositID_bad ;
  double                    m_showerShape_bad   ;
  double                    m_clusterMass_bad   ;
  double                    m_photonID_bad      ;
  LHCb::ClusterFunctors::ClusterFromCalo         m_calo        ;  
  
  long m_nEvts  ;
  std::map<  std::string, long  > m_counts ;
};
#endif // GLOBALRECO_NEUTRALPROTOPALG_H

