#include "histograms.h"

histograms::histograms(TFile *outputfile, TString dirname, int isMC, int debug) {

  outputfile->cd();
  outputfile->mkdir(dirname);
  outputfile->cd(dirname);
  _outputname = outputfile->GetName();
  _dirname = dirname;
  _histolist = new TList();
  _debug = debug;

  h1_stdjet_pt	= setupHisto("stdjet_pt",50, 0,100000);
  h1_toptosjets_samePV_lmu  = setupHisto("toptosjets_samePV_lmu",10,0,10);
  h1_toptosjets_samePV_lmu_pt  = setupHisto("toptosjets_samePV_lmu_pt",50,0,100000);
  h1_toptosjets_samePV_lmu_DPhi_Z0  = setupHisto("toptosjets_samePV_lmu_DPhi_Z0",50,0,3.2);

  h1_stdjet_ratio_lmupt_pt = setupHisto("stdjet_ratio_lmupt_pt",30, 0,3.);

  h1_stdjet_mtf	= setupHisto("stdjet_mtf",50, 0,1.);
  h1_stdjet_n90	= setupHisto("stdjet_n90",10, -0.01,10);
  h1_stdjet_size	= setupHisto("stdjet_size",20, -0.01,20);

  h1_stdjet_nmax = setupHisto("stdjet_nmax",20, 0,20);

  h1_stdmuon_pt	= setupHisto("stdmuon_pt",50, 0,100000);

  h1_lmuon_pt = setupHisto("lmuon_pt",50,0,100000);
  //h1_lmuon_ratio_pt_cljetpt = setupHisto("lmuon_pt",30,0,3.);

  h1_ljet_pt = setupHisto("ljet_pt",50,0,100000);
  h1_nljet_pt = setupHisto("nljet_pt",50,0,100000);
  h1_lDijet_Mass = setupHisto("lDijet_Mass",50,0,200000);
  h1_lDijet_pt = setupHisto("lDijet_pt",50,0,100000);
  h1_lDijet_DR = setupHisto("lDijet_DR",50,0,5);
  h1_lDijet_DPhi = setupHisto("lDijet_DPhi",50,0,3.2);
  h1_lDijetMu_Mass = setupHisto("lDijetMu_Mass",50,0,200000);
  h1_lDijetMu_pt = setupHisto("lDijetMu_pt",50,0,100000);
  h1_lDijetMu_DR = setupHisto("lDijetMu_DR",50,0,5);
  h1_lDijetMu_DPhi = setupHisto("lDijetMu_DPhi",50,0,3.2);

  h1_Dijet_Mass = setupHisto("Dijet_Mass", 50, 0, 200000);
  h1_Dijet_pt = setupHisto("Dijet_pt", 50, 0, 100000);
  h1_DijetMu_Mass = setupHisto("DijetMu_Mass", 50, 0, 200000);
  h1_DijetMu_pt = setupHisto("DijetMu_pt", 50, 0, 100000);

  h1_fatjet_Mass = setupHisto("Fatjet_Mass", 50, 0, 200000);
  h1_fatjet_Pt = setupHisto("Fatjet_Pt", 50, 0, 200000);

  h2_lDijet_Mass_ljetnljet_ratio = setupHisto("lDijet_Mass_ljetnljet_ratio",50,0,200000,50,0.99,1.01);

  h1_lDiMuon_Mass = setupHisto("DiMuon_Mass",60,60000,120000);
  h1_lDiMuon_Pt = setupHisto("DiMuon_Pt",50,0,100000);

  h1_lDiMuon_DPhi = setupHisto("DiMuon_DPhi",50,0,3.2);
  h1_lDiMuon_DR = setupHisto("DiMuon_DR",50,0,5);

} //histograms::histograms

void histograms::FillHistos(TES  *vars) {
	if(_debug>4) cout << "Filling Histos" << endl;
	if(_debug>4) cout << "Jet Size " << vars->stdjets.size() << endl;
	if(_debug>4) cout << "Muon Size " << vars->stdmuons.size() << endl;

	// ===================================================
	// Jet Information
	// ===================================================

	for(ParticlesVector::iterator ip = vars->stdjets.begin() ; ip != vars->stdjets.end() ; ip++){
		Particle * p = *ip;
		h1_stdjet_pt->Fill(p->Pt());
		h1_stdjet_n90->Fill(p->n90);
		h1_stdjet_mtf->Fill(p->mtf);
		if(_debug>4)cout << "Filling jet w Pt " << p->Pt() << endl;
		h1_stdjet_ratio_lmupt_pt->Fill( vars->stdmuons.size() ? vars->stdmuons[0]->Pt() / p->Pt() : 0 );
	}

	h1_stdjet_size->Fill(vars->stdjets.size());

	// ===================================================
	// Muons Information
	// ===================================================

	if(_debug>4) cout << "StdLooseMuons Loop"<< endl;

	for(ParticlesVector::iterator imu = vars->stdmuons.begin() ; imu != vars->stdmuons.end() ; imu++){
		Particle * mu = *imu;
		h1_stdmuon_pt->Fill(mu->Pt());
		if(_debug>4)cout << "Muon Pt Decrescent " << mu->Pt()/1000. << " GeV/c " << endl;
	}

	// ===================================================
	// DiJet and DiJetMu Information (Combinatorial)
	// ===================================================
        /*
	if(_debug>4) cout << "DiJet_samePV Loop"<< endl;

	for(int i = 0 ; i<vars->stdjets.size() ; i++){
		Particle &jet1 =  * (vars->stdjets[i]);
		for(int j = i+1 ; j<vars->stdjets.size() ; j++){
			Particle &jet2 = *(vars->stdjets[j]);
			if(jet1.bpvz == jet2.bpvz){
				Particle dijet(jet1.momentum + jet2.momentum);

				h1_Dijet_Mass->Fill(dijet.M());
				h1_Dijet_pt->Fill(dijet.Pt());

				for (ParticlesVector::iterator imu = vars->stdmuons.begin() ; imu != vars->stdmuons.end() ; imu++){

					Particle *mu = *imu;
					if(mu->bpvz == jet1.bpvz){

						Particle dijet_mu(jet1.momentum + jet2.momentum + mu->momentum);
						h1_DijetMu_Mass->Fill(dijet_mu.M());
						h1_DijetMu_pt->Fill(dijet_mu.Pt());

					}
				}
			}
		}
	}
        */
	if(_debug>4) cout << "leading fill"<< endl;

	if(vars->stdmuons.size()!=0){
		h1_lmuon_pt->Fill(vars->stdmuons[0]->Pt());
	}

	if(_debug>4) cout << "leading ljet and nljet fill"<< endl;
	if(vars->stdjets.size())h1_ljet_pt->Fill(vars->stdjets[0]->Pt());
	if(vars->stdjets.size() > 1)h1_nljet_pt->Fill(vars->stdjets[1]->Pt());

	if(_debug>4) cout << "leading lDiJet fill"<< endl;

	//if(vars->stdmuons.size())if(Abs(vars->stdjets[0]->bpvz - vars->stdmuons[0]->bpvz) < 1e-6){
	Particle *lbjet = 0; // Adhoc here
	if(vars->stdmuons.size()){
		Particle *lmu = vars->stdmuons[0];
		Particle *ljet = 0;
		Particle *nljet = 0;
		int toptosjets =0;
	
		for(int w = 0 ; w < vars->stdjets.size() ; w++){
			if(Abs(lmu->bpvz - vars->stdjets[w]->bpvz) < 1e-6) if(int (vars->stdjets[w]->Ntustostriggers) % 100 > 0){
				toptosjets++;
				lbjet = vars->stdjets[w];
				h1_toptosjets_samePV_lmu_pt->Fill(lbjet->Pt());
			};
		}

		h1_toptosjets_samePV_lmu->Fill(toptosjets);

		int i = 0;
			for(; i < vars->stdjets.size() ; i++)if(Abs(lmu->bpvz - vars->stdjets[i]->bpvz) < 1e-6){ljet = vars->stdjets[i]; break;}

		for(int j = i+1 ; j < vars->stdjets.size() ; j++)if(Abs(lmu->bpvz - vars->stdjets[j]->bpvz) < 1e-6){nljet = vars->stdjets[j]; break;}

		if(ljet != 0 && nljet != 0){
			Particle ldijet(ljet->momentum + nljet->momentum);
			h1_lDijet_DPhi->Fill(ljet->momentum.DeltaPhi(nljet->momentum));
			h1_lDijet_DR->Fill(ljet->momentum.DeltaR(nljet->momentum));
			h1_lDijet_Mass->Fill(ldijet.M());
			//h1_lDijet_Mass->Fill(vars->lDiJet->M());
			h1_lDijet_pt->Fill(ldijet.Pt());

			Particle ldijet_lmu(ldijet.momentum + vars->stdmuons[0]->momentum);

			h1_lDijetMu_DPhi->Fill(ldijet.momentum.DeltaPhi(lmu->momentum));
			h1_lDijetMu_DR->Fill(ldijet.momentum.DeltaR(lmu->momentum));

			h1_lDijetMu_Mass->Fill(ldijet_lmu.M());
			h1_lDijetMu_pt->Fill(ldijet_lmu.Pt());

		}
	}

	if(_debug>4) cout << "Filling NMax jets" << endl;
	h1_stdjet_nmax->Fill(vars->stdjets.size());

	if(_debug>4) cout << "Filling DiMuon" << endl;

	if(vars->stdmuons.size() > 1){
		Particle *lmu = vars->stdmuons[0];
		Particle *nlmu = 0;
		for(int i =1 ; i < vars->stdmuons.size() ; i++){if(lmu->pdgid * vars->stdmuons[i]->pdgid < 0){nlmu = vars->stdmuons[i]; break;}}
		//Diferent signs
		if(nlmu != 0){

		if(Abs(lmu->bpvz - nlmu->bpvz) < 1e-6){
			Particle ldimuon(lmu->momentum + nlmu->momentum);
			h1_lDiMuon_Mass->Fill(ldimuon.M());
			h1_lDiMuon_Pt->Fill(ldimuon.Pt());
			h1_lDiMuon_DPhi->Fill(lmu->momentum.DeltaPhi(nlmu->momentum)) ;
			h1_lDiMuon_DR->Fill(lmu->momentum.DeltaR(nlmu->momentum)) ;
			if(lbjet!=0)h1_toptosjets_samePV_lmu_DPhi_Z0->Fill(Abs(lbjet->momentum.DeltaPhi(ldimuon.momentum)));
		}
	}

		if(vars->fatjets.size() > 0.5){
				Particle * lfatjet = 0;
				for(int j = 0 ; j < vars->fatjets.size() ; j++)if(Abs(lmu->bpvz - vars->fatjets[j]->bpvz) < 1e-6){lfatjet = vars->fatjets[j]; break;}
				if(lfatjet != 0){
					if(lfatjet->momentum.DeltaR(lmu->momentum) < 1 && lfatjet->Pt() > 100000){
						h1_fatjet_Mass->Fill(lfatjet->M());
						h1_fatjet_Pt->Fill(lfatjet->Pt());
						
					}

				}
		}
	}

//}

	if(_debug>4) cout << "End of filling histos"<< endl;

} //fillHistos
