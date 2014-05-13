#include "Multi.hpp"

using namespace std;


double * ScalarProd(const int ncomp, double v[], double c)
{
	double * res = new double[ncomp];
	for(int n = 0; n < ncomp; n++) res[n] = v[n]*c;

	return res;
}






vector<double> doMultiHistos(TTree *nt, TDirectory *curdir, string options)
{
	double avg_nChPerc = 0;
	double avg_ChPlusPerc = 0; 
	double avg_ChMinusPerc = 0; 

	Double_t  endx,endy,endz,prox,proy,proz;
	int  cptype,cpstype;
	int  trid,trpdg,ndau;
	int  dirparent[100],isinside[100],parent[100];
	int eptype[100],epstype[100],daupdg[100];
	Double_t daukine[100];
	Double_t daucharge[100];

	nt->SetBranchAddress("TrackID",&trid);
	nt->SetBranchAddress("TrackPDG",&trpdg);
	nt->SetBranchAddress("CreatorProcessSubType",&cpstype);
	nt->SetBranchAddress("CreatorProcessType",&cptype);
	nt->SetBranchAddress("Produced_at_x",&prox);
	nt->SetBranchAddress("Produced_at_y",&proy);
	nt->SetBranchAddress("Produced_at_z",&proz);
	nt->SetBranchAddress("Ended_at_x",&endx);
	nt->SetBranchAddress("Ended_at_y",&endy);
	nt->SetBranchAddress("Ended_at_z",&endz);
	nt->SetBranchAddress("NDaugthers",&ndau);
	nt->SetBranchAddress("ParentID",&parent);
	nt->SetBranchAddress("directParent",&dirparent);
	nt->SetBranchAddress("IsInside",&isinside);
	nt->SetBranchAddress("EndingProcessSubType",&epstype);
	nt->SetBranchAddress("EndingProcessType",&eptype);
	nt->SetBranchAddress("DaughterPDG",&daupdg);
	nt->SetBranchAddress("DaughterKinE",&daukine);
	nt->SetBranchAddress("DaughterCharge",&daucharge);

	curdir->cd();

	TH1D* p_endsubproc = new TH1D("endsubproc","p_endsubproc", 100, 0, 200);
	TH1D* in_dau_mult = new TH1D("in_dau_mult","Multi of inelastic daughters", 50, 0, 100);
	in_dau_mult->GetXaxis()->SetTitle("Multi");
	in_dau_mult->GetYaxis()->SetTitle("N evts");
	TH1D* el_dau_mult = new TH1D("el_dau_mult","Multi of elastic daughters", 5, 0, 5);
	TH1D* in_nchdau_mult = new TH1D("in_chdau_mult","Multi of neutral inelastic daughters", 50, 0, 100);
	in_nchdau_mult->GetXaxis()->SetTitle("Multi");
	in_nchdau_mult->GetYaxis()->SetTitle("N evts");
	TH1D* in_dau_gamma = new TH1D("in_dau_gamma","Multi gamma inelastic daughters", 50, 0, 100);
	in_dau_gamma->GetXaxis()->SetTitle("Multi");
	in_dau_gamma->GetYaxis()->SetTitle("N evts");
	TH1D* in_nchdau_nogamma = new TH1D("in_chdau_nogamma","Multi of neutral inelastic daughters but no gamma", 50, 0, 100);
	in_nchdau_nogamma->GetXaxis()->SetTitle("Multi");
	in_nchdau_nogamma->GetYaxis()->SetTitle("N evts");
	TH1D* in_chplusdau_mult = new TH1D("in_chplusdau_mult","Multi of positively charged inelastic daughters", 50, 0, 100);
	in_chplusdau_mult->GetXaxis()->SetTitle("Pos Multi");
	in_chplusdau_mult->GetYaxis()->SetTitle("N evts");
	TH1D* in_chminusdau_mult = new TH1D("in_chminusdau_mult","Multi of negatively charged inelastic daughters", 50, 0, 100);
	in_chminusdau_mult->GetXaxis()->SetTitle("Neg Multi");
	in_chminusdau_mult->GetYaxis()->SetTitle("N evts");
	TH1D* el_chdau_mult = new TH1D("el_chdau_mult","Multi of charged elastic daughters", 5, 0, 5);
	TH1D* pdg_dau = new TH1D("pdg_dau","PDG ID of daughters (only inelastic, no heavy nuclei)", 8000, -4000, 4000);
	TH1D* pdg_dau_high = new TH1D("pdg_dau_high","pdg_dau_high", 150000, -150000, 150000);
	TH1D* dau_gammas_kine = new TH1D("Ekine_gammas","E of gammas", 100, 0., 15);
	dau_gammas_kine->GetXaxis()->SetTitle("E_{kine} (MeV)");
	dau_gammas_kine->GetYaxis()->SetTitle("N evts");
	TH1D* dau_high_kine = new TH1D("Ekine_high","Ekine_high", 100, 0., 1000);
	TH1D* dau_kine = new TH1D("Ekine_daughters","E of daughters", 200, 0., 20000);
	dau_kine->GetXaxis()->SetTitle("E_{kine} (MeV)");
	dau_kine->GetYaxis()->SetTitle("N evts");
	TH1D* end_z = new TH1D("end_z","End z coord", 2000, -1000, 1000);
	TH1D* end_z_inside = new TH1D("end_z_inside","End z coord", 2000, -1000, 1000);
	TH1I* end_proc = new TH1I("end_process","End Process", 4, 0, 4);
	end_proc->GetXaxis()->SetBinLabel(1,"Others");
	end_proc->GetXaxis()->SetBinLabel(2,"Elastic");
	end_proc->GetXaxis()->SetBinLabel(3,"Inelastic");
	end_proc->GetXaxis()->SetBinLabel(4,"Decay");

	TH1D* pdg_dau_named = new TH1D("pdg_dau_named","ID of daughters (only inelastic, no heavy nuclei)", 13, 0, 13);
	pdg_dau_named->GetXaxis()->SetBinLabel(1,"p");
	pdg_dau_named->GetXaxis()->SetBinLabel(2,"#bar{p}");
	pdg_dau_named->GetXaxis()->SetBinLabel(3,"n");
	pdg_dau_named->GetXaxis()->SetBinLabel(4,"K^{+}");
	pdg_dau_named->GetXaxis()->SetBinLabel(5,"K^{-}");
	pdg_dau_named->GetXaxis()->SetBinLabel(6,"#pi^{+}");
	pdg_dau_named->GetXaxis()->SetBinLabel(7,"#pi^{-}");
	pdg_dau_named->GetXaxis()->SetBinLabel(8,"#pi^{0}");
	pdg_dau_named->GetXaxis()->SetBinLabel(9,"#eta");
	pdg_dau_named->GetXaxis()->SetBinLabel(10,"#Lambda");
	pdg_dau_named->GetXaxis()->SetBinLabel(11,"K^{0}_{s}");
	pdg_dau_named->GetXaxis()->SetBinLabel(12,"#gamma");
	pdg_dau_named->GetXaxis()->SetBinLabel(13,"Others");

	TH1D* in_nch_perc = new TH1D("in_nch_perc","Percent of neutral inelastic daughters", 50, 0, 1);
	TH1D* in_chplus_perc = new TH1D("in_chplus_perc","Percent of positively charged inelastic daughters", 50, 0, 1);
	TH1D* in_chminus_perc = new TH1D("in_chminus_perc","Multi of negatively charged inelastic daughters", 50, 0, 1);


	Long_t ntracks = nt->GetEntries();

	int countel = 0;
	int countinel = 0;
	int totP = 0;

	for (int i=0; i < ntracks; i++)
	{
		nt->GetEntry(i);

		if( cptype==-2 )
		{
			totP++;

			int in_daughters = 0;
			int el_daughters = 0;
			int in_nchDaughters = 0;
			int in_dauGamma = 0;
			int in_nchDaughters_nogamma = 0;
			int in_chPlusDaughters = 0;
			int in_chMinusDaughters = 0;
			int el_chDaughters = 0;

			for(int nd = 0; nd < ndau; nd++)     //Loop on daughters
			{
				end_z->Fill(endz);
				if (isinside[nd]==0) continue;
				end_z_inside->Fill(endz);

				if (epstype[nd] == 111)  //Elastic interaction
				{
					el_daughters++; 
					if(daucharge[nd] == 0) el_chDaughters++;
					end_proc->Fill(1);
				}
				else if (epstype[nd] == 121) //Inelastic interaction
				{
					in_daughters++;
					if(daucharge[nd] == 0) { in_nchDaughters++; if(daupdg[nd]!=22) in_nchDaughters_nogamma++;}
					else if(daucharge[nd] > 0) in_chPlusDaughters++;
					else if(daucharge[nd] < 0) in_chMinusDaughters++;
					end_proc->Fill(2);

					if(daupdg[nd]>1000000000)  //Is heavy nucleus?
					{
						pdg_dau_high->Fill(daupdg[nd]-1000000000);
						//dau_kine->Fill(daukine[nd]);
						dau_high_kine->Fill(daukine[nd]);
					}
					else
					{
						pdg_dau->Fill(daupdg[nd]);
						dau_kine->Fill(daukine[nd]);
						if( daupdg[nd]==22 ){ dau_gammas_kine->Fill(daukine[nd]); }

						if(daupdg[nd]==2212) pdg_dau_named->Fill(0);
						else if(daupdg[nd]==-2212) pdg_dau_named->Fill(1);
						else if(daupdg[nd]==2112) pdg_dau_named->Fill(2);
						else if(daupdg[nd]==321) pdg_dau_named->Fill(3);
						else if(daupdg[nd]==-321) pdg_dau_named->Fill(4);
						else if(daupdg[nd]==211) pdg_dau_named->Fill(5);
						else if(daupdg[nd]==-211) pdg_dau_named->Fill(6);
						else if(daupdg[nd]==111) pdg_dau_named->Fill(7);
						else if(daupdg[nd]==221) pdg_dau_named->Fill(8);
						else if(daupdg[nd]==3122) pdg_dau_named->Fill(9);
						else if(daupdg[nd]==310) pdg_dau_named->Fill(10);
						else if(daupdg[nd]==22) { pdg_dau_named->Fill(11); in_dauGamma++; }
						else pdg_dau_named->Fill(12);
					}
				}
				else if(epstype[nd]==201) end_proc->Fill(3);
				else end_proc->Fill(0);

				p_endsubproc->Fill(epstype[nd]);
			}

			if(el_daughters > 0) {el_dau_mult->Fill(el_daughters); countel++;}
			if(el_chDaughters > 0) el_chdau_mult->Fill(el_chDaughters);
			if(in_nchDaughters > 0) in_nchdau_mult->Fill(in_nchDaughters);
			if(in_dauGamma>0) in_dau_gamma->Fill(in_dauGamma);
			if(in_nchDaughters_nogamma > 0) in_nchdau_nogamma->Fill(in_nchDaughters_nogamma);
			if(in_chPlusDaughters > 0) in_chplusdau_mult->Fill(in_chPlusDaughters);
			if(in_chMinusDaughters > 0) in_chminusdau_mult->Fill(in_chMinusDaughters);
			if(in_daughters > 0) 
			{
				countinel++; in_dau_mult->Fill(in_daughters);

				avg_nChPerc += (double) in_nchDaughters / in_daughters;
				avg_ChPlusPerc += (double) in_chPlusDaughters / in_daughters;
				avg_ChMinusPerc += (double) in_chMinusDaughters / in_daughters;

				in_nch_perc->Fill((double) in_nchDaughters / in_daughters);
				in_chplus_perc->Fill((double) in_chPlusDaughters / in_daughters);
				in_chminus_perc->Fill((double) in_chMinusDaughters / in_daughters);
			}
		}
	}

	double Xsecinel = (double) countinel/totP;
	double Xsecel = (double) countel/totP;
	double XsecinelErr = TMath::Sqrt(Xsecinel*(1-Xsecinel)/totP);
	double XsecelErr = TMath::Sqrt(Xsecel*(1-Xsecel)/totP);
	double multi = in_dau_mult->GetMean();
	double multi_err = in_dau_mult->GetRMS()/TMath::Sqrt(in_dau_mult->GetEntries());
	cout << "Tot particles = " << totP << endl;
	cout << fixed << setprecision(4) << "Pint inel = " << Xsecinel << " +/- " << XsecinelErr << endl;
	cout << fixed << setprecision(4) << "Pint el = " << Xsecel << " +/- " << XsecelErr << endl;
	cout << fixed << setprecision(2) << "< Multi > = " << multi << " +/- " << multi_err << endl;



	vector<double> result;
	result.push_back(totP);
	result.push_back(Xsecinel);
	result.push_back(XsecinelErr);
	result.push_back(Xsecel);
	result.push_back(XsecelErr);
	result.push_back(multi);
	result.push_back(multi_err);	
	result.push_back(avg_nChPerc/countinel);
	result.push_back(avg_ChPlusPerc/countinel);
	result.push_back(avg_ChMinusPerc/countinel);
	result.push_back(in_nchdau_mult->GetMean());
	result.push_back(in_nchdau_nogamma->GetMean());
	result.push_back(in_dau_gamma->GetMean());


	curdir->Write();
	p_endsubproc->Write();
	in_dau_mult->Write();
	el_dau_mult->Write();
	in_nchdau_mult->Write();
	in_chplusdau_mult->Write();
	in_chminusdau_mult->Write();
	el_chdau_mult->Write();
	pdg_dau->Write();
	pdg_dau_high->Write();
	dau_gammas_kine->Write();
	dau_high_kine->Write();
	dau_kine->Write();
	end_proc->Write();
	end_z->Write();
	end_z_inside->Write();

/*
	delete p_endsubproc;
	delete in_dau_mult;
	delete el_dau_mult;
	delete in_nchdau_mult;
	delete el_chdau_mult;
	delete pdg_dau;
	delete pdg_dau_high;
	delete dau_gammas_kine;
	delete dau_high_kine;
	delete dau_kine;
*/
	return result;
}














Plotter::Plotter( string inputPath, string outputPath, vector <string> _models, vector < string > pgun, vector < double > _energies, vector < string > material, vector < double > _thicks, bool print ):
	thicks(_thicks), models(_models), energies(_energies)
{
	vector < Material > all_materials;
	Material Al("Al",2.7,26.98); //name, density (g/cm^3), A (g/mol)
	Material Be("Be",1.85,9.012);
	Material Si("Si",2.329,28.085);
	Material Cu("Cu",8.92,63.546);
	Material Pb("Pb",11.34,207.2);
	all_materials.push_back(Be);
	all_materials.push_back(Si);
	all_materials.push_back(Al);
	all_materials.push_back(Cu);
	all_materials.push_back(Pb);

	vector < PGuns > all_pguns;
	PGuns Piplus("Piplus","\\pi^{+}",1,211);
	PGuns Piminus("Piminus","\\pi^{-}",-1,-211);
	PGuns Kplus("Kplus","K^{+}",1,321);
	PGuns Kminus("Kminus","K^{-}",-1,-321);
	PGuns p("p","p",1,2212);
	PGuns pbar("pbar","\\bar{p}",-1,-2212);
	all_pguns.push_back(p);
	all_pguns.push_back(pbar);
	all_pguns.push_back(Piplus);
	all_pguns.push_back(Piminus);
	all_pguns.push_back(Kplus);
	all_pguns.push_back(Kminus);

	for(size_t p = 0; p < pgun.size(); p++)
		for(size_t p2 = 0; p2 < all_pguns.size(); p2++)
			if(all_pguns[p2].GetName()==pgun[p]) pguns.push_back(all_pguns[p2]);

	for(size_t p = 0; p < material.size(); p++)
		for(size_t p2 = 0; p2 < all_materials.size(); p2++)
			if(all_materials[p2].GetName()==material[p]) materials.push_back(all_materials[p2]);



	dataTree = new TTree("dataTree","dataTree");
	int pGun, mat, model;
	double energy, thickness;
	double multi, multi_err;
	double xsec, xsec_err;
	double inel_xsec, inel_xsec_err;
    double el_xsec, el_xsec_err;
    double percPlus, percMinus, percNCh;
	double multiNCh_nogamma, multi_gamma, multiNCh;


	dataTree->Branch("pGun",&pGun,"pGun/I");
	dataTree->Branch("energy",&energy,"energy/D");
	dataTree->Branch("thickness",&thickness,"thickness/D");
	dataTree->Branch("material",&mat,"material/I");
	dataTree->Branch("model",&model,"model/I");
	dataTree->Branch("multi",&multi,"multi/D");
	dataTree->Branch("multi_err",&multi_err,"multi_err/D");
	dataTree->Branch("xsec",&xsec,"xsec/D");
	dataTree->Branch("xsec_err",&xsec_err,"xsec_err/D");
	dataTree->Branch("inel_xsec",&inel_xsec,"inel_xsec/D");
	dataTree->Branch("inel_xsec_err",&inel_xsec_err,"inel_xsec_err/D");
	dataTree->Branch("el_xsec",&el_xsec,"el_xsec/D");
	dataTree->Branch("el_xsec_err",&el_xsec_err,"el_xsec_err/D");
	dataTree->Branch("percPlus",&percPlus,"PercPlus/D");
	dataTree->Branch("percMinus",&percMinus,"percMinus/D");
	dataTree->Branch("multi_gamma",&multi_gamma,"multi_gamma/D");
	dataTree->Branch("percNCh",&percNCh,"percNCh/D");
	dataTree->Branch("multiNCh",&multiNCh,"multiNCh/D");
	dataTree->Branch("multiNCh_nogamma",&multiNCh_nogamma,"multiNCh_nogamma/D");


	for(size_t m = 0; m < models.size(); m++)
	{
		cout << "\n##############################\n  Analysing " << models[m] << " data\n##############################\n " << endl;

		string rootFileName = outputPath+"/TargetsPlots_" + models[m] + ".root";
		TFile * rootout = new TFile(rootFileName.c_str(),"RECREATE");

		ofstream xsec_txtout, multi_txtout;
		xsec_txtout.open((outputPath+"/XsecSummary_"+models[m]+".txt").c_str());
		multi_txtout.open((outputPath+"/MultiSummary_"+models[m]+".txt").c_str());

		string path = inputPath;
		path += "/"+models[m]+"/";

		xsec_txtout << "\\begin{tabular}{ c | c | c | c | c  }" << endl;
		xsec_txtout << "PGun in Material & $N^{inel}/N^{gen}$ ($10^{-3}$) & $\\sigma_{inel}$ (mb) & $N^{el}/N^{gen}$ ($10^{-3}$) & $\\sigma_{el}$ (mb) \\\\ \\hline" << endl;

		multi_txtout << "\\begin{tabular}{ c | c | c }" << endl;
		multi_txtout << "PGun in Material & $< Mult >$ & RMS \\\\ \\hline" << endl;


		//Begin analysis
		for(size_t g = 0; g < energies.size(); g++)
		{
			xsec_txtout << "\\multicolumn{5}{c}{Energy = " << energies[g] << "} \\\\" << endl;
			multi_txtout << "\\multicolumn{3}{c}{Energy = " << energies[g] << "} \\\\" << endl;

			TDirectory * dir1 = rootout->mkdir(Form("E_%iGeV",(int)energies[g]));
			dir1->cd();

			for(size_t j = 0; j < thicks.size(); j++)
			{
				TDirectory * dir2 = dir1->mkdir(Form("Thick_%imm",(int)thicks[j]));
				dir2->cd();

				for(size_t i = 0; i < materials.size(); i++)
				{
					for(size_t k = 0; k < pguns.size(); k++)
					{
						TString namefile = path+Form("E%i",(int)energies[g]) + "GeV/"+ Form("T%i",(int)thicks[j]) + "mm/Multi_" + pguns[k].GetName() + "_in" + materials[i].GetName() +  ".root";

						cout << "\nAnalysing... " << energies[g] << "GeV " << pguns[k].GetName() << " in " << thicks[j] << "mm of " << materials[i].GetName() << endl;

						TDirectory * dir3 = dir2->mkdir((pguns[k].GetName()+"_in"+materials[i].GetName()).c_str());

						TFile* f = TFile::Open(namefile);
						if(f==0) { cout << "*** ATTENTION!! FILE NOT FOUND! Skipping: " << namefile << endl; continue; }

						TTree * mytree = (TTree*)f->Get("GiGa.TrackSeq.GaussTargetMultiplicity/tree");

						if(mytree!=0)
						{
							TTree* nt = (TTree*)mytree->Clone("treeToUse");

							vector <double> res = doMultiHistos(nt,dir3);//,options,print);
							cout << "\n----------------------------------------- " << endl;

							double Dx = thicks[j];
							double SigmaOverPint = 1000*materials[i].GetSigmaDxOverPintFactor() / Dx;

							xsec_txtout << fixed << setprecision(2) << pguns[k].GetLatex("$") << " in " << thicks[j] << "mm " << (materials[i].GetName()).c_str() << " & $" << res[1] * 1e3 << " \\pm " << res[2] * 1e3 << "$ & $" << fixed << setprecision(0) << res[1] * SigmaOverPint * 1e3 << " \\pm " << res[2] * SigmaOverPint * 1e3 << "$ & $" << fixed << setprecision(2) << res[3] * 1e3 << " \\pm " << res[4] * 1e3 << "$ & $" << fixed << setprecision(0) << res[3] * SigmaOverPint * 1e3<< " \\pm " << res[4] * SigmaOverPint * 1e3 << "$ \\\\" << endl;

							multi_txtout << fixed << setprecision(2) << pguns[k].GetLatex("$") << " in " << thicks[j] << "mm " << (materials[i].GetName()).c_str() << " & " << res[5] << " & " << res[6] << " \\\\" << endl;

							pGun = pguns[k].GetPDG();
							energy = energies[g];
							thickness = Dx;
							mat = i;
							model = m;
							multi = res[5];
							xsec = res[1]+res[3];
							inel_xsec  = res[1];
							multi_err = res[6];
							xsec_err = res[2] + res[4];
							inel_xsec_err = res[2];
							el_xsec = res[3];
							el_xsec_err = res[4];
							percNCh = res[7]*100;
							percPlus = res[8]*100;
							percMinus = res[9]*100;
							multiNCh = res[10];
							multiNCh_nogamma = res[11];
							multi_gamma = res[12];

							dataTree->Fill();

							delete nt;
						}
						else cout << "ATTENTION! TREE NOT FOUND!! Skipping: " << namefile << endl << endl;

						delete dir3;
						delete mytree;
						delete f;
					}

					xsec_txtout << "\\hline" << endl;
					multi_txtout << "\\hline" << endl;
				}
			}
		}

		xsec_txtout << "\\end{tabular}" << endl;
		multi_txtout << "\\end{tabular}" << endl;
		rootout->Close();
		delete rootout;
	}
}


void Plotter::Plot( TString outputPath, string finalPlot, double curthick, bool plotData)
{
	TLegend leg(0.7,0.15,0.99,0.85);
	double Dx = curthick;

	TString var = "";
	if(finalPlot.find("MULTI")!=string::npos) var = "multi";
	else if(finalPlot.find("TOTAL")!=string::npos) var = "xsec";
	else if(finalPlot.find("INEL")!=string::npos) var = "inel_xsec";
	else if(finalPlot.find("EL")!=string::npos) var = "el_xsec";
	else if(finalPlot.find("MULTI_NCH")!=string::npos) var = "multiNCh";
	else if(finalPlot.find("MULTI_NCH_NOGAMMA")!=string::npos) var = "multiNCh_nogamma";
	else if(finalPlot.find("PERC_NCH")!=string::npos) var = "percNCh";
	else if(finalPlot.find("PERC_MINUS")!=string::npos) var = "percMinus";
	else if(finalPlot.find("PERC_PLUS")!=string::npos) var = "percPlus";
	else if(finalPlot.find("MULTI_GAMMA")!=string::npos) var = "multi_gamma";


	if(finalPlot.find("RATIO")!=string::npos || finalPlot.find("ASYM")!=string::npos)
	{
		double pdgenergies[] = { 1, 5, 10, 50, 100};
		double pdgRatios_p[] = {1.67, 1.42, 1.31, 1.14, 1.10};
		double pdgRatios_pi[] = {1.22, 1.13, 1.10, 1.05, 1.03};
		double pdgRatios_K[] = {1.61, 1.32, 1.23, 1.10, 1.07};

		ofstream ratiotxt;
		ratiotxt.open(outputPath+"/ratio_inAl.txt");
		ratiotxt << "\\begin{tabular}{| c | c | c |}" << endl;
		ratiotxt << "$p$ (GeV) & LHEP & BERT \\\\ \\hline" << endl;

		vector < TGraphErrors * > PDGRatio;

		TMultiGraph mgrRatio("mgrRatio","");

		TLegend leg2(0.60,0.79,0.99,0.99);

		for(size_t pg = 0; pg < (pguns.size()-1); pg+=2)
		{
			ratiotxt << "\\multicolumn{2}{c}{ratio " << pguns[pg+1].GetLatex("$") << "/"  << pguns[pg].GetLatex("$") << "} \\\\ \\hline" << endl;

			for(size_t m = 0; m < models.size(); m++)
			{
				ratiotxt << "\\multicolumn{2}{c}{ratio " << models[m] << "} \\\\ \\hline" << endl;

				TString varexp = Form("h_%i",(int)(pg+m));
				stringstream select;
				select << "model == " << m << " && material == " << 0 << " && pGun == " << pguns[pg].GetPDG() << " && thickness == " << Dx;
				dataTree->SetEntryList(NULL);
				dataTree->Draw(">>"+varexp,select.str().c_str(), "entrylist");
				TEntryList *list = (TEntryList*)gDirectory->Get(varexp);
				Int_t entries = list->GetN();
				dataTree->SetEntryList(list);
				dataTree->Draw("energy:"+var+":"+var+"_err");

				double errx[entries];
				for (int ee = 0; ee < entries; ++ee) errx[ee] = 0.;

				TGraphErrors gr1(entries,dataTree->GetV1(),dataTree->GetV2(),errx,dataTree->GetV3());

				select.str("");
				varexp = Form("h_%i",(int)(pg+m+pguns.size()+models.size()));
				select << "model == " << m << " && material == " << 0 << " && pGun == " << pguns[pg+1].GetPDG() << " && thickness == " << Dx;
				dataTree->SetEntryList(NULL);
				dataTree->Draw(">>"+varexp,select.str().c_str(), "entrylist");
				TEntryList *list2 = (TEntryList*)gDirectory->Get(varexp);
				dataTree->SetEntryList(list2);
				dataTree->Draw("energy:"+var+":"+var+"_err");

				TGraphErrors gr2(entries,dataTree->GetV1(),dataTree->GetV2(),errx,dataTree->GetV3());
				Double_t * x = dataTree->GetV1();

				double y[entries];
				double erry[entries];

				for (int ee = 0; ee < gr1.GetN(); ++ee)
				{
					ratiotxt << energies[ee];

					double x1, x2, y1, y2, e1, e2;

					gr1.GetPoint(ee, x1,y1);
					gr2.GetPoint(ee, x2,y2);
					e1 = gr1.GetErrorY(ee);
					e2 = gr2.GetErrorY(ee);

					if(finalPlot.find("RATIO")!=string::npos) y[ee] = y2 / y1;
					else y[ee] = 100*TMath::Abs(y1 - y2)/(2. - y1 - y2);

					double totErr2 = TMath::Power(e1/y1,2) + TMath::Power(e2/y2,2);
					erry[ee] = y[ee] * TMath::Sqrt(totErr2);

					ratiotxt << fixed << setprecision(2) << " & $" << y[ee]  << " \\pm " << erry[ee] << "\\\\" << endl;
				}



				TGraphErrors * gr = new TGraphErrors(entries,x,y,errx,erry);

				gr->SetMarkerColor(1+pg/2);
				gr->SetMarkerStyle(20+m);

				string label = pguns[pg+1].GetLatex("LEG") + " / " + pguns[pg].GetLatex("LEG");
				if(models.size() > 1) label += " (" + models[m] + ")";
				leg2.AddEntry(gr,label.c_str(),"P");

				if(plotData)
				{
					TGraphErrors * grPDG;
					if(pguns[pg].GetName()=="p") grPDG = new TGraphErrors(5,pdgenergies,pdgRatios_p);
					else if(pguns[pg].GetName()=="Piplus") grPDG = new TGraphErrors(5,pdgenergies,pdgRatios_pi);
					else if(pguns[pg].GetName()=="Kplus") grPDG = new TGraphErrors(5,pdgenergies,pdgRatios_K);

					grPDG->SetMarkerColor(4);
					grPDG->SetMarkerStyle(34);
					grPDG->SetMarkerSize(1.2);
					if(m==0)
					{
						PDGRatio.push_back(grPDG);
						leg2.AddEntry(grPDG,"PDG ratio","P");
					}
				}

				mgrRatio.Add(gr);
			}
			ratiotxt << "\\hline" << endl;
		}

		ratiotxt << "\\end{tabular}" << endl;


		if(plotData && finalPlot == "RATIO_TOTAL") mgrRatio.Add(PDGRatio[0]);

		TCanvas * cratio = new TCanvas();
		cratio->SetLogx();
		cratio->SetGrid();

		mgrRatio.SetTitle(finalPlot.c_str());
		mgrRatio.Draw("APL");
		if(finalPlot.find("RATIO")!=string::npos) mgrRatio.GetYaxis()->SetTitle("Ratio");
		else mgrRatio.GetYaxis()->SetTitle("Asym (%)");
		mgrRatio.GetXaxis()->SetTitle("|p| (GeV)");
		mgrRatio.GetXaxis()->SetLimits(0.5,300);

		leg2.Draw("same");

		cratio->Print(outputPath + "/" + finalPlot + ".pdf");
		delete cratio;
	}
	else
	{
		TCanvas * c1 = new TCanvas();

		TString nameMultigr = Form("mgr%e",curthick);
		TString titleMultigr = finalPlot;
		if(finalPlot == "TOTAL") titleMultigr = Form("Total (inel + el) cross sections in %i mm",(int)curthick);
		else if(finalPlot == "INEL") titleMultigr = Form("Inelastic cross sections in %i mm",(int)curthick);
		else if(finalPlot == "EL") titleMultigr = Form("Elastic cross sections in %i mm",(int)curthick);
		else if(finalPlot == "PERC_PLUS") titleMultigr = "Percentage of positive particles";
		else if(finalPlot == "PERC_MINUS") titleMultigr = "Percentage of negative particles";
		else if(finalPlot == "MULTI_NCH") titleMultigr = Form("Multiplicity (neutral) %i mm",(int)curthick);
		else if(finalPlot == "MULTI_NCH_NOGAMMA") titleMultigr = Form("Multiplicity (neutral minus gamma) %i mm",(int)curthick);
		else if(finalPlot == "PERC_NCH") titleMultigr = "Percentage of neutral particles";
		else if(finalPlot == "MULTI_NOGAMMA") titleMultigr = Form("Gamma multiplicity %i mm",(int)curthick);

		TMultiGraph mgr(nameMultigr,titleMultigr);

		double PintOverSigmaFactor = Dx /(1000. * materials[0].GetSigmaDxOverPintFactor() * 1000.);	

		if(plotData)
		{
			//COMPAS Inelastic Xsec data in Al
			double COMPAS_p_x[] = {1.52, 5., 9., 20., 30., 60.};
			double COMPAS_p_sigmaErr[] = {10., 4., 4., 5., 5., 7.};
			double COMPAS_p_sigma[] = {445., 445., 465., 446., 445., 455.};
			double * COMPAS_p_y = ScalarProd(6,COMPAS_p_sigma,PintOverSigmaFactor);
			double * COMPAS_p_yErr = ScalarProd(6,COMPAS_p_sigmaErr,PintOverSigmaFactor);
			double COMPAS_pbar_x[] = {1.45,6.65,13.3,25.,30.,60.};
			double COMPAS_pbar_sigma[] = {617., 558., 536., 480., 457., 439.};
			double COMPAS_pbar_sigmaErr[] = {17., 10., 10., 9., 11., 13.};
			double * COMPAS_pbar_y = ScalarProd(6,COMPAS_pbar_sigma,PintOverSigmaFactor);
			double * COMPAS_pbar_yErr = ScalarProd(6,COMPAS_pbar_sigmaErr,PintOverSigmaFactor);

			//COMPAS Inelastic Xsec data in Be

			double COMPAS_inBe_p_x[] = {3., 5., 9., 30.,50.,60.};
			double COMPAS_inBe_p_sigmaErr[] = {4., 3., 3., 3., 3., 2.};
			double COMPAS_inBe_p_sigma[] = {236., 207., 210., 210., 210., 216.};
			double * COMPAS_inBe_p_y = ScalarProd(6,COMPAS_inBe_p_sigma,PintOverSigmaFactor);
			double * COMPAS_inBe_p_yErr = ScalarProd(6,COMPAS_inBe_p_sigmaErr,PintOverSigmaFactor);
			double COMPAS_inBe_pbar_x[] = {6.65,13.3,20.,30.,40.};
			double COMPAS_inBe_pbar_sigma[] = {296., 275., 240., 235., 226., 190.};
			double COMPAS_inBe_pbar_sigmaErr[] = {6., 4., 10., 6., 7.};
			double * COMPAS_inBe_pbar_y = ScalarProd(5,COMPAS_inBe_pbar_sigma,PintOverSigmaFactor);
			double * COMPAS_inBe_pbar_yErr = ScalarProd(5,COMPAS_inBe_pbar_sigmaErr,PintOverSigmaFactor);

			//COMPAS Total Xsec data in Al

			double COMPASTot_p_x[] = {1.52, 1.8, 19.3, 20.};
			double COMPASTot_p_sigmaErr[] = {22., 27., 10., 10.};
			double COMPASTot_p_sigma[] = {687., 694., 687., 687.};
			double * COMPASTot_p_y = ScalarProd(4,COMPASTot_p_sigma,PintOverSigmaFactor);
			double * COMPASTot_p_yErr = ScalarProd(4,COMPASTot_p_sigmaErr,PintOverSigmaFactor);
			double COMPASTot_pbar_x[] = {1.45,1.8};
			double COMPASTot_pbar_sigma[] = {1034., 1066.};
			double COMPASTot_pbar_sigmaErr[] = {40., 40.};
			double * COMPASTot_pbar_y = ScalarProd(2,COMPASTot_pbar_sigma,PintOverSigmaFactor);
			double * COMPASTot_pbar_yErr = ScalarProd(2,COMPASTot_pbar_sigmaErr,PintOverSigmaFactor);

			TGraphErrors * COMPAS_p_gr = new TGraphErrors(6,COMPAS_p_x,COMPAS_p_y,0,COMPAS_p_yErr);
			TGraphErrors * COMPAS_pbar_gr = new TGraphErrors(6,COMPAS_pbar_x,COMPAS_pbar_y,0,COMPAS_pbar_yErr);
			COMPAS_p_gr->SetMarkerColor(4);
			COMPAS_p_gr->SetMarkerStyle(29);
			COMPAS_p_gr->SetMarkerSize(1.1);
			COMPAS_pbar_gr->SetMarkerColor(4);
			COMPAS_pbar_gr->SetMarkerStyle(30);
			COMPAS_pbar_gr->SetMarkerSize(1.2);

			TGraphErrors * COMPASTot_p_gr = new TGraphErrors(4,COMPASTot_p_x,COMPASTot_p_y,0,COMPASTot_p_yErr);
			TGraphErrors * COMPASTot_pbar_gr = new TGraphErrors(2,COMPASTot_pbar_x,COMPASTot_pbar_y,0,COMPASTot_pbar_yErr);
			COMPASTot_p_gr->SetMarkerColor(4);
			COMPASTot_p_gr->SetMarkerStyle(29);
			COMPASTot_p_gr->SetMarkerSize(1.1);
			COMPASTot_pbar_gr->SetMarkerColor(4);
			COMPASTot_pbar_gr->SetMarkerStyle(30);
			COMPASTot_pbar_gr->SetMarkerSize(1.2);

			TGraphErrors * COMPAS_inBe_p_gr = new TGraphErrors(6,COMPAS_inBe_p_x,COMPAS_inBe_p_y,0,COMPAS_inBe_p_yErr);
			TGraphErrors * COMPAS_inBe_pbar_gr = new TGraphErrors(5,COMPAS_inBe_pbar_x,COMPAS_inBe_pbar_y,0,COMPAS_inBe_pbar_yErr);
			COMPAS_inBe_p_gr->SetMarkerColor(4);
			COMPAS_inBe_p_gr->SetMarkerStyle(29);
			COMPAS_inBe_p_gr->SetMarkerSize(1.1);
			COMPAS_inBe_pbar_gr->SetMarkerColor(4);
			COMPAS_inBe_pbar_gr->SetMarkerStyle(30);
			COMPAS_inBe_pbar_gr->SetMarkerSize(1.2);



			if(finalPlot == "TOTAL")
			{
				mgr.Add(COMPASTot_p_gr);
				mgr.Add(COMPASTot_pbar_gr);
				leg.AddEntry(COMPASTot_p_gr,"COMPASTot p","P");
				leg.AddEntry(COMPASTot_pbar_gr,"COMPASTot #bar{p}","P");
			}
			else if(finalPlot == "INEL")
			{
				mgr.Add(COMPAS_p_gr);
				mgr.Add(COMPAS_pbar_gr);
				leg.AddEntry(COMPAS_p_gr,"COMPAS p","P");
				leg.AddEntry(COMPAS_pbar_gr,"COMPAS #bar{p}","P");
			}
		}


		//Plotting Gauss values
		int nh = 0;
		for(size_t n0 = 0; n0 < models.size(); n0++) 
			for(size_t n1 = 0; n1 < materials.size(); n1++) 
				for(size_t n2 = 0; n2 < pguns.size(); n2++) 
				{
					TString varexp = Form("h_%i",nh);
					nh++;

					stringstream select;
					select << "model == " << n0 << " && material == " << n1 << " && pGun == " << pguns[n2].GetPDG() << " && thickness == " << Dx;

					dataTree->SetEntryList(NULL);
					dataTree->Draw(">>"+varexp,select.str().c_str(), "entrylist");
					TEntryList *list = (TEntryList*)gDirectory->Get(varexp);
					int entries = list->GetN();
					dataTree->SetEntryList(list);

					if(finalPlot == "MULTI" || finalPlot == "TOTAL" || finalPlot == "INEL" || finalPlot == "EL")
						dataTree->Draw("energy:"+var+":"+var+"_err");	
					else dataTree->Draw("energy:"+var);

					double errx[entries];
					for (int ee = 0; ee < entries; ++ee) errx[ee] = 0.;

					TGraphErrors *gr = new TGraphErrors(entries,dataTree->GetV1(),dataTree->GetV2(),errx,dataTree->GetV3());

					gr->SetMarkerColor(1+n2);
					if(n0==0) gr->SetMarkerStyle(20+n0);
					else gr->SetMarkerStyle(24+n0);
					if(n0==0) gr->SetMarkerSize(1.1);
					string label = pguns[n2].GetLatex("LEG") + " in " + materials[n1].GetName();
					if(models.size() > 1) label += " (" + models[n0] + ")";
					leg.AddEntry(gr,label.c_str(),"P");

					mgr.Add(gr);
				}

		c1->Clear();
		mgr.Draw("APL");

		if(finalPlot == "TOTAL")     mgr.GetYaxis()->SetTitle("P^{tot}_{int} = N^{inel+el}/N^{gen}");
		else if(finalPlot == "INEL") mgr.GetYaxis()->SetTitle("P^{inel}_{int} = N^{inel}/N^{gen}");
		else if(finalPlot == "EL")   mgr.GetYaxis()->SetTitle("P^{el}_{int} = N^{el}/N^{gen}");
		else if(((string)finalPlot).find("MULTI")!=string::npos) mgr.GetYaxis()->SetTitle("< Multi >");
		else if(((string)finalPlot).find("PERC")!=string::npos) mgr.GetYaxis()->SetTitle("%");

		mgr.GetXaxis()->SetTitle("|p| (GeV)");
		mgr.GetXaxis()->SetLimits(0.5,900);

		c1->SetGrid();
		c1->SetLogx();
		leg.Draw("same");

		TString printname = outputPath+"/" + finalPlot + Form("_%i",(int)curthick) + "mm.pdf";
		c1->Print(printname);
		delete c1;
	}
}
