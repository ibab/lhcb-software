{
  /*-----------------------------------------------------------
   *---  DRAW PLOTS -------------------------------------------
   *-----------------------------------------------------------*/
  gROOT->Reset();
  gROOT->ProcessLine(".L plot.h+"); //load some functions defined there
  TFile fhisto("output/tag.root");

  TCanvas* c = new TCanvas("c","Tagging", 0, 0, 600, 600);
  c->SetBorderMode(0);
  c->SetFrameBorderMode(0);
  c->SetFillColor(10);
  gStyle->SetDrawBorder(0);
  gStyle->SetOptFit(1);
  gStyle->SetOptStat(0);
  gStyle->SetTitleFillColor(5);

  bool endblock = 1;
  //  goto taggercandidates;
  //  goto general;
  //  goto asymm;
  //  goto nnet;
  //  goto vertex;
  //  goto omegapt;
  //  goto addseed;
  goto vtxcharge;
  //  goto config;  
  //  goto pid;
  //  goto effeff_cut;
  //  goto secondaryvertex;
  //  goto multipleinteraction;
  goto omegataggers;

omegataggers:///////////////////////////////////////////////////////
  cout<<"Plot omega for taggers"<<endl;
  hom_combOSSS->SetTitle("Combination of taggers");
  hom_combOSSS->GetXaxis()->SetTitle("Omega");
  hom_combOSSS->GetYaxis()->SetTitle("Events");
  hom_combOSSS->SetLineWidth(3);   hom_combOSSS->SetLineColor(1);   hom_combOSSS->Draw();  
  hom_combOS->SetLineWidth(3); hom_combOS->SetLineColor(2); hom_combOS->Draw("same");
  hom_ss->SetLineColor(6);       hom_ss->Draw("same");
  hom_k->SetLineColor(5);        hom_k->Draw("same");
  hom_e->SetLineColor(4);        hom_e->Draw("same");
  hom_mu->SetLineColor(3);       hom_mu->Draw("same");
  hom_vtx->SetLineColor(7);      hom_vtx->Draw("same");
  //categories
  l1 = new TLine(0.17,0, 0.17, 3500);
  l2 = new TLine(0.24,0, 0.24, 3500);
  l3 = new TLine(0.31,0, 0.31, 3500);
  l4 = new TLine(0.38,0, 0.38, 3500);
  l1->Draw("same");  l2->Draw("same");  l3->Draw("same");  l4->Draw("same");
  TLegend *leg = new TLegend(0.7483221,0.7027972,0.8892617,0.8916084,NULL,"brNDC");
  TLegendEntry *entry=leg->AddEntry("hom_combOSSS","OS + SS","l");
  entry=leg->AddEntry("hom_combOS","OS","l");
  entry=leg->AddEntry("hom_mu","muon","l");
  entry=leg->AddEntry("hom_e","ele","l");
  entry=leg->AddEntry("hom_k","kaon","l");
  entry=leg->AddEntry("hom_ss","sameside","l");
  entry=leg->AddEntry("hom_vtx","vtx","l");
  leg->Draw();
  goto end;

taggercandidates:////////////////////////////////////////////////////

  cout<<"plotting at taggercandidates"<<endl;

  //compare same plots on two different root files
  TString f1 = "output/tag_s10.root";  //
  TString f2 = "output/tag_s12.root"; //in red

  c->Divide(2,2);
  c->cd(1); plot_taggercut(f1,f2,"h1");
  c->cd(2); plot_taggercut(f1,f2,"h2");
  c->cd(3); plot_taggercut(f1,f2,"h6");
  c->cd(4); plot_taggercut(f1,f2,"h4");
  c->Print("output/hcut_B.gif"); if(wait())return;

  c->cd(1); plot_taggercut(f1,f2,"hcut_mu_pid");
  c->cd(2); plot_taggercut(f1,f2,"hcut_mu_pt");
  c->cd(3); plot_taggercut(f1,f2,"hcut_mu_p");
  c->cd(4); plot_taggercut(f1,f2,"hcut_mu_lcs");
  c->Print("output/hcut_mu1.gif"); if(wait())return;
  c->cd(1); plot_taggercut(f1,f2,"hcut_mu_ippu");
  c->cd(2); plot_taggercut(f1,f2,"hcut_mu_N");
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

 omegapt://///////////////////////////////////////////////////////////
  cout<<"plotting at omegapt"<<endl;
  c->Clear(); c->Divide(1,2);
  h6w->Draw();
  plot_omega(h6w,    h6r,    "PtB");     if(wait())return;
  plot_omega(h6rtis, h6wtis, "PtB_tis"); if(wait())return;
  plot_omega(h6rtos, h6wtos, "PtB_tos"); if(wait())return;
  plot_omega(h6rtistos, h6wtistos,  "PtB_tistos"); 
  if(endblock) goto end;

 vertex://///////////////////////////////////////////////////////////
  cout<<"plotting at vertex"<<endl;
  plot_ratio("hvmass", hvmass2 , hvmass0, hvmass0);    if(wait())return;
  plot_ratio("hvreson", hvreson2 , hvreson0, hvreson0);if(wait())return;   
  plot_ratio("probf",hvprobf2,hvprobf0,hvprobf0 );           if(wait())return;
  plot_ratio("maxprobf",hmaxprobf2, hmaxprobf0, hmaxprobf0); if(wait())return;
  plot_ratio("delta_r",hz2, hz0, hz0);                 if(wait())return;
  plot_ratio("chi2", hvChi2 , hvChi0, hvChi0);         if(wait())return;
  plot_ratio("ptmin", hvpmin_2 ,hvpmin_0 , hvpmin_0);  if(wait())return;
  plot_ratio("ipmax", hvipmax2 ,hvipmax0 , hvipmax0);  if(wait())return;
  plot_ratio("ipsmin", hvipsmin2 ,hvipsmin0 ,hvipsmin0); if(wait())return;
  plot_ratio("deltaphi",hvdphi2  , hvdphi0, hvdphi0);  
  if(endblock) goto end;

 addseed://///////////////////////////////////////////////////////////
  //   cout<<"plotting at addseed"<<endl;
  //   plot_ratio("pt",htr_pt1, htr_pt0, htr_ptd );          if(wait())return;
  //   plot_ratio("mass",htr_mass1, htr_mass0, htr_massd );  if(wait())return;
  //   plot_ratio("ip",htr_ip1, htr_ip0, htr_ipd );          if(wait())return;
  //   plot_ratio("ips",htr_ips1, htr_ips0, htr_ipsd );      if(wait())return;
  //   plot_ratio("ipsv",htr_ipsv1, htr_ipsv0, htr_ipsvd );  if(wait())return;
  //   plot_ratio("ipsvs",htr_ipsvs1, htr_ipsvs0, htr_ipsvsd ); if(wait())return;
  //   plot_ratio("doca",htr_DOCA1, htr_DOCA0, htr_DOCAd );  if(wait())return;
  //   plot_ratio("deltaphi",htr_deltaphi1, htr_deltaphi0, htr_deltaphid );
  if(endblock) goto end;

 vtxcharge://///////////////////////////////////////////////////////////
  cout<<"plotting at vtxcharge"<<endl;
  //make sure tVch->setcharge( Vch ); in TaggerVertex and put MinVCh to 0!
  hvtxch_r->SetLineColor(kGreen); hvtxch_r->Draw();
  hvtxch_w->SetLineColor(kRed); hvtxch_w->Draw("same");
  //  hvtxch_u->SetLineColor(kBlue); hvtxch_u->Draw("same");
  c->Print("output/hvtxch.gif"); if(wait())return;
  plot_Eff_Omega(hvtxch_r, hvtxch_w); if(wait())return;
  plotEffectiveEff(hvtxch_r, hvtxch_w); if(wait())return;

  //  c->Clear(); c->Divide(1,3);
  //  c.cd(1); hvtx_pullz.Draw(); hvtx_pullz2.SetLineColor(3); hvtx_pullz2.Draw("same");
  //  hvtx_pullzd.SetLineStyle(2);hvtx_pullzd.SetLineColor(4); hvtx_pullzd.Draw("same");
  //  c.cd(2); hvtx_pullf.Draw();
  //  c.cd(3); hvtx_pulltheta.Draw();c.cd(); if(wait())return;
  
  //  plot_ratio("vertexcharge", hvtxch1, hvtxch0, hvtxchb);  if(wait())return;
  //  plot_omega(hvtxch1,     hvtxch0, "vertexcharge");     if(wait())return;
  //  plot_omega(hvtx_ptmin1, hvtx_ptmin0, "hvtx_ptmin");   if(wait())return;
  //  plot_omega(hvtx_ipsmin1, hvtx_ipsmin0, "hvtx_ipsmin");   if(wait())return;
  //  plot_omega(hvtx_docamax1, hvtx_docamax0, "hvtx_docamax");  if(wait())return;
  //  plot_omega(hvtx_maxprobf1, hvtx_maxprobf0, "hvtx_maxprobf"); if(wait())return;
  //  plot_omega(hvtx_vflagged1, hvtx_vflagged0, "hvtx_vflagged"); if(wait())return;
  if(endblock) goto end;

 config://///////////////////////////////////////////////////////////
  c->Clear(); c->Divide(1,2);
  plot_omega(hright,       hwrong,  "ip");  if(wait())return;
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
  plot_omega(hright,  hwrong, "variable");
  if(wait())return;
  c->cd(2); h4->Draw();

  if(endblock) goto end;

 nnet: /////////////////////////////////////////////////////////////
  cout<<"plotting at nnet"<<endl;
  c->Clear(); c->Divide(1,2);
  gStyle->SetOptStat(0);
  plot_omega(h100,  h102,  "NNet");         if(wait())return;
  plot_omega(h103,  h104,  "NNet_omega",1); if(wait())return;
  plot_omega(h613,  h513,  "mu");           if(wait())return;
  plot_omega(h613p, h513p, "mu_omega", 1);  if(wait())return;
  plot_omega(h611,  h511,  "e");            if(wait())return;
  plot_omega(h611p, h511p, "e_omega", 1);   if(wait())return;
  plot_omega(h6321, h5321,  "k");           if(wait())return;
  plot_omega(h6321p,h5321p, "k_omega", 1);  if(wait())return;
  plot_omega(h535,  h635, "sameside");      if(wait())return;
  plot_omega(h535p, h635p,"sameside_omega",1); if(wait())return;
  plot_omega(h540,  h640,  "vtx", 1);       if(wait())return;
  plot_omega(h540p, h640p, "vtx_omega", 1); 
  if(endblock) goto end;

 effeff_cut: /////////////////////////////////////////////////////////////
  cout<<"plotting at effeff_cut"<<endl;
  h1->Draw(); //changes dir (stupid root feature)
  plotEffectiveEff(hright, hwrong); if(wait())return;

  plotEffectiveEff(hr_mu_p, hw_mu_p);                if(wait())return;
  plotEffectiveEff(hr_mu_pt, hw_mu_pt);              if(wait())return;
  plotEffectiveEff(hr_mu_ips, hw_mu_ips);            if(wait())return;
  plotEffectiveEff(hr_mu_ippu, hw_mu_ippu);          if(wait())return;
  plotEffectiveEff(homphi_mu_r, homphi_mu_w);          if(wait())return;
  plotEffectiveEff(hr_mu_lcs, hw_mu_lcs, "right2left");if(wait())return;
  plotEffectiveEff(hr_mu_pid, hw_mu_pid);            if(wait())return;
  plotEffectiveEff(hr_mu_tsal, hw_mu_tsal);          if(wait())return;
  plotEffectiveEff(hr_mu_mult, hw_mu_mult, "right2left");if(wait())return;

  plotEffectiveEff(hr_ele_p, hw_ele_p);              if(wait())return;
  plotEffectiveEff(hr_ele_pt, hw_ele_pt);            if(wait())return;
  plotEffectiveEff(hr_ele_ips, hw_ele_ips);          if(wait())return;
  plotEffectiveEff(hr_ele_ippu, hw_ele_ippu);        if(wait())return;
  plotEffectiveEff(homphi_ele_r, homphi_ele_w);          if(wait())return;
  plotEffectiveEff(hr_ele_lcs, hw_ele_lcs, "right2left");if(wait())return;
  plotEffectiveEff(hr_ele_pid, hw_ele_pid);          if(wait())return;
  plotEffectiveEff(hr_ele_tsal, hw_ele_tsal);        if(wait())return;
  plotEffectiveEff(hr_ele_mult, hw_ele_mult,"right2left");if(wait())return;

  plotEffectiveEff(hr_k_p, hw_k_p);                if(wait())return;
  plotEffectiveEff(hr_k_pt, hw_k_pt);              if(wait())return;
  plotEffectiveEff(hr_k_ips, hw_k_ips);            if(wait())return;
  plotEffectiveEff(hr_k_ippu, hw_k_ippu);          if(wait())return;
  plotEffectiveEff(homphi_kO_r, homphi_kO_w);          if(wait())return;
  plotEffectiveEff(hr_k_lcs, hw_k_lcs, "right2left");if(wait())return;
  plotEffectiveEff(hr_k_pid, hw_k_pid);            if(wait())return;
  plotEffectiveEff(hr_k_tsal, hw_k_tsal);          if(wait())return;
  plotEffectiveEff(hr_k_mult, hw_k_mult, "right2left");if(wait())return;

  //pions and kaons same side
  plotEffectiveEff(hr_kS_p, hw_kS_p);                  if(wait())return;
  plotEffectiveEff(hr_kS_pt, hw_kS_pt);                if(wait())return;
  plotEffectiveEff(hr_kS_ips, hw_kS_ips, "right2left");if(wait())return;
  plotEffectiveEff(hr_kS_ippu, hw_kS_ippu);            if(wait())return;
  plotEffectiveEff(homphi_kS_r, homphi_kS_w);          if(wait())return;
  plotEffectiveEff(hr_kS_lcs, hw_kS_lcs, "right2left");if(wait())return;
  plotEffectiveEff(hr_kS_pid, hw_kS_pid, "right2left");if(wait())return;
  plotEffectiveEff(hr_kS_tsal, hw_kS_tsal);            if(wait())return;
  plotEffectiveEff(hr_kS_mult, hw_kS_mult, "right2left");if(wait())return;
  //  plotEffectiveEff(hr_kS_dq, hw_kS_dq);                if(wait())return;
  //  plotEffectiveEff(hr_kS_dqe, hw_kS_dqe);              if(wait())return;

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


 pid: /////////////////////////////////////////////////////////////
  cout<<"plotting at pid"<<endl;

  plotPID("muon",       h1011, h1001, h1021); if(wait())return;
  plotPID("electron",   h1010, h1000, h1020); if(wait())return;
  plotPID("kaon",       h1012, h1002, h1022); if(wait())return;
  plotPID("kaon-proton",h1015, h1005, h1025); if(wait())return;
  plotPID("kaonsame",   h1013, h1003, h1023); if(wait())return;
  plotPID("pion",       h1014, h1004, h1024); 
  
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

 multipleinteraction: /////////////////////////////////////////////////////////////

  cout<<"plotting pointing to right primary vertex"<<endl;
  plotEffectiveEff(hpv_gippu_r, hpv_gippu_w           ); if(wait())return;  
  plotEffectiveEff(hpv_difip_r, hpv_difip_w, "right2left"); if(wait())return;
  plotEffectiveEff(hpv_ipmeandif_r, hpv_ipmeandif_w, "right2left" ); if(wait())return;
  plotEffectiveEff(hpv_zposdif_r, hpv_zposdif_w, "right2left"); if(wait())return;
  plotEffectiveEff(hpv_zposabsdif_r, hpv_zposabsdif_w       ); if(wait())return;

  if(endblock) goto end;

 end:

}
