// $Id: ProtoParticle.cpp,v 1.9 2009-11-20 16:36:37 jonrob Exp $

#include <algorithm>

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
    << " CaloHypos " << this->calo()
    << " RichPID "   << this->richPID()
    << " MuonPID "   << this->muonPID()
    << " ExtraInfo [";
  for ( ExtraInfo::const_iterator i = this->extraInfo().begin();
        i != this->extraInfo().end(); ++i )
  {
    const LHCb::ProtoParticle::additionalInfo info =
      static_cast<LHCb::ProtoParticle::additionalInfo>(i->first);
    s << " " << info << "=" << i->second;
  }
  return s << " ] }";
}

LHCb::ProtoParticle::ExtraInfo::size_type 
LHCb::ProtoParticle::clearCalo( const LHCb::CaloHypo::Hypothesis & hypo )
{
  using namespace boost::lambda;

  // number of hypos removed
  LHCb::ProtoParticle::ExtraInfo::size_type removed = 0;

  // Find hypos to remove
  const SmartRefVector<LHCb::CaloHypo>::iterator iHypoRemove =
    std::remove_if ( m_calo.begin(), m_calo.end(), 
                     bind( &LHCb::CaloHypo::hypothesis, _1 ) == hypo );
  
  // remove selected hypos
  if ( m_calo.end() != iHypoRemove )
  {
    removed = m_calo.end() - iHypoRemove;
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

