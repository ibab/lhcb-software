#ifndef HLTNAMES_HLTNAMES_H
#define HLTNAMES_HLTNAMES_H 1 
#include "Event/HltEnums.h" 
namespace HltNames {
	 inline int particleInfoID(const std::string& e){
		 if (e == "HltParticleInfoUnknown") return LHCb::HltEnums::HltParticleInfoUnknown;
		 if (e == "RZVeloKey") return LHCb::HltEnums::RZVeloKey;
		 if (e == "VeloKey") return LHCb::HltEnums::VeloKey;
		 if (e == "VeloTTKey") return LHCb::HltEnums::VeloTTKey;
		 if (e == "ForwardKey") return LHCb::HltEnums::ForwardKey;
		 if (e == "MuonKey") return LHCb::HltEnums::MuonKey;
		 if (e == "CaloKey") return LHCb::HltEnums::CaloKey;
		 if (e == "PVKey") return LHCb::HltEnums::PVKey;
		 if (e == "CaloEt") return LHCb::HltEnums::CaloEt;
		 if (e == "rIP") return LHCb::HltEnums::rIP;
		 if (e == "IP") return LHCb::HltEnums::IP;
		 if (e == "IPS") return LHCb::HltEnums::IPS;
		 if (e == "PT") return LHCb::HltEnums::PT;
		 if (e == "Mass") return LHCb::HltEnums::Mass;
		 if (e == "DOCA") return LHCb::HltEnums::DOCA;
		 if (e == "VertexChi2") return LHCb::HltEnums::VertexChi2;
		 if (e == "VertexD") return LHCb::HltEnums::VertexD;
		 if (e == "VertexDS") return LHCb::HltEnums::VertexDS;
		 if (e == "VertexRho") return LHCb::HltEnums::VertexRho;
		 if (e == "VertexDz") return LHCb::HltEnums::VertexDz;
		 if (e == "VertexPointing") return LHCb::HltEnums::VertexPointing;
		 if (e == "Muon2DChi2") return LHCb::HltEnums::Muon2DChi2;
		 if (e == "Muon3DChi2") return LHCb::HltEnums::Muon3DChi2;
		 if (e == "Calo2DChi2") return LHCb::HltEnums::Calo2DChi2;
		 if (e == "Calo3DChi2") return LHCb::HltEnums::Calo3DChi2;
		 return -1; 
	}
	 inline std::string particleInfoStr(int e){
		 switch(e) { 
		 case LHCb::HltEnums::HltParticleInfoUnknown : return "HltParticleInfoUnknown";
		 case LHCb::HltEnums::RZVeloKey : return "RZVeloKey";
		 case LHCb::HltEnums::VeloKey : return "VeloKey";
		 case LHCb::HltEnums::VeloTTKey : return "VeloTTKey";
		 case LHCb::HltEnums::ForwardKey : return "ForwardKey";
		 case LHCb::HltEnums::MuonKey : return "MuonKey";
		 case LHCb::HltEnums::CaloKey : return "CaloKey";
		 case LHCb::HltEnums::PVKey : return "PVKey";
		 case LHCb::HltEnums::CaloEt : return "CaloEt";
		 case LHCb::HltEnums::rIP : return "rIP";
		 case LHCb::HltEnums::IP : return "IP";
		 case LHCb::HltEnums::IPS : return "IPS";
		 case LHCb::HltEnums::PT : return "PT";
		 case LHCb::HltEnums::Mass : return "Mass";
		 case LHCb::HltEnums::DOCA : return "DOCA";
		 case LHCb::HltEnums::VertexChi2 : return "VertexChi2";
		 case LHCb::HltEnums::VertexD : return "VertexD";
		 case LHCb::HltEnums::VertexDS : return "VertexDS";
		 case LHCb::HltEnums::VertexRho : return "VertexRho";
		 case LHCb::HltEnums::VertexDz : return "VertexDz";
		 case LHCb::HltEnums::VertexPointing : return "VertexPointing";
		 case LHCb::HltEnums::Muon2DChi2 : return "Muon2DChi2";
		 case LHCb::HltEnums::Muon3DChi2 : return "Muon3DChi2";
		 case LHCb::HltEnums::Calo2DChi2 : return "Calo2DChi2";
		 case LHCb::HltEnums::Calo3DChi2 : return "Calo3DChi2";
		 default: return "particleInfoUnknown";
		}
	}
	 inline int alleyID(const std::string& e){
		 if (e == "AlleyUnknown") return LHCb::HltEnums::AlleyUnknown;
		 if (e == "Mu") return LHCb::HltEnums::Mu;
		 if (e == "MuHad") return LHCb::HltEnums::MuHad;
		 if (e == "Had") return LHCb::HltEnums::Had;
		 if (e == "Cal") return LHCb::HltEnums::Cal;
		 return -1; 
	}
	 inline std::string alleyStr(int e){
		 switch(e) { 
		 case LHCb::HltEnums::AlleyUnknown : return "AlleyUnknown";
		 case LHCb::HltEnums::Mu : return "Mu";
		 case LHCb::HltEnums::MuHad : return "MuHad";
		 case LHCb::HltEnums::Had : return "Had";
		 case LHCb::HltEnums::Cal : return "Cal";
		 default: return "alleyUnknown";
		}
	}
	 inline int quantityID(const std::string& e){
		 if (e == "HltQuantityUnknown") return LHCb::HltEnums::HltQuantityUnknown;
		 if (e == "BoxDecision") return LHCb::HltEnums::BoxDecision;
		 if (e == "NInputTracks") return LHCb::HltEnums::NInputTracks;
		 if (e == "NInputTracks1") return LHCb::HltEnums::NInputTracks1;
		 if (e == "NPVertices") return LHCb::HltEnums::NPVertices;
		 if (e == "NInputVertices") return LHCb::HltEnums::NInputVertices;
		 if (e == "NOutputTracks") return LHCb::HltEnums::NOutputTracks;
		 if (e == "NOutputTrack2") return LHCb::HltEnums::NOutputTrack2;
		 if (e == "NOutputVertices") return LHCb::HltEnums::NOutputVertices;
		 return -1; 
	}
	 inline std::string quantityStr(int e){
		 switch(e) { 
		 case LHCb::HltEnums::HltQuantityUnknown : return "HltQuantityUnknown";
		 case LHCb::HltEnums::BoxDecision : return "BoxDecision";
		 case LHCb::HltEnums::NInputTracks : return "NInputTracks";
		 case LHCb::HltEnums::NInputTracks1 : return "NInputTracks1";
		 case LHCb::HltEnums::NPVertices : return "NPVertices";
		 case LHCb::HltEnums::NInputVertices : return "NInputVertices";
		 case LHCb::HltEnums::NOutputTracks : return "NOutputTracks";
		 case LHCb::HltEnums::NOutputTrack2 : return "NOutputTrack2";
		 case LHCb::HltEnums::NOutputVertices : return "NOutputVertices";
		 default: return "quantityUnknown";
		}
	}
	 inline int decisionTypeID(const std::string& e){
		 if (e == "Decision") return LHCb::HltEnums::Decision;
		 if (e == "Forced") return LHCb::HltEnums::Forced;
		 if (e == "Muon") return LHCb::HltEnums::Muon;
		 if (e == "DiMuon") return LHCb::HltEnums::DiMuon;
		 if (e == "MuonHadron") return LHCb::HltEnums::MuonHadron;
		 if (e == "Hadron") return LHCb::HltEnums::Hadron;
		 if (e == "DiHadron") return LHCb::HltEnums::DiHadron;
		 if (e == "Electron") return LHCb::HltEnums::Electron;
		 if (e == "DiElectron") return LHCb::HltEnums::DiElectron;
		 if (e == "Gamma") return LHCb::HltEnums::Gamma;
		 return -1; 
	}
	 inline std::string decisionTypeStr(int e){
		 switch(e) { 
		 case LHCb::HltEnums::Decision : return "Decision";
		 case LHCb::HltEnums::Forced : return "Forced";
		 case LHCb::HltEnums::Muon : return "Muon";
		 case LHCb::HltEnums::DiMuon : return "DiMuon";
		 case LHCb::HltEnums::MuonHadron : return "MuonHadron";
		 case LHCb::HltEnums::Hadron : return "Hadron";
		 case LHCb::HltEnums::DiHadron : return "DiHadron";
		 case LHCb::HltEnums::Electron : return "Electron";
		 case LHCb::HltEnums::DiElectron : return "DiElectron";
		 case LHCb::HltEnums::Gamma : return "Gamma";
		 default: return "decisionTypeUnknown";
		}
	}
	 inline int boxID(const std::string& e){
		 if (e == "HltBoxUnknown") return LHCb::HltEnums::HltBoxUnknown;
		 if (e == "MuonEntry") return LHCb::HltEnums::MuonEntry;
		 if (e == "MuonL0Conf") return LHCb::HltEnums::MuonL0Conf;
		 if (e == "MuonPreTrigger") return LHCb::HltEnums::MuonPreTrigger;
		 if (e == "MuonTrigger") return LHCb::HltEnums::MuonTrigger;
		 if (e == "MuonHadEntry") return LHCb::HltEnums::MuonHadEntry;
		 if (e == "MuonHadL0Conf") return LHCb::HltEnums::MuonHadL0Conf;
		 if (e == "MuonHadPreTrigger") return LHCb::HltEnums::MuonHadPreTrigger;
		 if (e == "MuonHadTrigger") return LHCb::HltEnums::MuonHadTrigger;
		 if (e == "HadEntry") return LHCb::HltEnums::HadEntry;
		 if (e == "HadL0Conf") return LHCb::HltEnums::HadL0Conf;
		 if (e == "HadPreTrigger") return LHCb::HltEnums::HadPreTrigger;
		 if (e == "HadTrigger") return LHCb::HltEnums::HadTrigger;
		 if (e == "CalEntry") return LHCb::HltEnums::CalEntry;
		 if (e == "CalL0Conf") return LHCb::HltEnums::CalL0Conf;
		 if (e == "CalPreTrigger") return LHCb::HltEnums::CalPreTrigger;
		 if (e == "CalTrigger") return LHCb::HltEnums::CalTrigger;
		 return -1; 
	}
	 inline std::string boxStr(int e){
		 switch(e) { 
		 case LHCb::HltEnums::HltBoxUnknown : return "HltBoxUnknown";
		 case LHCb::HltEnums::MuonEntry : return "MuonEntry";
		 case LHCb::HltEnums::MuonL0Conf : return "MuonL0Conf";
		 case LHCb::HltEnums::MuonPreTrigger : return "MuonPreTrigger";
		 case LHCb::HltEnums::MuonTrigger : return "MuonTrigger";
		 case LHCb::HltEnums::MuonHadEntry : return "MuonHadEntry";
		 case LHCb::HltEnums::MuonHadL0Conf : return "MuonHadL0Conf";
		 case LHCb::HltEnums::MuonHadPreTrigger : return "MuonHadPreTrigger";
		 case LHCb::HltEnums::MuonHadTrigger : return "MuonHadTrigger";
		 case LHCb::HltEnums::HadEntry : return "HadEntry";
		 case LHCb::HltEnums::HadL0Conf : return "HadL0Conf";
		 case LHCb::HltEnums::HadPreTrigger : return "HadPreTrigger";
		 case LHCb::HltEnums::HadTrigger : return "HadTrigger";
		 case LHCb::HltEnums::CalEntry : return "CalEntry";
		 case LHCb::HltEnums::CalL0Conf : return "CalL0Conf";
		 case LHCb::HltEnums::CalPreTrigger : return "CalPreTrigger";
		 case LHCb::HltEnums::CalTrigger : return "CalTrigger";
		 default: return "boxUnknown";
		}
	}
};
#endif
