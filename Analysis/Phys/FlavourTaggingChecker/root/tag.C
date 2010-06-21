{
/*-----------------------------------------------------------
 *---  DRAW PLOTS -------------------------------------------
 *-----------------------------------------------------------*/
gROOT->Reset();
gROOT->ProcessLine(".L tag.h+"); //load some functions defined there

/* Example
TFile *f2 = new TFile("dati/dc06/set5/bsdsp.root")
TNtuple *t1 = (TNtuple*)f2->Get("1")
t1->SetName("nt")
nt->Draw("distPhi","abs(ID)==321&&distPhi<0.01")
*/

TFile fhisto("tag.root"); //fhisto.cd("mc09");

gStyle->SetDrawBorder(0);
gStyle->SetOptFit(1);
gStyle->SetOptStat(0);
gStyle->SetTitleFillColor(5);
TCanvas* c = new TCanvas("c","Tagging",0,0,600,600);
char *s = new char[1];
c->SetBorderMode(0);
c->SetFrameBorderMode(0);
c->SetFillColor(10);
gStyle->SetOptStat(0);

 goto jump;
// goto weightedch;
// goto omegapt;
// goto propomega;

   weightedch:
//To test weight the pt to select candidate
c->Clear(); c->Divide(1,2);
TH1F* hom = plot_omega(hr_k_ncand,  hw_k_ncand,  "ncand_for_k"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
TH1F* hom = plot_omega(hr_k_weightedch,  hw_k_weightedch,  "weightedch_Pt_k"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

TH1F* hom = plot_omega(hr_kS_ncand,  hw_kS_ncand,  "ncand_SS"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
TH1F* hom = plot_omega(hr_SS_weightedch,  hw_SS_weightedch,  "weightedch_Pt_SS"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
 goto end;

   omegapt:
// omega vs pt
c->Clear(); c->Divide(1,2);
TH1F* hom = plot_omega(h6rSS,  h6wSS,  "PtB_SS"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
TH1F* hom = plot_omega(h6rOS,  h6wOS,  "PtB_OS"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
TH1F* hom = plot_omega(h6r,  h6w,  "PtB"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
TH1F* hom = plot_omega(h6rtis,  h6wtis,  "PtB_tis"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
TH1F* hom = plot_omega(h6rtos,  h6wtos,  "PtB_tos"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
TH1F* hom = plot_omega(h6rtistos,  h6wtistos,  "PtB_tistos"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
cout<<" tis / tos fracttion";
TH1F* hom = plot_omega(h6notis,  h6tis,  "PtBtisfrac"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
TH1F* hom = plot_omega(h6tos,  h6notos,  "PtBtosfrac"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
TH1F* hom = plot_omega(h6notistos,  h6tistos,  "PtBtistosfrac"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
cout<<" tis / tos fracttion";
h6u.Draw();
h6tis.SetLineColor(kRed); h6tis.SetFillColor(0); h6tis.Draw("same");
h6tistos.SetLineColor(kBlue); h6tistos.SetFillColor(0); h6tistos.Draw("same");
h6tos.SetLineColor(kGreen); h6tos.SetFillColor(0); h6tos.Draw("same");

goto end;
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


//goto addseed;
//goto jump;
goto vtxcharge;

c->Clear(); c->Divide(1,2);
 hv2D2.SetMarkerColor(kBlack); hv2D2.Draw();
 hv2D1.SetMarkerColor(kGreen); hv2D1.Draw("same");
 hv2D0.SetMarkerColor(kRed);   hv2D0.Draw("same");
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

plot_ratio("probf",hvprobf2,hvprobf0,hvprobf0 ); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

 plot_ratio("maxprobf",hmaxprobf2, hmaxprobf0, hmaxprobf0); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

plot_ratio("delta_r",hz2, hz0, hzd); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

plot_ratio("chi2", hvChi2 , hvChi0, hvChid); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

plot_ratio("ptmin", hvpmin_2 ,hvpmin_0 , hvpmin_d); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

plot_ratio("ptmax", hvpmax_2  , hvpmax_0 ,  hvpmax_d); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

plot_ratio("ipmin", hvipmin2 ,hvipmin0 ,hvipmind ); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

plot_ratio("ipmax", hvipmax2 ,hvipmax0 , hvipmaxd); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

plot_ratio("ipsmin", hvipsmin2 ,hvipsmin0 ,hvipsmind ); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

plot_ratio("deltaphi",hvdphi2  , hvdphi0, hvdphid); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

plot_ratio("hvmass", hvmass2 , hvmass0, hvmassd); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

 goto end;

 addseed:

 htr_count0.SetLineColor(2); htr_count0.Draw();
 htr_count1.SetLineColor(3); htr_count1.Draw("same");
 htr_countd.SetLineColor(4); htr_countd.Draw("same");
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

plot_ratio("pt",htr_pt1, htr_pt0, htr_ptd ); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

plot_ratio("mass",htr_mass1, htr_mass0, htr_massd ); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

plot_ratio("chi2seed",htr_Chi2Seed1, htr_Chi2Seed0, htr_Chi2Seedd ); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

plot_ratio("ip",htr_ip1, htr_ip0, htr_ipd ); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

plot_ratio("ips",htr_ips1, htr_ips0, htr_ipsd ); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

plot_ratio("ipsv",htr_ipsv1, htr_ipsv0, htr_ipsvd ); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

plot_ratio("ipsvs",htr_ipsvs1, htr_ipsvs0, htr_ipsvsd ); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

plot_ratio("doca",htr_DOCA1, htr_DOCA0, htr_DOCAd ); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

plot_ratio("deltaphi",htr_deltaphi1, htr_deltaphi0, htr_deltaphid ); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

 goto end;

 vtxcharge:
c->Clear(); c->Divide(1,3);
 c.cd(1); hvtx_pullz.Draw(); hvtx_pullz2.SetLineColor(3); hvtx_pullz2.Draw("same");
 hvtx_pullzd.SetLineStyle(2);hvtx_pullzd.SetLineColor(4); hvtx_pullzd.Draw("same");
 c.cd(2); hvtx_pullf.Draw();
 c.cd(3); hvtx_pulltheta.Draw();c.cd(); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

 plot_ratio("vertexcharge", hvtxch1, hvtxch0, hvtxchb); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
 plot_omega(hvtxch1, hvtxch0, "vertexcharge"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

 plot_omega(hvtx_ptmin1, hvtx_ptmin0, "hvtx_ptmin"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

 plot_omega(hvtx_ipsmin1, hvtx_ipsmin0, "hvtx_ipsmin"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

 plot_omega(hvtx_docamax1, hvtx_docamax0, "hvtx_docamax"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

 plot_omega(hvtx_maxprobf1, hvtx_maxprobf0, "hvtx_maxprobf"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

 plot_omega(hvtx_vflagged1, hvtx_vflagged0, "hvtx_vflagged"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;


goto end;

c->Clear(); c->Divide(1,2);
TH1F* hom = plot_omega(hright,  hwrong,  "ip"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;


TH1F* hom = plot_omega(homphi_mu_r,  homphi_mu_w,  "mu_distphi"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

TH1F* hom = plot_omega(homphi_kO_r,  homphi_kO_w,  "kO_distphi"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

TH1F* hom = plot_omega(homphi_kS_r,  homphi_kS_w,  "kS_distphi"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

c->cd();c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
c->Clear(); c->Divide(1,2);
c->cd(1); h82.DrawNormalized(); h82_3.SetLineColor(kRed); h82_3.DrawNormalized("same");
c->cd(2); h83.DrawNormalized(); h83_3.SetLineColor(kRed); h83_3.DrawNormalized("same");
c->Print("pics/conf_2.gif");
c->cd();c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
c->Clear(); c->Divide(1,2);
c->cd(1); h84.DrawNormalized(); h84_3.SetLineColor(kRed); h84_3.DrawNormalized("same");
c->cd(2); h85.DrawNormalized(); h85_3.SetLineColor(kRed); h85_3.DrawNormalized("same");
c->Print("pics/conf_3.gif");
c->cd();c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
c->Clear(); c->Divide(1,2);
c->cd(1); h88.DrawNormalized(); h88_3.SetLineColor(kRed); h88_3.DrawNormalized("same");
c->cd(2); h89.DrawNormalized(); h89_3.SetLineColor(kRed); h89_3.DrawNormalized("same");
c->Print("pics/conf_4.gif");
c->cd();c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
// c->Clear(); c->Divide(1,2);
// c->cd(1); h86.DrawNormalized(); h86_3.SetLineColor(kRed); h86_3.DrawNormalized("same");
// c->Print("pics/conf_5.gif");
// c->cd();c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;


/////////////////////////////////////////////////////////////
gStyle->SetOptStat(1111);
c->Clear(); 
c->Divide(2,2); gPad->SetBorderMode(0);
c->cd(1); h5->Draw();
c->cd(2); h6->Draw();
c->cd(3); h301->Draw();
c->cd(4); h401->Draw(); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

c->Clear(); 
c->Divide(1,2);
c->cd(1);
 hright->SetFillColor(kGreen); hright->Draw();
 hwrong->SetFillColor(kRed); hwrong->Draw("same");
c->cd(2);
// heffec->Draw();
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
c->Clear(); 
c->Divide(1,2);
TH1F* hom = plot_omega(hright,  hwrong,  "");
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

jump: 

/////////////////////////////////////////////////////////////
c->Clear(); c->Divide(1,2);
gStyle->SetOptStat(0);

//##################################################### <<<<<<<-----
//marc - find omega from NNcomb
//TH1F* hom = plot_omega(h200,  h202,  "NNcombprob",1);
//c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

TH1F* hom = plot_omega(h100,  h102,  "prob(NNetSum)"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
TH1F* hom = plot_omega(h103,  h104,  "prob(1-NNetSum)",1);
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

TH1F* hom = plot_omega(h613,  h513,  "mu");   
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
TH1F* homp= plot_omega(h613p, h513p, "mu_om", 1); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

TH1F* hom = plot_omega(h611,  h511,  "e");    
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
TH1F* homp= plot_omega(h611p, h511p, "e_om", 1); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

TH1F* hom = plot_omega(h6321,  h5321,  "k");   
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
TH1F* homp= plot_omega(h6321p, h5321p, "k_om", 1); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

TH1F* hom = plot_omega(h53210,  h63210, "sameside"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
TH1F* homp= plot_omega(h53210p, h63210p,"sameside_om",1);
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

 TH1F* hom = plot_omega(h540,  h640,  "vtx", 3);  
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
TH1F* homp= plot_omega(h540p, h640p, "vtx_om", 1); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;


 goto end;


propomega: 
/////////////////////////////////////////////////////////////
 TString propname;
 // propname="TIS";
 // propname="mult";
 propname="PtB";

c->Clear(); c->Divide(1,2);
gStyle->SetOptStat(0);
//##################################################### <<<<<<<-----
 cout<<"Muon tagger"<<endl;
TH1F* homp= plot_omega(h613p, h513p, "mu_om", 1); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
TH1F* hom0p= plot_omega(h90mpr, h90mpw, "mu_om", 1); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
TH1F* hom1p= plot_omega(h91mpr, h91mpw, "mu_om", 1); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
homp.Draw();
hom0p.SetLineColor(kRed); hom0p.GetFunction("pol1").SetLineColor(kRed); hom0p.Draw("same");
hom1p.SetLineColor(kBlue); hom1p.GetFunction("pol1").SetLineColor(kBlue); hom1p.Draw("same");
leg = new TLegend(0.6,0.8,0.98,0.996);
 leg->SetHeader("Muon Tagger");
 leg->AddEntry(hom1p,propname+" events","l");
 leg->AddEntry(hom0p,"no " + propname +" events","l");
 // leg->AddEntry("homp","all events","l");
 leg->Draw();
 c->Print("someplots/muon_omega_vs_"+propname+".gif");
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

 cout<<"Electron tagger"<<endl;
TH1F* homp= plot_omega(h611p, h511p, "e_om", 1); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
TH1F* hom0p= plot_omega(h90epr, h90epw, "e_om", 1); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
TH1F* hom1p= plot_omega(h91epr, h91epw, "e_om", 1); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
homp.Draw();
hom0p.SetLineColor(kRed); hom0p.GetFunction("pol1").SetLineColor(kRed); hom0p.Draw("same");
hom1p.SetLineColor(kBlue); hom1p.GetFunction("pol1").SetLineColor(kBlue); hom1p.Draw("same");
leg = new TLegend(0.6,0.8,0.98,0.996);
 leg->SetHeader("Electron Tagger");
 leg->AddEntry(hom1p,propname+" events","l");
 leg->AddEntry(hom0p,"no " + propname +" events","l");
 // leg->AddEntry("homp","all events","l");
 leg->Draw();
 c->Print("someplots/ele_omega_vs_"+propname+".gif");
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

 cout<<"Kaon tagger"<<endl;
TH1F* homp= plot_omega(h6321p, h5321p, "k_om", 1); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
TH1F* hom0p= plot_omega(h90kpr, h90kpw, "k_om", 1); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
TH1F* hom1p= plot_omega(h91kpr, h91kpw, "k_om", 1); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
homp.Draw();
hom0p.SetLineColor(kRed); hom0p.GetFunction("pol1").SetLineColor(kRed); hom0p.Draw("same");
hom1p.SetLineColor(kBlue); hom1p.GetFunction("pol1").SetLineColor(kBlue); hom1p.Draw("same");
leg = new TLegend(0.6,0.8,0.98,0.996);
 leg->SetHeader("Kaon Tagger");
 leg->AddEntry(hom1p,propname+" events","l");
 leg->AddEntry(hom0p,"no " + propname +" events","l");
 // leg->AddEntry("homp","all events","l");
 leg->Draw();
 c->Print("someplots/kaon_omega_vs_"+propname+".gif");
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

 cout<<"SS tagger"<<endl;
TH1F* homp= plot_omega(h53210p, h63210p, "ss_om", 1); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
TH1F* hom0p= plot_omega(h90sspr, h90sspw, "ss_om", 1); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
TH1F* hom1p= plot_omega(h91sspr, h91sspw, "ss_om", 1); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
homp.Draw();
hom0p.SetLineColor(kRed); hom0p.GetFunction("pol1").SetLineColor(kRed); hom0p.Draw("same");
hom1p.SetLineColor(kBlue); hom1p.GetFunction("pol1").SetLineColor(kBlue); hom1p.Draw("same");
leg = new TLegend(0.6,0.8,0.98,0.996);
 leg->SetHeader("SS Tagger");
 leg->AddEntry(hom1p,propname+" events","l");
 leg->AddEntry(hom0p,"no " + propname +" events","l");
 // leg->AddEntry("homp","all events","l");
 leg->Draw();
 c->Print("someplots/ss_omega_vs_"+propname+".gif");
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

 cout<<"Vtx tagger"<<endl;
TH1F* homp= plot_omega(h540p, h640p, "vtx_om", 1); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
TH1F* hom0p= plot_omega(h90vpr, h90vpw, "vtx_om", 1); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
TH1F* hom1p= plot_omega(h91vpr, h91vpw, "vtx_om", 1); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
homp.Draw();
hom0p.SetLineColor(kRed); hom0p.GetFunction("pol1").SetLineColor(kRed); hom0p.Draw("same");
hom1p.SetLineColor(kBlue); hom1p.GetFunction("pol1").SetLineColor(kBlue); hom1p.Draw("same");
leg = new TLegend(0.6,0.8,0.98,0.996);
 leg->SetHeader("Vtx Tagger");
 leg->AddEntry(hom1p,propname+" events","l");
 leg->AddEntry(hom0p,"no " + propname +" events","l");
 // leg->AddEntry(homp,"all events","l");
 leg->Draw();
 c->Print("someplots/vtx_omega_vs_"+propname+".gif");
cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

 goto end;

/////////////////////////////////////////////////////////////
c->Clear();
c->Divide(2,2);

c->cd(1);
gPad->SetBorderMode(0);
h1011->SetFillColor(4); h1011->SetFillStyle(3001);
h1011->Draw();
h1011->SetTitle(";#Delta L#mu;Events");
h1001->SetFillColor(kRed); h1001->SetFillStyle(3001);
h1001->SetLineWidth(1); 
h1001->Draw("same");
h1021->SetLineWidth(1); h1021->Draw("same");
c->cd(2);
gPad->SetBorderMode(0);
gPad->SetGrid();
TH1F *hom = (TH1F*) h1001->Clone(); hom->Sumw2();
hom->Divide(hom,h1011);
hom->SetMinimum(0.0); hom->SetMaximum(1.0);
hom->SetLineColor(kBlue);
hom->SetTitle(";#Delta L#mu;Purity");
hom->Draw("pe");

c->cd(3);
gPad->SetBorderMode(0);
h1010->SetFillColor(4); h1010->SetFillStyle(3001);
h1010->Draw();
h1010->SetTitle(";#Delta Le;Events");
h1000->SetFillColor(kRed); h1000->SetFillStyle(3001);
h1000->SetLineWidth(1); 
h1000->Draw("same");
h1020->SetLineWidth(1); h1020->Draw("same");
c->cd(4);
gPad->SetBorderMode(0);
gPad->SetGrid();
TH1F *hom = (TH1F*) h1000->Clone(); hom->Sumw2();
hom->Divide(hom,h1010);
hom->SetMinimum(0.0); hom->SetMaximum(1.0);
hom->SetLineColor(kBlue);
hom->SetTitle(";#Delta Le;Purity");
hom->Draw("pe");
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;



c->Clear();
c->Divide(2,2);
c->cd(1);
gPad->SetBorderMode(0);
h1012->SetFillColor(4); h1012->SetFillStyle(3001);
h1012->Draw();
h1012->SetTitle(";#Delta Lk;Events");
h1002->SetFillColor(kRed); h1002->SetFillStyle(3001);
h1002->SetLineWidth(1); 
h1002->Draw("same");
h1022->SetLineWidth(1); h1022->Draw("same");
c->cd(2);
gPad->SetBorderMode(0);
gPad->SetGrid();
TH1F *hom = (TH1F*) h1002->Clone(); hom->Sumw2();
hom->Divide(hom,h1012);
hom->SetMinimum(0.0); hom->SetMaximum(1.0);
hom->SetLineColor(kBlue);
hom->SetTitle(";#Delta Lk;Purity");
hom->Draw("pe");

c->cd(3);
gPad->SetBorderMode(0);
h1015->SetFillColor(4); h1015->SetFillStyle(3001);
h1015->Draw();
h1015->SetTitle(";#Delta Lk-Lp;Events");
h1005->SetFillColor(kRed); h1005->SetFillStyle(3001);
h1005->SetLineWidth(1); 
h1005->Draw("same");
h1025->SetLineWidth(1); h1025->Draw("same");
c->cd(4);
gPad->SetBorderMode(0);
gPad->SetGrid();
TH1F *hom = (TH1F*) h1005->Clone(); hom->Sumw2();
hom->Divide(hom,h1015);
hom->SetMinimum(0.0); hom->SetMaximum(1.0);
hom->SetLineColor(kBlue);
hom->SetTitle(";#Delta Lk-Lp;Purity");
hom->Draw("pe");

c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
c->Clear();
c->Divide(2,2);
c->cd(1);
gPad->SetBorderMode(0);
h1013->SetFillColor(4); h1013->SetFillStyle(3001);
h1013->Draw();
h1013->SetTitle(";#Delta LkS;Events");
h1003->SetFillColor(kRed); h1003->SetFillStyle(3001);
h1003->SetLineWidth(1); 
h1003->Draw("same");
c->cd(2);
gPad->SetBorderMode(0);
gPad->SetGrid();
TH1F *hom = (TH1F*) h1003->Clone(); hom->Sumw2();
hom->Divide(hom,h1013);
hom->SetMinimum(0.0); hom->SetMaximum(1.0);
hom->SetLineColor(kBlue);
hom->SetTitle(";#Delta LkS;Purity");
hom->Draw("pe");

c->cd(3);
gPad->SetBorderMode(0);
h1014->SetFillColor(4); h1014->SetFillStyle(3001);
h1014->Draw();
h1014->SetTitle(";#Delta LkS-Lproton;Events");
h1004->SetFillColor(kRed); h1004->SetFillStyle(3001);
h1004->SetLineWidth(1); 
h1004->Draw("same");
c->cd(4);
gPad->SetBorderMode(0);
gPad->SetGrid();
TH1F *hom = (TH1F*) h1004->Clone(); hom->Sumw2();
hom->Divide(hom,h1014);
hom->SetMinimum(0.0); hom->SetMaximum(1.0);
hom->SetLineColor(kBlue);
hom->SetTitle(";#Delta LkS-Lproton;Purity");
hom->Draw("pe");

end:
}
