// $Id: ProtoParticle.cpp,v 1.6 2009-08-30 22:19:07 jonrob Exp $

// local
#include "Event/ProtoParticle.h"

//-----------------------------------------------------------------------------

// fillstream method
std::ostream& LHCb::ProtoParticle::fillStream(std::ostream& s) const
{
  s << "{ " << "extraInfo :";
  for ( ExtraInfo::const_iterator i = extraInfo().begin();
        i != extraInfo().end(); ++i )
  {
    const LHCb::ProtoParticle::additionalInfo info = 
      static_cast<LHCb::ProtoParticle::additionalInfo>(i->first);
    s << " " << info << "=" << i->second;
  }
  s << std::endl << " }";
  return s;
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
  this->removeCombinedInfo();
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
  this->removeCombinedInfo();
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
  this->clearCalo();
  // Invalidate Combined DLL information since information has changed
  this->removeCombinedInfo();
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
  this->clearCalo();
  // Invalidate Combined DLL information since information has changed
  this->removeCombinedInfo();
  return erased;
}

// Remove all CALO-SPD information stored in this ProtoParticle
LHCb::ProtoParticle::ExtraInfo::size_type 
LHCb::ProtoParticle::removeCaloSpdInfo()
{
  LHCb::ProtoParticle::ExtraInfo::size_type erased = 0;
  erased += this->eraseInfo(LHCb::ProtoParticle::InAccSpd);
  erased += this->eraseInfo(LHCb::ProtoParticle::CaloSpdE);
  this->clearCalo();
  // Invalidate Combined DLL information since information has changed
  this->removeCombinedInfo();
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
  this->clearCalo();
  // Invalidate Combined DLL information since information has changed
  this->removeCombinedInfo();
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
  this->clearCalo();
  // Invalidate Combined DLL information since information has changed
  this->removeCombinedInfo();
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


