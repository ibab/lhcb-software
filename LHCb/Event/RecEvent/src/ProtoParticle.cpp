
// STL
#include <algorithm>

// Gaudi
#include "GaudiKernel/GaudiException.h"

// local
#include "Event/ProtoParticle.h"

// boost
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

//-----------------------------------------------------------------------------

// Fillstream method
std::ostream& LHCb::ProtoParticle::fillStream( std::ostream & s ) const
{
  s << "{"
    << " Track "     << this->track()
    << " CaloHypos " << this->calo()
    << " RichPID "   << this->richPID()
    << " MuonPID "   << this->muonPID()
    << " ExtraInfo [";
  for ( const auto & i : extraInfo() )
  {
    const auto info = static_cast<LHCb::ProtoParticle::additionalInfo>(i.first);
    s << " " << info << "=" << i.second;
  }
  return s << " ] }";
}

LHCb::ProtoParticle::ExtraInfo::size_type
LHCb::ProtoParticle::clearCalo( const LHCb::CaloHypo::Hypothesis & hypo )
{
  using namespace boost::lambda;

  // number of hypos removed
  LHCb::ProtoParticle::ExtraInfo::size_type removed = 0;

  // remove NULL hypos
  const auto it = std::remove ( m_calo.begin(), m_calo.end(), 0 );
  if ( m_calo.end() != it )
  {
    removed += m_calo.end() - it;
    m_calo.erase ( it , m_calo.end() );
  }

  // Find hypos to remove
  const auto iHypoRemove =
    std::remove_if ( m_calo.begin(), m_calo.end(),
                     bind( &LHCb::CaloHypo::hypothesis, _1 ) == hypo );
  if ( m_calo.end() != iHypoRemove )
  {
    removed += m_calo.end() - iHypoRemove;
    m_calo.erase ( iHypoRemove , m_calo.end() );
  }

  // return number of hypos removed
  return removed;
}

// Remove all Combined DLL information stored in this ProtoParticle
LHCb::ProtoParticle::ExtraInfo::size_type
LHCb::ProtoParticle::removeCombinedInfo()
{
  LHCb::ProtoParticle::ExtraInfo::size_type erased = 0;
  erased += this->eraseInfo( LHCb::ProtoParticle::CombDLLe  );
  erased += this->eraseInfo( LHCb::ProtoParticle::CombDLLmu );
  erased += this->eraseInfo( LHCb::ProtoParticle::CombDLLpi );
  erased += this->eraseInfo( LHCb::ProtoParticle::CombDLLk  );
  erased += this->eraseInfo( LHCb::ProtoParticle::CombDLLp  );
  erased += this->eraseInfo( LHCb::ProtoParticle::NoPID     );
  erased += this->eraseInfo( LHCb::ProtoParticle::CombDLLd  );
  return erased;
}

// Remove all RICH information stored in this ProtoParticle
LHCb::ProtoParticle::ExtraInfo::size_type
LHCb::ProtoParticle::removeRichInfo()
{
  LHCb::ProtoParticle::ExtraInfo::size_type erased = 0;
  erased += this->eraseInfo( LHCb::ProtoParticle::RichDLLe );
  erased += this->eraseInfo( LHCb::ProtoParticle::RichDLLmu );
  erased += this->eraseInfo( LHCb::ProtoParticle::RichDLLpi );
  erased += this->eraseInfo( LHCb::ProtoParticle::RichDLLk );
  erased += this->eraseInfo( LHCb::ProtoParticle::RichDLLp );
  erased += this->eraseInfo( LHCb::ProtoParticle::RichDLLbt );
  erased += this->eraseInfo( LHCb::ProtoParticle::RichDLLd );
  erased += this->eraseInfo( LHCb::ProtoParticle::RichPIDStatus );
  // Set RichPID pointer to NULL
  this->setRichPID(NULL);
  // Invalidate Combined DLL information since information has changed
  erased += this->removeCombinedInfo();
  return erased;
}

// Remove all MUON information stored in this ProtoParticle
LHCb::ProtoParticle::ExtraInfo::size_type
LHCb::ProtoParticle::removeMuonInfo()
{
  LHCb::ProtoParticle::ExtraInfo::size_type erased = 0;
  erased += this->eraseInfo( LHCb::ProtoParticle::InAccMuon );
  erased += this->eraseInfo( LHCb::ProtoParticle::MuonMuLL );
  erased += this->eraseInfo( LHCb::ProtoParticle::MuonBkgLL );
  erased += this->eraseInfo( LHCb::ProtoParticle::MuonNShared );
  erased += this->eraseInfo( LHCb::ProtoParticle::MuonPIDStatus );
  // Set MuonPID pointer to NULL
  this->setMuonPID(NULL);
  // Invalidate Combined DLL information since information has changed
  erased += this->removeCombinedInfo();
  return erased;
}

// Remove all CALO-ECAL information stored in this ProtoParticle
LHCb::ProtoParticle::ExtraInfo::size_type
LHCb::ProtoParticle::removeCaloEcalInfo()
{
  LHCb::ProtoParticle::ExtraInfo::size_type erased = 0;
  erased += this->eraseInfo( LHCb::ProtoParticle::InAccEcal );
  erased += this->eraseInfo( LHCb::ProtoParticle::CaloTrajectoryL );
  erased += this->eraseInfo( LHCb::ProtoParticle::CaloChargedSpd );
  erased += this->eraseInfo( LHCb::ProtoParticle::CaloChargedPrs );
  erased += this->eraseInfo( LHCb::ProtoParticle::CaloChargedEcal );
  erased += this->eraseInfo( LHCb::ProtoParticle::CaloElectronMatch );
  erased += this->eraseInfo( LHCb::ProtoParticle::CaloTrMatch );
  erased += this->eraseInfo( LHCb::ProtoParticle::CaloEcalE );
  erased += this->eraseInfo( LHCb::ProtoParticle::CaloEcalChi2 );
  erased += this->eraseInfo( LHCb::ProtoParticle::CaloClusChi2 );
  erased += this->eraseInfo( LHCb::ProtoParticle::EcalPIDe );
  erased += this->eraseInfo( LHCb::ProtoParticle::EcalPIDmu );
  // removed associated CaloHypos
  erased += this->clearCalo( LHCb::CaloHypo::EmCharged );
  // Invalidate Combined DLL information since information has changed
  erased += this->removeCombinedInfo();
  return erased;
}

// Remove all CALO-BREM information stored in this ProtoParticle
LHCb::ProtoParticle::ExtraInfo::size_type
LHCb::ProtoParticle::removeCaloBremInfo()
{
  LHCb::ProtoParticle::ExtraInfo::size_type erased = 0;
  erased += this->eraseInfo( LHCb::ProtoParticle::InAccBrem );
  erased += this->eraseInfo( LHCb::ProtoParticle::CaloNeutralSpd );
  erased += this->eraseInfo( LHCb::ProtoParticle::CaloNeutralPrs );
  erased += this->eraseInfo( LHCb::ProtoParticle::CaloNeutralEcal );
  erased += this->eraseInfo( LHCb::ProtoParticle::CaloBremMatch );
  // remove associated Hypos
  erased += this->clearCalo( LHCb::CaloHypo::Photon );
  // Invalidate Combined DLL information since information has changed
  erased += this->removeCombinedInfo();
  return erased;
}

// Remove all CALO-SPD information stored in this ProtoParticle
LHCb::ProtoParticle::ExtraInfo::size_type
LHCb::ProtoParticle::removeCaloSpdInfo()
{
  LHCb::ProtoParticle::ExtraInfo::size_type erased = 0;
  erased += this->eraseInfo(LHCb::ProtoParticle::InAccSpd);
  erased += this->eraseInfo(LHCb::ProtoParticle::CaloSpdE);
  // Invalidate Combined DLL information since information has changed
  erased += this->removeCombinedInfo();
  return erased;
}

// Remove all CALO-PRS information stored in this ProtoParticle
LHCb::ProtoParticle::ExtraInfo::size_type
LHCb::ProtoParticle::removeCaloPrsInfo()
{
  LHCb::ProtoParticle::ExtraInfo::size_type erased = 0;
  erased += this->eraseInfo(LHCb::ProtoParticle::InAccPrs);
  erased += this->eraseInfo(LHCb::ProtoParticle::CaloPrsE);
  erased += this->eraseInfo(LHCb::ProtoParticle::PrsPIDe);
  // Invalidate Combined DLL information since information has changed
  erased += this->removeCombinedInfo();
  return erased;
}

// Remove all CALO-HCAL information stored in this ProtoParticle
LHCb::ProtoParticle::ExtraInfo::size_type
LHCb::ProtoParticle::removeCaloHcalInfo()
{
  LHCb::ProtoParticle::ExtraInfo::size_type erased = 0;
  erased += this->eraseInfo(LHCb::ProtoParticle::InAccHcal);
  erased += this->eraseInfo(LHCb::ProtoParticle::CaloHcalE);
  erased += this->eraseInfo(LHCb::ProtoParticle::HcalPIDe);
  erased += this->eraseInfo(LHCb::ProtoParticle::HcalPIDmu);
  // Invalidate Combined DLL information since information has changed
  erased += this->removeCombinedInfo();
  return erased;
}

// Remove all VELO information stored in this ProtoParticle
LHCb::ProtoParticle::ExtraInfo::size_type
LHCb::ProtoParticle::removeVeloInfo()
{
  LHCb::ProtoParticle::ExtraInfo::size_type erased = 0;
  erased += this->eraseInfo(LHCb::ProtoParticle::VeloCharge);
  return erased;
}

LHCb::ProtoParticle::additionalInfo
LHCb::ProtoParticle::convertExtraInfo(const std::string& name)
{
  if ( "NoPID"             == name ) { return LHCb::ProtoParticle::NoPID;     }
  if ( "RichDLLe"          == name ) { return LHCb::ProtoParticle::RichDLLe;  }
  if ( "RichDLLmu"         == name ) { return LHCb::ProtoParticle::RichDLLmu; }
  if ( "RichDLLpi"         == name ) { return LHCb::ProtoParticle::RichDLLpi; }
  if ( "RichDLLk"          == name ) { return LHCb::ProtoParticle::RichDLLk;  }
  if ( "RichDLLp"          == name ) { return LHCb::ProtoParticle::RichDLLp;  }
  if ( "RichPIDStatus"     == name ) { return LHCb::ProtoParticle::RichPIDStatus; }
  if ( "RichDLLbt"         == name ) { return LHCb::ProtoParticle::RichDLLbt; }
  if ( "RichDLLd"          == name ) { return LHCb::ProtoParticle::RichDLLd;  }
  if ( "MuonMuLL"          == name ) { return LHCb::ProtoParticle::MuonMuLL; }
  if ( "MuonBkgLL"         == name ) { return LHCb::ProtoParticle::MuonBkgLL; }
  if ( "MuonNShared"       == name ) { return LHCb::ProtoParticle::MuonNShared; }
  if ( "MuonPIDStatus"     == name ) { return LHCb::ProtoParticle::MuonPIDStatus; }
  if ( "InAccMuon"         == name ) { return LHCb::ProtoParticle::InAccMuon; }
  if ( "InAccSpd"          == name ) { return LHCb::ProtoParticle::InAccSpd; }
  if ( "InAccPrs"          == name ) { return LHCb::ProtoParticle::InAccPrs; }
  if ( "InAccEcal"         == name ) { return LHCb::ProtoParticle::InAccEcal; }
  if ( "InAccHcal"         == name ) { return LHCb::ProtoParticle::InAccHcal; }
  if ( "InAccBrem"         == name ) { return LHCb::ProtoParticle::InAccBrem; }
  if ( "CaloTrMatch"       == name ) { return LHCb::ProtoParticle::CaloTrMatch; }
  if ( "CaloElectronMatch" == name ) { return LHCb::ProtoParticle::CaloElectronMatch; }
  if ( "CaloBremMatch"     == name ) { return LHCb::ProtoParticle::CaloBremMatch; }
  if ( "CaloChargedSpd"    == name ) { return LHCb::ProtoParticle::CaloChargedSpd; }
  if ( "CaloChargedPrs"    == name ) { return LHCb::ProtoParticle::CaloChargedPrs; }
  if ( "CaloChargedEcal"   == name ) { return LHCb::ProtoParticle::CaloChargedEcal; }
  if ( "CaloDepositID"     == name ) { return LHCb::ProtoParticle::CaloDepositID; }
  if ( "ShowerShape"       == name ) { return LHCb::ProtoParticle::ShowerShape; }
  if ( "ClusterMass"       == name ) { return LHCb::ProtoParticle::ClusterMass; }
  if ( "CaloNeutralSpd"    == name ) { return LHCb::ProtoParticle::CaloNeutralSpd; }
  if ( "CaloNeutralPrs"    == name ) { return LHCb::ProtoParticle::CaloNeutralPrs; }
  if ( "CaloNeutralEcal"   == name ) { return LHCb::ProtoParticle::CaloNeutralEcal; }
  if ( "CaloSpdE"          == name ) { return LHCb::ProtoParticle::CaloSpdE; }
  if ( "CaloPrsE"          == name ) { return LHCb::ProtoParticle::CaloPrsE; }
  if ( "CaloEcalE"         == name ) { return LHCb::ProtoParticle::CaloEcalE; }
  if ( "CaloHcalE"         == name ) { return LHCb::ProtoParticle::CaloHcalE; }
  if ( "CaloEcalChi2"      == name ) { return LHCb::ProtoParticle::CaloEcalChi2; }
  if ( "CaloBremChi2"      == name ) { return LHCb::ProtoParticle::CaloBremChi2; }
  if ( "CaloClusChi2"      == name ) { return LHCb::ProtoParticle::CaloClusChi2; }
  if ( "CaloTrajectoryL"   == name ) { return LHCb::ProtoParticle::CaloTrajectoryL; }
  if ( "EcalPIDe"          == name ) { return LHCb::ProtoParticle::EcalPIDe; }
  if ( "PrsPIDe"           == name ) { return LHCb::ProtoParticle::PrsPIDe; }
  if ( "BremPIDe"          == name ) { return LHCb::ProtoParticle::BremPIDe; }
  if ( "HcalPIDe"          == name ) { return LHCb::ProtoParticle::HcalPIDe; }
  if ( "HcalPIDmu"         == name ) { return LHCb::ProtoParticle::HcalPIDmu; }
  if ( "EcalPIDmu"         == name ) { return LHCb::ProtoParticle::EcalPIDmu; }
  if ( "PhotonID"          == name ) { return LHCb::ProtoParticle::PhotonID; }
  if ( "VeloCharge"        == name ) { return LHCb::ProtoParticle::VeloCharge; }
  if ( "TrackChi2PerDof"   == name ) { return LHCb::ProtoParticle::TrackChi2PerDof; }
  if ( "TrackNumDof"       == name ) { return LHCb::ProtoParticle::TrackNumDof; }
  if ( "TrackType"         == name ) { return LHCb::ProtoParticle::TrackType; }
  if ( "TrackHistory"      == name ) { return LHCb::ProtoParticle::TrackHistory; }
  if ( "TrackP"            == name ) { return LHCb::ProtoParticle::TrackP; }
  if ( "TrackPt"           == name ) { return LHCb::ProtoParticle::TrackPt; }
  if ( "CombDLLe"          == name ) { return LHCb::ProtoParticle::CombDLLe; }
  if ( "CombDLLmu"         == name ) { return LHCb::ProtoParticle::CombDLLmu; }
  if ( "CombDLLpi"         == name ) { return LHCb::ProtoParticle::CombDLLpi; }
  if ( "CombDLLk"          == name ) { return LHCb::ProtoParticle::CombDLLk; }
  if ( "CombDLLp"          == name ) { return LHCb::ProtoParticle::CombDLLp; }
  if ( "ProbNNe"           == name ) { return LHCb::ProtoParticle::ProbNNe; }
  if ( "ProbNNmu"          == name ) { return LHCb::ProtoParticle::ProbNNmu; }
  if ( "ProbNNpi"          == name ) { return LHCb::ProtoParticle::ProbNNpi; }
  if ( "ProbNNk"           == name ) { return LHCb::ProtoParticle::ProbNNk; }
  if ( "ProbNNp"           == name ) { return LHCb::ProtoParticle::ProbNNp; }
  if ( "LastGlobal"        == name ) { return LHCb::ProtoParticle::LastGlobal; }

  // if get here something bad happened
  throw GaudiException ( "Unknown enum name "+name,
                         "*LHCb::ProtoParticle*", StatusCode::FAILURE );
  return (LHCb::ProtoParticle::additionalInfo)0; // value unimportant ...
}
