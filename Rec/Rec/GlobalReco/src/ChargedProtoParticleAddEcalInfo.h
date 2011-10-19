//-----------------------------------------------------------------------------
/** @file ChargedProtoParticleAddEcalInfo.h
 *
 * Header file for algorithm ChargedProtoParticleAddEcalInfo
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 29/03/2006
 */
//-----------------------------------------------------------------------------

#ifndef GLOBALRECO_ChargedProtoParticleAddEcalInfo_H
#define GLOBALRECO_ChargedProtoParticleAddEcalInfo_H 1

#include "ChargedProtoParticleCALOBaseAlg.h"
#include "CaloUtils/ICaloElectron.h"

/** @class ChargedProtoParticleAddEcalInfo ChargedProtoParticleAddEcalInfo.h
 *
 *  Updates the ECAL information stored in the ProtoParticles
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date 28/08/2009
 */

class ChargedProtoParticleAddEcalInfo : public ChargedProtoParticleCALOBaseAlg
{

public:

  /// Standard constructor
  ChargedProtoParticleAddEcalInfo( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ChargedProtoParticleAddEcalInfo( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();       ///< Algorithm execution

private:

  /// Load the Calo Ecal tables
  bool getEcalData();

  /// Add Calo Ecal information to the given ProtoParticle
  bool addEcal( LHCb::ProtoParticle * proto ) const;

  /// Access the electron tool
  inline ICaloElectron * electronTool() const { return m_electron; }
  
private:

  std::string m_protoPath; ///< Location of the ProtoParticles in the TES

  std::string m_inEcalPath ;
  std::string m_electronMatchPath ;
  std::string m_clusterMatchPath ;
  std::string m_ecalChi2Path ;
  std::string m_ecalEPath ;
  std::string m_clusterChi2Path ;
  std::string m_ecalPIDePath ;
  std::string m_ecalPIDmuPath ;

  const LHCb::Calo2Track::ITrAccTable*  m_InEcalTable ;
  const LHCb::Calo2Track::ITrHypoTable2D* m_elecTrTable ;
  const LHCb::Calo2Track::IClusTrTable2D* m_clusTrTable ;
  const LHCb::Calo2Track::ITrEvalTable*  m_EcalChi2Table ;
  const LHCb::Calo2Track::ITrEvalTable*  m_EcalETable ;
  const LHCb::Calo2Track::ITrEvalTable*  m_ClusChi2Table ;
  const LHCb::Calo2Track::ITrEvalTable* m_dlleEcalTable ;
  const LHCb::Calo2Track::ITrEvalTable* m_dllmuEcalTable ;

  /// CaloElectron tool
  ICaloElectron * m_electron;

};

#endif // GLOBALRECO_ChargedProtoParticleAddEcalInfo_H
