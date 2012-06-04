// $Id: $
//#ifndef MUONVARS_H
//#define MUONVARS_H 1

#include "MuonVars.h"

void fill_muon_vars(Tuples::Tuple tuple_stdpfjets ,LHCb::Particle::Range stdloosemuons, ITriggerTisTos * m_L0TriggerTisTosTool, ITriggerTisTos * m_TriggerTisTosTool){  

	LoKi::Types::Fun ECAL_E = LoKi::Cuts::PPINFO(332,-10.);
	LoKi::Types::Fun HCAL_E = LoKi::Cuts::PPINFO(333,-10.);

	tuple_stdpfjets->farray("stdloosemuon_px",PX,
			"stdloosemuon_py",PY,
			"stdloosemuon_pz",PZ,
			"stdloosemuon_e",E,stdloosemuons.begin(),stdloosemuons.end(),"NMaxMuons",10000);
	tuple_stdpfjets->farray("stdloosemuon_pdgid",ID,
			"stdloosemuon_bpvvz",BPV(VZ),
			"stdloosemuon_PERR2",PERR2,
			"stdloosemuon_chi2dof",TRCHI2DOF,stdloosemuons.begin(),stdloosemuons.end(),"NMaxMuons",10000);

	tuple_stdpfjets->farray("stdloosemuon_TrPROBCHI2",TRFUN(TrPROBCHI2),stdloosemuons.begin(),stdloosemuons.end(),"NMaxMuons",10000);
	tuple_stdpfjets->farray("stdloosemuon_ECAL_E",ECAL_E,stdloosemuons.begin(),stdloosemuons.end(),"NMaxMuons",10000);
	tuple_stdpfjets->farray("stdloosemuon_HCAL_E",HCAL_E,stdloosemuons.begin(),stdloosemuons.end(),"NMaxMuons",10000);

	std::vector<std::string> triggerlist,l0triggerlist;
	triggerlist.push_back("Hlt1SingleMuonHighPTDecision");
	triggerlist.push_back("Hlt2SingleMuonHighPTDecision");
	
	l0triggerlist.push_back("L0MuonDecision");

	for (LHCb::Particle::Range::iterator imuon = stdloosemuons.begin() ; imuon != stdloosemuons.end() ; ++imuon) {
		const LHCb::Particle * muon = *imuon;
		m_TriggerTisTosTool->setOfflineInput(*muon);
		for( std::vector< std::string >::const_iterator s=triggerlist.begin();s!= triggerlist.end();++s){
			m_TriggerTisTosTool->setTriggerInput(*s);
			ITriggerTisTos::TisTosDecision classifiedDec;
			//classifiedDec = m_TriggerTisTosTool->triggerTisTos();
			bool dec, tis, tos;
			m_TriggerTisTosTool->triggerTisTos(*muon, *s, dec, tis, tos);
			tuple_stdpfjets->column("stdloosemuon_"+*s+"_Dec",dec);
			tuple_stdpfjets->column("stdloosemuon_"+*s+"_TOS",tos);
			tuple_stdpfjets->column("stdloosemuon_"+*s+"_TIS",tis);
			/*
			   tuple_stdpfjets->column("stdloosemuon_"+*s+"_TOS",classifiedDec.tos());
			   tuple_stdpfjets->column("stdloosemuon_"+*s+"_TIS",classifiedDec.tis());
			   tuple_stdpfjets->column("stdloosemuon_"+*s+"_TOS",classifiedDec.tos());
			 */
		}

		for( std::vector< std::string >::const_iterator s=l0triggerlist.begin();s!= l0triggerlist.end();++s){
			m_L0TriggerTisTosTool->setTriggerInput(*s);
			ITriggerTisTos::TisTosDecision classifiedDec;
			//classifiedDec = m_TriggerTisTosTool->triggerTisTos();
			bool dec, tis, tos;
			m_L0TriggerTisTosTool->triggerTisTos(*muon, *s, dec, tis, tos);
			tuple_stdpfjets->column("stdloosemuon_"+*s+"_Dec",dec);
			tuple_stdpfjets->column("stdloosemuon_"+*s+"_TOS",tos);
			tuple_stdpfjets->column("stdloosemuon_"+*s+"_TIS",tis);
			/*
			   tuple_stdpfjets->column("stdloosemuon_"+*s+"_TOS",classifiedDec.tos());
			   tuple_stdpfjets->column("stdloosemuon_"+*s+"_TIS",classifiedDec.tis());
			   tuple_stdpfjets->column("stdloosemuon_"+*s+"_TOS",classifiedDec.tos());
			 */
		}
	}
}
//#endif /*MUONVARS_H*/ 
