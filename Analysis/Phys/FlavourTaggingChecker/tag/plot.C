{
  /*-----------------------------------------------------------
   *---  DRAW PLOTS -------------------------------------------
   *-----------------------------------------------------------*/
  gROOT->Reset();
  gROOT->ProcessLine(".L plot.h+"); //load some functions defined there
  TFile fhisto("output/tag.root");
  //  TFile fhisto("output/tag_dstar0jun.root");
  //  TFile fhisto("output/tag_dstar15jun.root");

  TCanvas* c = new TCanvas("c","Tagging", 0, 0, 600, 600);
  c->SetBorderMode(0);
  c->SetFrameBorderMode(0);
  c->SetFillColor(10);
  gStyle->SetDrawBorder(0);
  gStyle->SetOptFit(1);
  gStyle->SetOptStat(0);
  gStyle->SetTitleFillColor(5);

  bool endblock = 1;
  //  goto taggercandidates;     //to compare 2 files
  //  goto taggercandidatesRW;   //to compare right/wrong 2 files
  //  goto omegataggers;         //plot omega of all taggers (split by categories)
  //  goto omegapt;              //plot omega vs pt (all/tis/tos)
  //  goto seed;                 //vertex
  //  goto addseed;
  //  goto vtxcharge;
  //  goto config;               //dphi + oldcut plot
  //  goto pid;                  //plot pid
  //  goto prob_min;             //probmin cut for ind taggers and comb
  //  goto correlations;         //correlations for NN variables
  //  goto effeff_cut;           //effeff for a given cut
  //  goto general;              //properties of Bsig
  goto nnet;                 //to tune polinomials from NN
  //  goto asymm;                //asymmetry plots for B0
  //  goto multipleinteraction;  //multiple interactions
  //  goto secondaryvertex;      //SV and multiple interaction


taggercandidates: ////////////////////////////////////////////////////

  cout<<"plotting at taggercandidates"<<endl;

  //compare same plots on two different root files
  TString f1 = "output/tag_1.root";  //
  TString f2 = "output/tag_2.root"; //in red

  c->Divide(2,2);
  c->cd(1); plot_taggercut(f1,f2,"h1");
  c->cd(2); plot_taggercut(f1,f2,"h2");
  c->cd(3); plot_taggercut(f1,f2,"h6");
  c->cd(4); plot_taggercut(f1,f2,"h4");
  c->Print("output/hcut_B.gif"); if(wait())return;

  c->cd(1); plot_taggercut(f1,f2,"hcut_mu_cand");
  c->cd(2); plot_taggercut(f1,f2,"hcut_ele_cand");
  c->cd(3); plot_taggercut(f1,f2,"hcut_k_cand");
  c->cd(4); plot_taggercut(f1,f2,"hcut_kS_cand");
  c->Print("output/hcut_cand.gif"); if(wait())return;

  c->cd(1); plot_taggercut(f1,f2,"hcut_mu_pid");
  c->cd(2); plot_taggercut(f1,f2,"hcut_mu_pt");
  c->cd(3); plot_taggercut(f1,f2,"hcut_mu_p");
  c->cd(4); plot_taggercut(f1,f2,"hcut_mu_lcs");
  c->Print("output/hcut_mu1.gif"); if(wait())return;
  c->cd(1); plot_taggercut(f1,f2,"hcut_mu_ippu");
  c->cd(2); plot_taggercut(f1,f2,"hcut_mu_N");
  c->cd(3); plot_taggercut(f1,f2,"hcut_mu_ipssign");
  c->Print("output/hcut_mu2.gif"); if(wait())return;

  c->cd(1); plot_taggercut(f1,f2,"hcut_ele_pidk");
  c->cd(2); plot_taggercut(f1,f2,"hcut_ele_pt");
  c->cd(3); plot_taggercut(f1,f2,"hcut_ele_p");
  c->cd(4); plot_taggercut(f1,f2,"hcut_ele_lcs");
  c->Print("output/hcut_ele1.gif"); if(wait())return;
  c->cd(1); plot_taggercut(f1,f2,"hcut_ele_gho");
  c->cd(2); plot_taggercut(f1,f2,"hcut_ele_EoP");
  c->cd(3); plot_taggercut(f1,f2,"hcut_ele_velo");
  c->cd(4); plot_taggercut(f1,f2,"hcut_ele_N");
  c->Print("output/hcut_ele2.gif"); if(wait())return;

  c->cd(1); plot_taggercut(f1,f2,"hcut_ko_pidk");
  c->cd(2); plot_taggercut(f1,f2,"hcut_ko_pidp");
  c->cd(3); plot_taggercut(f1,f2,"hcut_ko_pt");
  c->cd(4); plot_taggercut(f1,f2,"hcut_ko_p");
  c->Print("output/hcut_koppo0.gif"); if(wait())return;
  c->cd(1); plot_taggercut(f1,f2,"hcut_ko_pidk");
  c->cd(2); plot_taggercut(f1,f2,"hcut_ko_pidp");
  c->cd(3); plot_taggercut(f1,f2,"hcut_ko_pidkp");
  c->cd(4); plot_taggercut(f1,f2,"hcut_ko_N");
  c->Print("output/hcut_koppo1.gif"); if(wait())return;
  c->cd(1); plot_taggercut(f1,f2,"hcut_ko_lcs");
  c->cd(2); plot_taggercut(f1,f2,"hcut_ko_gho");
  c->cd(3); plot_taggercut(f1,f2,"hcut_ko_IPs");
  c->cd(4); plot_taggercut(f1,f2,"hcut_ko_IP");
  c->Print("output/hcut_koppo2.gif"); if(wait())return;
  c->cd(1); plot_taggercut(f1,f2,"hcut_ko_ippu");
  c->cd(2); plot_taggercut(f1,f2,"hcut_ko_N");
  c->cd(3); plot_taggercut(f1,f2,"hcut_ko_pt");
  c->cd(4); plot_taggercut(f1,f2,"hcut_ko_p");
  c->Print("output/hcut_koppo3.gif"); if(wait())return;

  c->cd(1); plot_taggercut(f1,f2,"hcut_pS_pt");
  c->cd(2); plot_taggercut(f1,f2,"hcut_pS_p");
  c->cd(3); plot_taggercut(f1,f2,"hcut_pS_lcs");
  c->cd(4); plot_taggercut(f1,f2,"hcut_pS_IPs");
  c->Print("output/hcut_pionsame1.gif"); if(wait())return;
  c->cd(1); plot_taggercut(f1,f2,"hcut_pS_gho");
  c->cd(2); plot_taggercut(f1,f2,"hcut_pS_dq");
  c->cd(3); plot_taggercut(f1,f2,"hcut_pS_dqe");
  c->Print("output/hcut_pionsame2.gif");

  c->cd(1); plot_taggercut(f1,f2,"hcut_kS_pidk");
  c->cd(2); plot_taggercut(f1,f2,"hcut_kS_pidp");
  c->cd(3); plot_taggercut(f1,f2,"hcut_kS_pt");
  c->cd(4); plot_taggercut(f1,f2,"hcut_kS_p");
  c->Print("output/hcut_ksame1.gif"); if(wait())return;
  c->cd(1); plot_taggercut(f1,f2,"hcut_kS_lcs");
  c->cd(2); plot_taggercut(f1,f2,"hcut_kS_IPs");
  c->cd(3); plot_taggercut(f1,f2,"hcut_kS_ippu");
  c->cd(4); plot_taggercut(f1,f2,"hcut_kS_N");
  c->Print("output/hcut_ksame2.gif"); if(wait())return;
  c->cd(1); plot_taggercut(f1,f2,"hcut_kS_dphi");
  c->cd(2); plot_taggercut(f1,f2,"hcut_kS_deta");
  c->cd(3); plot_taggercut(f1,f2,"hcut_kS_dq");
  c->Print("output/hcut_ksame3.gif"); if(wait())return;

  goto end;

  
taggercandidatesRW: ////////////////////////////////////////////////////

  c->SetBorderMode(0);
  c->SetFrameBorderMode(0);
  c->SetFillColor(10);

  cout<<"plotting at taggercandidates RW"<<endl;
  //compare same plots on two different root files
  TString f1 = "output/tag_1.root";  //in nero Dati
  TString f2 = "output/tag_2.root"; //in red Montecarlo

  c->Divide(2,1);
  c->cd(1); plot_taggercut_er(f1,f2,"hr_mu_pt");
  c->cd(2); plot_taggercut_er(f1,f2,"hw_mu_pt");
  c->Print("output/hcut_er_mu_pT_RW.pdf"); if(wait())return;
  c->cd(1); plot_taggercut_er(f1,f2,"hr_mu_p");
  c->cd(2); plot_taggercut_er(f1,f2,"hw_mu_p");
  c->Print("output/hcut_er_mu_p_RW.pdf"); if(wait())return;
  c->cd(1); plot_taggercut_er(f1,f2,"hr_mu_ips");
  c->cd(2); plot_taggercut_er(f1,f2,"hw_mu_ips");
  c->Print("output/hcut_er_mu_ips_RW.pdf"); if(wait())return;
  c->cd(1); plot_taggercut_er(f1,f2,"hr_mu_ippu");
  c->cd(2); plot_taggercut_er(f1,f2,"hw_mu_ippu");
  c->Print("output/hcut_er_mu_ippu_RW.pdf"); if(wait())return;
  c->cd(1); plot_taggercut_er(f1,f2,"hr_mu_pid");
  c->cd(2); plot_taggercut_er(f1,f2,"hw_mu_pid");
  c->Print("output/hcut_er_mu_pid_RW.pdf"); if(wait())return;

  c->cd(1); plot_taggercut_er(f1,f2,"hr_ele_pt");
  c->cd(2); plot_taggercut_er(f1,f2,"hw_ele_pt");
  c->Print("output/hcut_er_ele_pt_RW.pdf"); if(wait())return;
  c->cd(1); plot_taggercut_er(f1,f2,"hr_ele_p");
  c->cd(2); plot_taggercut_er(f1,f2,"hw_ele_p");
  c->Print("output/hcut_er_ele_p_RW.pdf"); if(wait())return;
  c->cd(1); plot_taggercut_er(f1,f2,"hr_ele_ips");
  c->cd(2); plot_taggercut_er(f1,f2,"hw_ele_ips");
  c->Print("output/hcut_er_ele_ips_RW.pdf"); if(wait())return;
  c->cd(1); plot_taggercut_er(f1,f2,"hr_ele_ippu");
  c->cd(2); plot_taggercut_er(f1,f2,"hw_ele_ippu");
  c->Print("output/hcut_er_ele_ippu_RW.pdf"); if(wait())return;
  c->cd(1); plot_taggercut_er(f1,f2,"hr_ele_pid");
  c->cd(2); plot_taggercut_er(f1,f2,"hw_ele_pid");
  c->Print("output/hcut_er_ele_pid_RW.pdf"); if(wait())return;

  c->cd(1); plot_taggercut_er(f1,f2,"hr_k_pt");
  c->cd(2); plot_taggercut_er(f1,f2,"hw_k_pt");
  c->Print("output/hcut_er_k_pt_RW.pdf"); if(wait())return;
  c->cd(1); plot_taggercut_er(f1,f2,"hr_k_p");
  c->cd(2); plot_taggercut_er(f1,f2,"hw_k_p");
  c->Print("output/hcut_er_k_p_RW.pdf"); if(wait())return;
  c->cd(1); plot_taggercut_er(f1,f2,"hr_k_ips");
  c->cd(2); plot_taggercut_er(f1,f2,"hw_k_ips");
  c->Print("output/hcut_er_k_ips_RW.pdf"); if(wait())return;
  c->cd(1); plot_taggercut_er(f1,f2,"hr_k_ippu");
  c->cd(2); plot_taggercut_er(f1,f2,"hw_k_ippu");
  c->Print("output/hcut_er_k_ippu_RW.pdf"); if(wait())return;
  c->cd(1); plot_taggercut_er(f1,f2,"hr_k_pid");
  c->cd(2); plot_taggercut_er(f1,f2,"hw_k_pid");
  c->Print("output/hcut_er_k_pid_RW.pdf"); if(wait())return;

  c->cd(1); plot_taggercut_er(f1,f2,"hr_kS_pt");
  c->cd(2); plot_taggercut_er(f1,f2,"hw_kS_pt");
  c->Print("output/hcut_er_pS_pt_RW.pdf"); if(wait())return;
  c->cd(1); plot_taggercut_er(f1,f2,"hr_kS_p");
  c->cd(2); plot_taggercut_er(f1,f2,"hw_kS_p");
  c->Print("output/hcut_er_pS_p_RW.pdf"); if(wait())return;
  c->cd(1); plot_taggercut_er(f1,f2,"hr_kS_ips");
  c->cd(2); plot_taggercut_er(f1,f2,"hw_kS_ips");
  c->Print("output/hcut_er_pS_ips_RW.pdf"); if(wait())return;
  c->cd(1); plot_taggercut_er(f1,f2,"hr_kS_ippu");
  c->cd(2); plot_taggercut_er(f1,f2,"hw_kS_ippu");
  c->Print("output/hcut_er_pS_ippu_RW.pdf"); if(wait())return;
  c->cd(1); plot_taggercut_er(f1,f2,"hr_kS_pid");
  c->cd(2); plot_taggercut_er(f1,f2,"hw_kS_pid");
  c->Print("output/hcut_er_pS_pid_RW.pdf"); if(wait())return;

  goto end;
  
omegataggers: ///////////////////////////////////////////////////////

  cout<<"Plot omega for taggers"<<endl;
  TCanvas *c = new TCanvas("c", "Tagging",0,0,600,300);
  gStyle->SetOptFit(1);
  gStyle->SetOptStat(0);
  c->Range(0,0,1,1);
  c->SetFillColor(10);
  c->SetBorderMode(0);
  c->SetBorderSize(2);
  c->SetFrameFillColor(0);
  c->SetFrameBorderMode(0);
  c->Divide(2,1);
  c->cd(1); 
  hom_vtx->SetTitle("Omega distribution");
  hom_vtx->GetXaxis()->SetTitle("Omega");
  hom_vtx->GetYaxis()->SetTitle("Events");
  hom_vtx->GetYaxis()->SetTitle("Events");
  hom_vtx->SetLineColor(7);      hom_vtx->Draw();
  hom_ss->SetLineColor(6);       hom_ss->Draw("same");
  hom_k->SetLineColor(5);        hom_k->Draw("same");
  hom_e->SetLineColor(4);        hom_e->Draw("same");
  hom_mu->SetLineColor(3);       hom_mu->Draw("same");
  TLegend *leg = new TLegend(0.73,0.699,0.885,0.886,NULL,"brNDC");
  leg->SetTextSize(0.028);
  leg->SetShadowColor(0);
  TLegendEntry *entry=leg->AddEntry("hom_muon"," muon","l");
  entry=leg->AddEntry("hom_e"," ele","l");
  entry=leg->AddEntry("hom_k"," kaon","l");
  entry=leg->AddEntry("hom_ss"," sameside","l");
  entry=leg->AddEntry("hom_vtx"," vtx","l");
  leg->Draw();
  c->cd(2); 
  hom_combOSSS->SetTitle("Omega combined distribution");
  hom_combOSSS->GetXaxis()->SetTitle("Omega");
  hom_combOSSS->GetYaxis()->SetTitle("Events");
  hom_combOSSS->SetLineWidth(3);   hom_combOSSS->SetLineColor(1);   hom_combOSSS->Draw();  
  hom_combOS->SetLineWidth(3); hom_combOS->SetLineColor(2); hom_combOS->Draw("same");
  //categories
  l1 = new TLine(0.17,0, 0.17, 2000);
  l2 = new TLine(0.24,0, 0.24, 2000);
  l3 = new TLine(0.31,0, 0.31, 2000);
  l4 = new TLine(0.38,0, 0.38, 2000);
  l1->Draw("same");  l2->Draw("same");  l3->Draw("same");  l4->Draw("same");
  TLegend *leg2 = new TLegend(0.74,0.699,0.882,0.886,NULL,"brNDC");
  leg2->SetTextSize(0.03);
  leg2->SetShadowColor(0);
  TLegendEntry *entry2=leg2->AddEntry("hom_combOSSS"," OS + SS","l");
  entry2=leg2->AddEntry("hom_combOS"," OS","l");
  leg2->Draw();

  goto end;

 omegapt: /////////////////////////////////////////////////////////////

  cout<<"plotting at omegapt"<<endl;
  c->Clear(); c->Divide(1,2);
  h6w->Draw();
  plot_omega(h6w,    h6r,    "PtB");     if(wait())return;
  plot_omega(h6rtis, h6wtis, "PtB_tis"); if(wait())return;
  plot_omega(h6rtos, h6wtos, "PtB_tos"); if(wait())return;
  plot_omega(h6rtistos, h6wtistos,  "PtB_tistos"); 

  if(endblock) goto end;

 seed: /////////////////////////////////////////////////////////////
  
  goto seedrightwrong;
  cout<<"plotting seed from B?"<<endl;
  //hvsvpointtheta0->Draw(); if(wait())return;
  plot_ratio("chi2", hvChi2 , hvChi0, hvChi0);         if(wait())return;
  plot_ratio("ptmin", hvpmin_2 ,hvpmin_0 , hvpmin_0);  if(wait())return;
  plot_ratio("ipmax", hvipmax2 ,hvipmax0 , hvipmax0);  if(wait())return;
  plot_ratio("ipsmin", hvipsmin2 ,hvipsmin0 ,hvipsmin0); if(wait())return;
  plot_ratio("deltaphi",hvdphi2  , hvdphi0, hvdphi0);  if(wait())return;
  plot_ratio("delta_r",hz2, hz0, hz0);                 if(wait())return;
  plot_ratio("hvsvpointtheta", hvsvpointtheta2 , hvsvpointtheta0, hvsvpointtheta0); if(wait())return;
  plot_ratio("hvmass", hvmass2 , hvmass0, hvmass0);    if(wait())return;
  plot_ratio("hvreson", hvreson2 , hvreson0, hvreson0);if(wait())return;   
  plot_ratio("probf",hvprobf2,hvprobf0,hvprobf0 );           if(wait())return;
  plot_ratio("maxprobf",hvmaxprobf2, hvmaxprobf0, hvmaxprobf0); if(wait())return;

 seedrightwrong:
  cout<<"plotting seed right/wrong"<<endl;
  plot_Eff_Omega(hr_seed_ipsmin, hw_seed_ipsmin);             if(wait())return;
  plot_Eff_Omega(hr_seed_ipmax, hw_seed_ipmax, "right2left");               if(wait())return;
  plot_Eff_Omega(hr_seed_ptmin, hw_seed_ptmin);               if(wait())return;
  plot_Eff_Omega(hr_seed_probf, hw_seed_probf);               if(wait())return;
  plot_Eff_Omega(hr_seed_chi2, hw_seed_chi2, "right2left");               if(wait())return;
  plot_Eff_Omega(hr_seed_dphi, hw_seed_dphi);                 if(wait())return;
  plot_Eff_Omega(hr_seed_dphi, hw_seed_dphi, "right2left");                 if(wait())return;
  plot_Eff_Omega(hr_seed_rdist, hw_seed_rdist, "right2left");               if(wait())return;
  plot_Eff_Omega(hr_seed_svpointtheta, hw_seed_svpointtheta, "right2left"); if(wait())return;
  
  if(endblock) goto end;

 addseed: /////////////////////////////////////////////////////////////

  goto addseedrightwrong;
  cout<<"plotting add track to seed from B?"<<endl;
  plot_ratio("pt",htr_pt1, htr_pt0, htr_pt0 );          if(wait())return;
  plot_ratio("ip",htr_ip1, htr_ip0, htr_ip0 );          if(wait())return;
  plot_ratio("ips",htr_ips1, htr_ips0, htr_ips0 );      if(wait())return;
  plot_ratio("ipsv",htr_ipsv1, htr_ipsv0, htr_ipsv0 );  if(wait())return;
  plot_ratio("doca",htr_doca1, htr_doca0, htr_doca0 );  if(wait())return;
  plot_ratio("lcs",htr_lcs1, htr_lcs0, htr_lcs0 );     if(wait())return;

 addseedrightwrong:
  cout<<"add track to seed right/wrong"<<endl;
  plot_Eff_Omega(hr_tr_pt, hw_tr_pt);             if(wait())return;
  plot_Eff_Omega(hr_tr_ip, hw_tr_ip);             if(wait())return;
  plot_Eff_Omega(hr_tr_ip, hw_tr_ip, "right2left");             if(wait())return;
  plot_Eff_Omega(hr_tr_ips, hw_tr_ips);             if(wait())return;
  plot_Eff_Omega(hr_tr_ips, hw_tr_ips, "right2left");             if(wait())return;
  plot_Eff_Omega(hr_tr_ipsv, hw_tr_ipsv);             if(wait())return;
  plot_Eff_Omega(hr_tr_ipsv, hw_tr_ipsv, "right2left");             if(wait())return;
  plot_Eff_Omega(hr_tr_lcs, hw_tr_lcs, "right2left");             if(wait())return;
  plot_Eff_Omega(hr_tr_docamax, hw_tr_docamax);             if(wait())return;
  plot_Eff_Omega(hr_tr_docamax, hw_tr_docamax, "right2left");             if(wait())return;

  if(endblock) goto end;

 vtxcharge: /////////////////////////////////////////////////////////////

  goto vtxchargecuts;
  cout<<"plotting vtx properties from tagger"<<endl;
  plot_Eff_Omega(hr_vtx_vmaxprobf, hw_vtx_vmaxprobf); if(wait())return;
  plot_Eff_Omega(hr_vtx_vptmean, hw_vtx_vptmean); if(wait())return;
  plot_Eff_Omega(hr_vtx_vptsum, hw_vtx_vptsum); if(wait())return;
  plot_Eff_Omega(hr_vtx_vipsmean, hw_vtx_vipsmean); if(wait())return;
  plot_Eff_Omega(hr_vtx_vipsmean, hw_vtx_vipsmean, "right2left"); if(wait())return;
  plot_Eff_Omega(hr_vtx_vdocamax, hw_vtx_vdocamax); if(wait())return;
  plot_Eff_Omega(hr_vtx_vdocamax, hw_vtx_vdocamax, "right2left"); if(wait())return;
  plot_Eff_Omega(hr_vtx_vdphidir, hw_vtx_vdphidir); if(wait())return;
  plot_Eff_Omega(hr_vtx_vdphidir, hw_vtx_vdphidir, "right2left"); if(wait())return;
  plot_Eff_Omega(hr_vtx_vsvpt, hw_vtx_vsvpt); if(wait())return;
  plot_Eff_Omega(hr_vtx_vsvtheta, hw_vtx_vsvtheta, "right2left"); if(wait())return;
  plot_Eff_Omega(hr_vtx_vsvphi, hw_vtx_vsvphi); if(wait())return;
  plot_Eff_Omega(hr_vtx_vsvdeltar, hw_vtx_vsvdeltar); if(wait())return;
  plot_Eff_Omega(hr_vtx_vsvmag, hw_vtx_vsvmag); if(wait())return;
  plot_Eff_Omega(hr_vtx_vsvp, hw_vtx_vsvp); if(wait())return;
  plot_Eff_Omega(hr_vtx_vsvm, hw_vtx_vsvm); if(wait())return;

  //if(endblock) goto end;

 vtxchargecuts:
  cout<<"plotting vtx properties"<<endl;
  plotEffectiveEff(hr_vtx_prob, hw_vtx_prob); if(wait())return;
  plotEffectiveEff(hr_vtx_ch, hw_vtx_ch); if(wait())return;
  plotEffectiveEff(hr_vtx_like, hw_vtx_like); if(wait())return;
  plotEffectiveEff(hr_vtx_dphidir, hw_vtx_dphidir, "right2left"); if(wait())return;
  plotEffectiveEff(hr_vtx_sumpt, hw_vtx_sumpt); if(wait())return;
  plotEffectiveEff(hr_vtx_sumips, hw_vtx_sumips); if(wait())return;
  plotEffectiveEff(hr_vtx_sumdoca, hw_vtx_sumdoca, "right2left"); if(wait())return;
  plotEffectiveEff(hr_vtx_M, hw_vtx_M); if(wait())return;
  plotEffectiveEff(hr_vtx_P, hw_vtx_P); if(wait())return;
  plotEffectiveEff(hr_vtx_Mag, hw_vtx_Mag); if(wait())return;

  if(endblock) goto end;

 plotvtxcharge:
  cout<<"plotting at vtxcharge"<<endl;
  //make sure tVch->setcharge( Vch ); in TaggerVertex and put MinVCh to 0!
  hr_vtx_ch->SetLineColor(kGreen); hr_vtx_ch->Draw();
  hw_vtx_ch->SetLineColor(kRed); hw_vtx_ch->Draw("same");
  //  hvtxch_u->SetLineColor(kBlue); hvtxch_u->Draw("same");
  c->Print("output/hvtxch.gif"); if(wait())return;
  plot_Eff_Omega(hr_vtx_ch, hw_vtx_ch); if(wait())return;
  plotEffectiveEff(hr_vtx_ch, hw_vtx_ch); if(wait())return;

  //plot k vs charge
  Int_t na=6, nb=5, nc=4, err=0.08;
  Double_t xa[na] = {0.2, 0.25, 0.3, 0.35, 0.4, 0.5};
  Double_t xb[nb] = {0.25, 0.3, 0.35, 0.4, 0.5};
  Double_t xc[nc] = {0.3, 0.35, 0.4, 0.5};
  Double_t va[na] = {0.77, 0.81, 0.86, 0.86, 0.88, 0.88};
  Double_t vb[nb] = {0.80, 0.83, 0.85, 0.84, 0.85};
  Double_t vc[nc] = {0.79, 0.82, 0.83, 0.83};
  gra = new TGraphErrors(na,xa,va);
  grb = new TGraph(nb,xb,vb);
  grc = new TGraph(nc,xc,vc);
  c1= new TCanvas("c","c", 0,0,600,600);
  c1->SetGrid();
  c1->SetFillColor(0);
  gra->SetTitle("Vtx Eff vs k");
  gra->GetYaxis()->SetTitle("Efficiency"); gra->GetXaxis()->SetTitle("k"); gra->SetLineColor(4); gra->SetMarkerColor(4);
  gra->Draw("AL*");
  grb->SetLineColor(3); grb->SetMarkerColor(3);
  grb->Draw("L*");
  grc->SetLineColor(6); grc->SetMarkerColor(6);
  grc->Draw("L*");
  TLegend *leg = new TLegend(0.15,0.68,0.3,0.85,NULL,"brNDC");
  leg->SetFillColor(0);
  TLegendEntry *entry=leg->AddEntry("gra","Vch > 0.17","lep");
  entry->SetLineColor(4); entry->SetMarkerColor(4);
  entry=leg->AddEntry("grb","Vch > 0.14","lep");
  entry->SetLineColor(3); entry->SetMarkerColor(3);
  entry=leg->AddEntry("grc","Vch > 0.11","lep");
  entry->SetLineColor(6); entry->SetMarkerColor(6);
  leg->Draw();

  if(endblock) goto end;

 config://///////////////////////////////////////////////////////////

  c->Clear(); c->Divide(1,2);
  plot_omega(homphi_mu_r,  homphi_mu_w,  "mu_distphi"); if(wait())return;
  plot_omega(homphi_kO_r,  homphi_kO_w,  "kO_distphi"); if(wait())return;
  plot_omega(homphi_kS_r,  homphi_kS_w,  "kS_distphi"); if(wait())return;

  c->cd();
  c->Clear(); c->Divide(1,2);
  c->cd(1); h82.DrawNormalized(); h82_3.SetLineColor(kRed); h82_3.DrawNormalized("same");
  c->cd(2); h83.DrawNormalized(); h83_3.SetLineColor(kRed); h83_3.DrawNormalized("same");
  if(wait(c, "output/conf_2.gif"))return;
  c->Clear(); c->Divide(1,2);
  c->cd(1); h84.DrawNormalized(); h84_3.SetLineColor(kRed); h84_3.DrawNormalized("same");
  c->cd(2); h85.DrawNormalized(); h85_3.SetLineColor(kRed); h85_3.DrawNormalized("same");
  if(wait(c, "output/conf_3.gif"))return;
  c->Clear(); c->Divide(1,2);
  c->cd(1); h88.DrawNormalized(); h88_3.SetLineColor(kRed); h88_3.DrawNormalized("same");
  c->cd(2); h89.DrawNormalized(); h89_3.SetLineColor(kRed); h89_3.DrawNormalized("same");
  if(wait(c, "output/conf_4.gif"))return;



 pid: /////////////////////////////////////////////////////////////

  cout<<"plotting at pid"<<endl;
  plotPID("muon",       h1011, h1001, h1021); if(wait())return;
  plotPID("electron",   h1010, h1000, h1020); if(wait())return;
  plotPID("kaon",       h1012, h1002, h1022); if(wait())return;
  plotPID("kaon-proton",h1015, h1005, h1025); if(wait())return;
  plotPID("kaonsame",   h1013, h1003, h1023); if(wait())return;
  plotPID("pion",       h1014, h1004, h1024); 

  if(endblock) goto end;  
 
 general://///////////////////////////////////////////////////////////

  cout<<"plotting at general"<<endl;
  c->Clear(); c->Divide(2,2); gPad->SetBorderMode(0);
  gStyle->SetOptStat(1111);
  c->cd(1); h1->Draw();
  c->cd(2); h6->Draw();
  c->cd(3); h2->Draw();
  c->cd(4); h3->Draw();
  if(wait())return;

  c->Clear(); c->Divide(1,2); gPad->SetBorderMode(0);
  if(wait())return;
  c->cd(2); h4->Draw();

  if(endblock) goto end;

 nnet: /////////////////////////////////////////////////////////////
  cout<<"plotting at nnet"<<endl;
  c->Clear(); c->Divide(1,2);
  gStyle->SetOptStat(0);

  //  plot_omega(h100,  h102,  "NNet");         if(wait())return;
  //  plot_omega(h103,  h104,  "NNet_omega",1); if(wait())return;
  plot_omega(h613,  h513,  "mu", 3);           if(wait())return;
  plot_omega(h613p, h513p, "mu_omega", 1);  if(wait())return;
  plot_omega(h611,  h511,  "e", 2);            if(wait())return;
  plot_omega(h611p, h511p, "e_omega", 1);   if(wait())return;
  plot_omega(h6321, h5321,  "k", 3);           if(wait())return;
  plot_omega(h6321p,h5321p, "k_omega", 1);  if(wait())return;
  plot_omega(h535,  h635, "sameside", 2);      if(wait())return;
  plot_omega(h535p, h635p,"sameside_omega",1); if(wait())return;
  plot_omega(h540,  h640,  "vtx", 1);       if(wait())return;
  plot_omega(h540p, h640p, "vtx_omega", 1); 
  if(endblock) goto end;

 prob_min: /////////////////////////////////////////////////////////////
  goto probmincomb;
  plotEffectiveEff(hr_mu_prob, hw_mu_prob);                  if(wait())return;
  plotEffectiveEff(hr_mu_probComb, hw_mu_probComb);          if(wait())return;
  plotEffectiveEff(hr_ele_prob, hw_ele_prob);                if(wait())return;
  plotEffectiveEff(hr_ele_probComb, hw_ele_probComb);        if(wait())return;
  plotEffectiveEff(hr_k_prob, hw_k_prob);                    if(wait())return;
  plotEffectiveEff(hr_k_probComb, hw_k_probComb);            if(wait())return;
  plotEffectiveEff(hr_kS_prob, hw_kS_prob);                  if(wait())return;
  plotEffectiveEff(hr_kS_probComb, hw_kS_probComb);          if(wait())return;
  plotEffectiveEff(hr_vtx_prob, hw_vtx_prob);                if(wait())return;
  plotEffectiveEff(hr_vtx_probComb, hw_vtx_probComb);        if(wait())return;
 probmincomb:
  plotEffectiveEff(hr_probComb_OS, hw_probComb_OS);          if(wait())return;
  plotEffectiveEff(hr_probComb, hw_probComb);                if(wait())return;

 effeff_cut: /////////////////////////////////////////////////////////////
  cout<<"plotting at effeff_cut"<<endl;
  h1->Draw(); //changes dir (stupid root feature)
  //  goto angle;
  //  goto pionSpt;
  //  goto cand;
  goto tagmu;
  goto tagele;
  goto tagk;
  goto tagss;

 angle:   
  plotEffectiveEff(hr_mu_ippu, hw_mu_ippu);                     if(wait())return;
  plotEffectiveEff(homphi_mu_r, homphi_mu_w);                   if(wait())return;
  plotEffectiveEff(hr_mu_theta, hw_mu_theta);                   if(wait())return;
  plotEffectiveEff(hr_ele_ippu, hw_ele_ippu);                     if(wait())return;
  plotEffectiveEff(homphi_ele_r, homphi_ele_w);                   if(wait())return;
  plotEffectiveEff(hr_ele_theta, hw_ele_theta);                   if(wait())return;
  plotEffectiveEff(hr_k_ippu, hw_k_ippu);                     if(wait())return;
  plotEffectiveEff(homphi_kO_r, homphi_kO_w);                 if(wait())return;
  plotEffectiveEff(hr_k_theta, hw_k_theta);                   if(wait())return;
  plotEffectiveEff(hr_kS_ippu, hw_kS_ippu);                     if(wait())return;
  plotEffectiveEff(homphi_kS_r, homphi_kS_w);                   if(wait())return;
  plotEffectiveEff(hr_kS_theta, hw_kS_theta);                   if(wait())return;
 pionSpt: 
  hcut_pS_cand->Draw(); if(wait())return;
  hcut_pt_1->DrawNormalized(); 
  hcut_pt_2->SetLineColor(kRed); hcut_pt_2->DrawNormalized("same"); 
  hcut_pt_3->SetLineColor(kGreen); hcut_pt_3->DrawNormalized("same"); 
  hcut_pt_4->SetLineColor(kBlue); hcut_pt_4->DrawNormalized("same"); 
  hcut_pt_5->SetLineColor(kYellow); hcut_pt_5->DrawNormalized("same"); 
  if(wait())return;
  plotEffectiveEff(hr_kS_pt, hw_kS_pt);   if(wait())return;
  plotEffectiveEff(hcut_pt_0_r, hcut_pt_0_w); if(wait())return;
  plotEffectiveEff(hcut_pt_1_r, hcut_pt_1_w); if(wait())return;
  plotEffectiveEff(hcut_pt_2_r, hcut_pt_2_w); if(wait())return;
  plotEffectiveEff(hcut_pt_3_r, hcut_pt_3_w); if(wait())return;
  plotEffectiveEff(hcut_pt_4_r, hcut_pt_4_w); if(wait())return;
  plotEffectiveEff(hcut_pt_5_r, hcut_pt_5_w); if(wait())return;
 cand:  
  hcut_mu_cand->Draw(); if(wait())return;
  hcut_ele_cand->Draw(); if(wait())return;
  hcut_ko_cand->Draw(); if(wait())return;
  hcut_kS_cand->Draw(); if(wait())return;
  hcut_pS_cand->Draw(); if(wait())return;
  
 tagmu:
  hcut_mu_cand->Draw(); if(wait())return;
  plotEffectiveEff(hr_mu_prob, hw_mu_prob);                     if(wait())return;
  plotEffectiveEff(hr_mu_p, hw_mu_p);                           if(wait())return;
  plotEffectiveEff(hr_mu_pt, hw_mu_pt);                         if(wait())return;
  plotEffectiveEff(hr_mu_ips, hw_mu_ips);                       if(wait())return;
  plotEffectiveEff(hr_mu_ipssign, hw_mu_ipssign);               if(wait())return;
  plotEffectiveEff(hr_mu_ipssign, hw_mu_ipssign, "right2left"); if(wait())return;
  plotEffectiveEff(hr_mu_ippu, hw_mu_ippu);                     if(wait())return;
  plotEffectiveEff(homphi_mu_r, homphi_mu_w);                   if(wait())return;
  plotEffectiveEff(hr_mu_theta, hw_mu_theta);                   if(wait())return;
  plotEffectiveEff(hr_mu_theta, hw_mu_theta, "right2left");     if(wait())return;
  plotEffectiveEff(hr_mu_lcs, hw_mu_lcs, "right2left");         if(wait())return;
  plotEffectiveEff(hr_mu_pid, hw_mu_pid);                       if(wait())return;
  plotEffectiveEff(hr_mu_tsal, hw_mu_tsal);                     if(wait())return;
  plotEffectiveEff(hr_mu_mult, hw_mu_mult, "right2left");       if(wait())return;
 tagele:
  hcut_ele_cand->Draw(); if(wait())return;
  plotEffectiveEff(hr_ele_prob, hw_ele_prob);                     if(wait())return;
  plotEffectiveEff(hr_ele_p, hw_ele_p);                           if(wait())return;
  plotEffectiveEff(hr_ele_pt, hw_ele_pt);                         if(wait())return;
  plotEffectiveEff(hr_ele_ips, hw_ele_ips);                       if(wait())return;
  plotEffectiveEff(hr_ele_ipssign, hw_ele_ipssign);               if(wait())return;
  plotEffectiveEff(hr_ele_ipssign1, hw_ele_ipssign1, "right2left"); if(wait())return;
  plotEffectiveEff(hr_ele_ippu, hw_ele_ippu);                     if(wait())return;
  plotEffectiveEff(homphi_ele_r, homphi_ele_w);                   if(wait())return;
  plotEffectiveEff(hr_ele_theta, hw_ele_theta);                   if(wait())return;
  plotEffectiveEff(hr_ele_lcs, hw_ele_lcs, "right2left");         if(wait())return;
  plotEffectiveEff(hr_ele_pid, hw_ele_pid);                       if(wait())return;
  plotEffectiveEff(hr_ele_tsal, hw_ele_tsal);                     if(wait())return;
  plotEffectiveEff(hr_ele_mult, hw_ele_mult,"right2left");        if(wait())return;
  plotEffectiveEff(hr_ele_veloch, hw_ele_veloch);                 if(wait())return;
  plotEffectiveEff(hr_ele_veloch, hw_ele_veloch, "right2left");   if(wait())return;
  plotEffectiveEff(hr_ele_eOverP, hw_ele_eOverP);                 if(wait())return;
  plotEffectiveEff(hr_ele_eOverP, hw_ele_eOverP, "right2left");   if(wait())return;
 tagk:
  hcut_ko_cand->Draw(); if(wait())return;
  plotEffectiveEff(hr_k_prob, hw_k_prob);                     if(wait())return;
  plotEffectiveEff(hr_k_p, hw_k_p);                           if(wait())return;
  plotEffectiveEff(hr_k_pt, hw_k_pt);                         if(wait())return;
  plotEffectiveEff(hr_k_ips, hw_k_ips);                       if(wait())return;
  plotEffectiveEff(hr_k_ip, hw_k_ip, "right2left");           if(wait())return;
  plotEffectiveEff(hr_k_ipssign, hw_k_ipssign);               if(wait())return;
  plotEffectiveEff(hr_k_ipssign1, hw_k_ipssign1, "right2left"); if(wait())return;
  plotEffectiveEff(hr_k_ippu, hw_k_ippu);                     if(wait())return;
  plotEffectiveEff(homphi_kO_r, homphi_kO_w);                 if(wait())return;
  plotEffectiveEff(hr_k_theta, hw_k_theta);                   if(wait())return;
  plotEffectiveEff(hr_k_lcs, hw_k_lcs, "right2left");         if(wait())return;
  plotEffectiveEff(hr_k_pid, hw_k_pid);                       if(wait())return;
  plotEffectiveEff(hr_k_pidkp, hw_k_pidkp);                   if(wait())return;
  plotEffectiveEff(hr_k_tsal, hw_k_tsal);                     if(wait())return;
  plotEffectiveEff(hr_k_mult, hw_k_mult, "right2left");       if(wait())return;
 tagss: //pions and kaons same side
  hcut_kS_cand->Draw(); if(wait())return;
  hcut_pS_cand->Draw(); if(wait())return;
  plotEffectiveEff(hr_kS_prob, hw_kS_prob);                       if(wait())return;
  plotEffectiveEff(hr_kS_p, hw_kS_p);                             if(wait())return;
  plotEffectiveEff(hr_kS_pt, hw_kS_pt);                           if(wait())return;
  plotEffectiveEff(hr_kS_ips, hw_kS_ips, "right2left");           if(wait())return;
  plotEffectiveEff(hr_kS_ipssign, hw_kS_ipssign);                 if(wait())return;
  plotEffectiveEff(hr_kS_ipssign, hw_kS_ipssign, "right2left");   if(wait())return;
  plotEffectiveEff(hr_kS_ipssign1, hw_kS_ipssign1);                 if(wait())return;
  plotEffectiveEff(hr_kS_ipssign1, hw_kS_ipssign1, "right2left");   if(wait())return;
  plotEffectiveEff(hr_kS_ipsign, hw_kS_ipsign);                   if(wait())return;
  plotEffectiveEff(hr_kS_ipsign, hw_kS_ipsign, "right2left");     if(wait())return;
  plotEffectiveEff(hr_kS_ippu, hw_kS_ippu);                       if(wait())return;
  plotEffectiveEff(homphi_kS_r, homphi_kS_w);                     if(wait())return;
  plotEffectiveEff(hr_kS_theta, hw_kS_theta);                     if(wait())return;
  plotEffectiveEff(hr_kS_lcs, hw_kS_lcs, "right2left");           if(wait())return;
  plotEffectiveEff(hr_kS_pid, hw_kS_pid, "right2left");           if(wait())return;
  plotEffectiveEff(hr_kS_pid, hw_kS_pid);           if(wait())return;
  plotEffectiveEff(hr_kS_pidp, hw_kS_pidp, "right2left");         if(wait())return;
  plotEffectiveEff(hr_kS_pidp, hw_kS_pidp);         if(wait())return;
  plotEffectiveEff(hr_kS_tsal, hw_kS_tsal);                       if(wait())return;
  plotEffectiveEff(hr_kS_mult, hw_kS_mult, "right2left");         if(wait())return;
  plotEffectiveEff(hr_kS_deta, hw_kS_deta);                       if(wait())return;
  plotEffectiveEff(hr_kS_deta, hw_kS_deta, "right2left");         if(wait())return;
  plotEffectiveEff(hr_kS_fabsdeta, hw_kS_fabsdeta, "right2left"); if(wait())return;
  plotEffectiveEff(hr_kS_dphi, hw_kS_dphi, "right2left");         if(wait())return;
  plotEffectiveEff(hr_kS_dr, hw_kS_dr, "right2left");             if(wait())return;
  plotEffectiveEff(hr_kS_dq, hw_kS_dq, "right2left");             if(wait())return;
  plotEffectiveEff(hr_kS_dqe, hw_kS_dqe);              if(wait())return;

  if(endblock) goto end;

 asymm: /////////////////////////////////////////////////////////////

  cout<<"plotting at asymm"<<endl;
  TF1* cosn = new TF1("cosn","(1-2*[1])*cos([0]*x)",0.0,10.0);
  cosn->FixParameter( 0 , 0.5 );
  cosn->SetParameters(0.5, 0.35);
  cosn->SetParNames("Dm","Omega");
  cosn->SetLineColor(2); cosn->SetLineWidth(2);

  c->Clear();
  cosn->SetParameters(0.52, 0.1);
  TGraphErrors* ttrue = asymmetry(h4061, h4062, "ttrue");
  if( ttrue ) {
    ttrue->SetTitle("True; time/ps; Asymmetry"); 
    ttrue->Draw("AP"); ttrue->Fit("cosn");
    cout<<"all"<<endl;
    cout<<"delta:"<<cosn->GetParameter(0)<<" +- "<<cosn->GetParError(0)<<endl;
    cout<<"omega:"<<cosn->GetParameter(1)<<" +- "<<cosn->GetParError(1)<<endl;
    cout<<"Dilution:"<<1-(2*cosn->GetParameter(1))<<" D^2: "<<pow(1-(2*cosn->GetParameter(1)),2)<<endl;
    c->Print("output/Truetime.gif");
    if(wait())return;
  }

  TCanvas* c = new TCanvas("c","Tagging", 0, 0, 500, 750);
  c->SetBorderMode(0);
  c->SetFrameBorderMode(0);
  c->SetFillColor(10);
  c->Clear();
  c->Divide(2,3);
  gPad->SetBorderMode(0);
  c->cd(1);
  cosn->SetParameters(0.5, 0.35);
  TGraphErrors* tmu = asymmetry(h2011, h2012, "tmu");
  if(tmu){tmu->SetTitle("MUON; time/ps; Asymmetry"); tmu->Draw("AP"); tmu->Fit("cosn");}
  cout<<"muon"<<endl;
  cout<<"delta:"<<cosn->GetParameter(0)<<" +- "<<cosn->GetParError(0)<<endl;
  cout<<"omega:"<<cosn->GetParameter(1)<<" +- "<<cosn->GetParError(1)<<endl;
  cout<<"Dilution:"<<1-(2*cosn->GetParameter(1))<<" D^2: "<<pow(1-(2*cosn->GetParameter(1)),2)<<endl;
  c->cd(2);
  cosn->SetParameters(0.5, 0.35);
  TGraphErrors* tel = asymmetry(h2021, h2022, "tel");
  if(tel){tel->SetTitle("ELEC; time/ps; Asymmetry"); tel->Draw("AP"); tel->Fit("cosn");}
  cout<<"ele"<<endl;  
  cout<<"delta:"<<cosn->GetParameter(0)<<" +- "<<cosn->GetParError(0)<<endl;
  cout<<"omega:"<<cosn->GetParameter(1)<<" +- "<<cosn->GetParError(1)<<endl;
  cout<<"Dilution:"<<1-(2*cosn->GetParameter(1))<<" D^2: "<<pow(1-(2*cosn->GetParameter(1)),2)<<endl;  
  c->cd(3);
  cosn->SetParameters(0.5, 0.35);
  TGraphErrors* tko = asymmetry(h2031, h2032, "tko");
  if(tko){tko->SetTitle("KOPP; time/ps; Asymmetry"); tko->Draw("AP"); tko->Fit("cosn");}
  cout<<"kaon"<<endl;
  cout<<"delta:"<<cosn->GetParameter(0)<<" +- "<<cosn->GetParError(0)<<endl;
  cout<<"omega:"<<cosn->GetParameter(1)<<" +- "<<cosn->GetParError(1)<<endl;
  cout<<"Dilution:"<<1-(2*cosn->GetParameter(1))<<" D^2: "<<pow(1-(2*cosn->GetParameter(1)),2)<<endl;  
  c->cd(4);
  cosn->SetParameters(0.5, 0.35);
  TGraphErrors* tks = asymmetry(h2041, h2042, "tks");
  if(tks){tks->SetTitle("PSAME;time/ps; Asymmetry"); tks->Draw("AP"); tks->Fit("cosn");}
  cout<<"ss"<<endl;
  cout<<"delta:"<<cosn->GetParameter(0)<<" +- "<<cosn->GetParError(0)<<endl;
  cout<<"omega:"<<cosn->GetParameter(1)<<" +- "<<cosn->GetParError(1)<<endl;    
  cout<<"Dilution:"<<1-(2*cosn->GetParameter(1))<<" D^2: "<<pow(1-(2*cosn->GetParameter(1)),2)<<endl;  
  c->cd(5);
  cosn->SetParameters(0.5, 0.35);
  TGraphErrors* tvtx = asymmetry(h2051, h2052, "tvtx");
  if(tvtx){tvtx->SetTitle("VERTEX;time/ps; Asymmetry");tvtx->Draw("AP");tvtx->Fit("cosn");}
  cout<<"vtx"<<endl;
  cout<<"delta:"<<cosn->GetParameter(0)<<" +- "<<cosn->GetParError(0)<<endl;
  cout<<"omega:"<<cosn->GetParameter(1)<<" +- "<<cosn->GetParError(1)<<endl;    
  cout<<"Dilution:"<<1-(2*cosn->GetParameter(1))<<" D^2: "<<pow(1-(2*cosn->GetParameter(1)),2)<<endl;  
  c->Print("output/Asymmetry1.gif"); c->cd();
  if(wait())return;

  c->Clear();c->Divide(2,3);
  c->cd(1);
  cosn->SetParameters(0.5, 0.35);
  TGraphErrors* tc5 = asymmetry(h3051, h3052, "tc5");
  if(tc5){tc5->SetTitle("CAT5; time/ps; Asymmetry"); tc5->Draw("AP"); tc5->Fit("cosn");}
  cout<<"cat5"<<endl;
  cout<<"delta:"<<cosn->GetParameter(0)<<" +- "<<cosn->GetParError(0)<<endl;
  cout<<"omega:"<<cosn->GetParameter(1)<<" +- "<<cosn->GetParError(1)<<endl;    
  cout<<"Dilution:"<<1-(2*cosn->GetParameter(1))<<" D^2: "<<pow(1-(2*cosn->GetParameter(1)),2)<<endl;  
  c->cd(2);
  cosn->SetParameters(0.5, 0.35);
  TGraphErrors* tc4 = asymmetry(h3041, h3042, "tc4");
  if(tc4){tc4->SetTitle("CAT4; time/ps; Asymmetry"); tc4->Draw("AP"); tc4->Fit("cosn");}
  cout<<"cat4"<<endl;
  cout<<"delta:"<<cosn->GetParameter(0)<<" +- "<<cosn->GetParError(0)<<endl;
  cout<<"omega:"<<cosn->GetParameter(1)<<" +- "<<cosn->GetParError(1)<<endl;    
  cout<<"Dilution:"<<1-(2*cosn->GetParameter(1))<<" D^2: "<<pow(1-(2*cosn->GetParameter(1)),2)<<endl;  
  c->cd(3);
  cosn->SetParameters(0.5, 0.35);
  TGraphErrors* tc3 = asymmetry(h3031, h3032, "tc3");
  if(tc3){tc3->SetTitle("CAT3; time/ps; Asymmetry"); tc3->Draw("AP"); tc3->Fit("cosn");}
  cout<<"cat3"<<endl;
  cout<<"delta:"<<cosn->GetParameter(0)<<" +- "<<cosn->GetParError(0)<<endl;
  cout<<"omega:"<<cosn->GetParameter(1)<<" +- "<<cosn->GetParError(1)<<endl;    
  cout<<"Dilution:"<<1-(2*cosn->GetParameter(1))<<" D^2: "<<pow(1-(2*cosn->GetParameter(1)),2)<<endl;  
  c->cd(4);
  cosn->SetParameters(0.5, 0.35);
  TGraphErrors* tc2 = asymmetry(h3021, h3022, "tc2");
  if(tc2){tc2->SetTitle("CAT2; time/ps; Asymmetry"); tc2->Draw("AP"); tc2->Fit("cosn");}
  cout<<"cat2"<<endl;
  cout<<"delta:"<<cosn->GetParameter(0)<<" +- "<<cosn->GetParError(0)<<endl;
  cout<<"omega:"<<cosn->GetParameter(1)<<" +- "<<cosn->GetParError(1)<<endl;    
  cout<<"Dilution:"<<1-(2*cosn->GetParameter(1))<<" D^2: "<<pow(1-(2*cosn->GetParameter(1)),2)<<endl;  
  c->cd(5);
  cosn->SetParameters(0.5, 0.35);
  TGraphErrors* tc1 = asymmetry(h3011, h3012, "tc1");
  if(tc1){tc1->SetTitle("CAT1; time/ps; Asymmetry"); tc1->Draw("AP"); tc1->Fit("cosn");}
  cout<<"cat1"<<endl;
  cout<<"delta:"<<cosn->GetParameter(0)<<" +- "<<cosn->GetParError(0)<<endl;
  cout<<"omega:"<<cosn->GetParameter(1)<<" +- "<<cosn->GetParError(1)<<endl;    
  cout<<"Dilution:"<<1-(2*cosn->GetParameter(1))<<" D^2: "<<pow(1-(2*cosn->GetParameter(1)),2)<<endl;  
  c->cd(6);
  cosn->SetParameters(0.5, 0.35);
  TGraphErrors* tc = asymmetry(h3061, h3062, "tc");
  if(tc ){tc->SetTitle("ALL CAT;time/ps; Asymmetry"); tc->Draw("AP"); tc->Fit("cosn");}
  cout<<"all cat"<<endl;
  cout<<"delta:"<<cosn->GetParameter(0)<<" +- "<<cosn->GetParError(0)<<endl;
  cout<<"omega:"<<cosn->GetParameter(1)<<" +- "<<cosn->GetParError(1)<<endl;    
  cout<<"Dilution:"<<1-(2*cosn->GetParameter(1))<<" D^2: "<<pow(1-(2*cosn->GetParameter(1)),2)<<endl;  
  c->cd();c->Print("output/Asymmetry.gif");

  if(endblock) goto end;

 multipleinteraction: /////////////////////////////////////////////////////////////

  cout<<"plotting pointing to right primary vertex"<<endl;
  plotEffectiveEff(hpv_gippu_r, hpv_gippu_w           ); if(wait())return;  
  plotEffectiveEff(hpv_difip_r, hpv_difip_w, "right2left"); if(wait())return;
  plotEffectiveEff(hpv_ipmeandif_r, hpv_ipmeandif_w, "right2left" ); if(wait())return;
  plotEffectiveEff(hpv_zposdif_r, hpv_zposdif_w, "right2left"); if(wait())return;
  plotEffectiveEff(hpv_zposabsdif_r, hpv_zposabsdif_w       ); if(wait())return;

  if(endblock) goto end;

 secondaryvertex: /////////////////////////////////////////////////////////////
  
  cout<<"SV studies"<<endl;
  cout<<" "<<endl;
  cout<<"kaons/leptons"<<endl;
  c->Clear(); c->Divide(1,3);
  c->cd(1); hsv100->Draw();//kaons
  c->cd(2); hsv101->Draw();//leptons
  c->cd(3); hsv102->Draw();//leptons or kaons
  if(wait())return;
  plot_Eff_Omega(hsv100_r, hsv100_w); if(wait())return;
  plot_Eff_Omega(hsv101_r, hsv101_w); if(wait())return;
  plot_Eff_Omega(hsv102_r, hsv102_w); if(wait())return;

  cout<<"PU, tracks in PU, z pos of PU, activity of PU"<<endl;
  c->Clear(); c->Divide(2,2);
  c->cd(1);hsv500->Draw();//PU
  c->cd(2);hsv501->Draw();//tracks in PU
  c->cd(3);hsv502->Draw();//z pos of PU
  c->cd(4);hsv503->Draw();//activity
  if(wait())return;
  hsv500_r->DrawNormalized(); hsv500_w->SetLineColor(2); hsv500_w->DrawNormalized("same"); if(wait())return;
  plot_Eff_Omega(hsv500_r, hsv500_w);   if(wait())return;
  hsv501_r->DrawNormalized(); hsv501_w->SetLineColor(2); hsv501_w->DrawNormalized("same"); if(wait())return;
  plot_Eff_Omega(hsv501_r, hsv501_w);   if(wait())return;
  hsv503_r->DrawNormalized(); hsv503_w->SetLineColor(2); hsv503_w->DrawNormalized("same"); if(wait())return;
  plot_Eff_Omega(hsv503_r, hsv503_w);   if(wait())return;
  
  cout<<"DeltaPhi/DeltaTheta"<<endl;
  c->Clear(); c->Divide(1,2);
  c->cd(1); hsv200->Draw();//DeltaPhi
  c->cd(2); hsv201->Draw();//DeltaTheta
  if(wait())return;
  hsv200->Draw(); hsv200_g->SetLineColor(3); hsv200_g->Draw("same"); if(wait())return;
  hsv201->Draw(); hsv201_g->SetLineColor(3); hsv201_g->Draw("same"); if(wait())return;
  plotEffectiveEff(hsv200_r, hsv200_w, "right2left");   if(wait())return;
  plotEffectiveEff(hsv201_r, hsv201_w, "right2left");   if(wait())return;
  plotEffectiveEff(hsv201_r, hsv201_w);   if(wait())return;
  
  cout<<"resolution all/good P tau DeltaZ"<<endl;
  c->Clear(); c->Divide(2,3);//resolution for P, tau, DeltaZ
  c->cd(1); hsv301->Draw(); hsv301->Fit("gaus","same"); 
  c->cd(2); hsv301_g->SetLineColor(3); hsv301_g->Draw(); hsv301_g->Fit("gaus","same");
  c->cd(3); hsv302->Draw(); hsv302->Fit("gaus","same"); 
  c->cd(4); hsv302_g->SetLineColor(3); hsv302_g->Draw(); hsv302_g->Fit("gaus","same");
  c->cd(5); hsv303->Draw(); hsv303->Fit("gaus","same"); 
  c->cd(6); hsv303_g->SetLineColor(3); hsv303_g->Draw(); hsv303_g->Fit("gaus","same");
  if(wait())return;

  cout<<"reco of momentum of BO for tau"<<endl;
  c->Clear(); c->Divide(2,2);//For the reconstruction of tau
  c->cd(1); hsv310_b->Draw(); hsv310_g->SetMarkerColor(3); hsv310_g->Draw("same");
  c->cd(2); hsv311_b->Draw(); hsv311_g->SetLineColor(3); hsv311_g->Draw("same");
  c->cd(3); hsv312_b->Draw(); hsv312_g->SetMarkerColor(3); hsv312_g->Draw("same");
  c->cd(4); hsv313_b->Draw(); hsv313_g->SetLineColor(3); hsv313_g->Draw("same");
  if(wait())return;
  hsv312_g->Draw(); hsv312_g->Fit("pol1"); if(wait())return; //pol for reconstruct BO momentum

  //tau
  cout<<"tau"<<endl;
  plot_Eff_Omega(hsv320_r, hsv320_w, "right2left");   if(wait())return;//no kaons
  plot_Eff_Omega(hsv321_r, hsv321_w, "right2left");   if(wait())return;//Bs
  plot_Eff_Omega(hsv322_r, hsv322_w, "right2left");   if(wait())return;//B0
  plot_Eff_Omega(hsv323_r, hsv323_w, "right2left");   if(wait())return;//B+-
  plot_Eff_Omega(hsv300_r, hsv300_w, "right2left");   if(wait())return;//tau

  cout<<"SV with PU studies"<<endl;

  cout<<"z pos, mag, zdif SV-PV, mag dif SV-PV"<<endl;
  c->Clear(); c->Divide(2,2);
  c->cd(1); hsv401->Draw();
  c->cd(2); hsv421->Draw();
  c->cd(3); hsv402->Draw();
  c->cd(4); hsv422->Draw();
  if(wait())return;
  cout<<"sv closer to PU or to PV (z pos, mag)"<<endl;
  cout<<"xy pos"<<endl;
  cout<<"ip wrt PU"<<endl;
  c->Clear(); c->Divide(2,2);
  c->cd(1); hsv403->Draw();
  c->cd(2); hsv423->Draw();
  c->cd(3); hsv404_w->Draw(); hsv404_r->SetMarkerColor(3); hsv404_r->Draw("same");
  c->cd(4); hsv405->Draw();
  if(wait())return;

  cout<<"zdif SV-PV, SV closer to PU or to PV, ip PU"<<endl;
  plot_Eff_Omega(hsv402_r, hsv402_w);   if(wait())return;
  plot_Eff_Omega(hsv403_r, hsv403_w);   if(wait())return;
  plot_Eff_Omega(hsv405_r, hsv405_w);   if(wait())return;

  if(endblock) goto end;

 correlations: /////////////////////////////////////////////////////////////
  goto correNN;
  //correlations of taggers
  c->Clear(); c->Divide(2,5);
  c->cd(1); eta_corr_mu_ele->Draw("zcol");
  c->cd(2); eta_corr_mu_ko->Draw("zcol");
  c->cd(3); eta_corr_mu_sP->Draw("zcol");
  c->cd(4); eta_corr_mu_vtx->Draw("zcol");
  c->cd(5); eta_corr_ele_ko->Draw("zcol");
  c->cd(6); eta_corr_ele_sP->Draw("zcol");
  c->cd(7); eta_corr_ele_vtx->Draw("zcol");
  c->cd(8); eta_corr_ko_sP->Draw("zcol");
  c->cd(9); eta_corr_ko_vtx->Draw("zcol");
  c->cd(10); eta_corr_vtx_sP->Draw("zcol");
  if(wait())return;

  if(endblock) goto end;

 correNN:
  //correlations of the variables in NN
  TString f1 = "output/tag_1.root"; //nero data
  TString f2 = "output/tag_2.root"; //rosso mc
  TString title_mu[10] = {"","B_pT","muon p","muon pT ","IPsign"," "," "," ","pileup "," "};
  plot_correlations(f1,f2,"hcut_mu_","Correlations for muon",title_mu);
  if(wait())return;
  TString title_ele[10] = {"","B_pT","ele p","ele pT ","IPsign"," "," "," ","pileup "," "};
  plot_correlations(f1,f2,"hcut_ele_","Correlations for electron",title_ele);
  if(wait())return;
  TString title_kaon[10] = {"vtags.size()","B_pT","kaon p","kaon pT ","IPsign","deta","dphi","dQ","pileup","ncand"};
  plot_correlations(f1,f2,"hcut_ko_","Correlations for opposite kaon",title_kaon);
  if(wait())return;  
  TString title_pion[10] = {"vtags.size()","B_pT","","pion pT ","IPs","dR","","dQ","pileup",""};
  plot_correlations(f1,f2,"hcut_pS_","Correlations for same side pion",title_pion);
  if(wait())return;  
  TString title_vertex[10] = {"part.size()","pileup","B pT","vflagged ","Vptmean","Vipsmean","","","fabs(Vch)","SVM"};
  plot_correlations(f1,f2,"hcut_vtx_","Correlations for vertex charge",title_vertex);
  if(wait())return;  

  if(endblock) goto end;

 end: /////////////////////////////////////////////////////////////

}
