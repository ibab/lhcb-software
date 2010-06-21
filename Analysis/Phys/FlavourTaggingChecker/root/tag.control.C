{

/*-----------------------------------------------------------
 *---  CONTROL PLOTS -------------------------------------------
 *-----------------------------------------------------------*/
gROOT->Reset();

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

c->Clear(); c->Divide(1,2); c->cd(1);
 hright->SetTitle("Ele"); hright->SetFillColor(kGreen); hright->Draw();
 hwrong->SetFillColor(kRed)  ; hwrong->Draw("same"); c->cd(2); gPad->SetGrid(); heffec->Draw();
 c->cd();c->Print("pics/tunerveloch.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

 goto jump;

//Pt ////////////////
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_mu_pt->SetTitle("Muon"); hr_mu_pt->SetFillColor(kGreen); hr_mu_pt->Draw();
 hw_mu_pt->SetFillColor(kRed)  ; hw_mu_pt->Draw("same"); c->cd(2); gPad->SetGrid(); he_mu_pt->Draw();
 c->cd();c->Print("pics/tuner_mu_pt.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_ele_pt->SetTitle("Electrons"); hr_ele_pt->SetFillColor(kGreen); hr_ele_pt->Draw();
 hw_ele_pt->SetFillColor(kRed)  ; hw_ele_pt->Draw("same"); c->cd(2); gPad->SetGrid(); he_ele_pt->Draw();
 c->cd();c->Print("pics/tuner_ele_pt.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_k_pt->SetTitle("kaon OS"); hr_k_pt->SetFillColor(kGreen); hr_k_pt->Draw();
 hw_k_pt->SetFillColor(kRed)  ; hw_k_pt->Draw("same"); c->cd(2); gPad->SetGrid(); he_k_pt->Draw();
 c->cd();c->Print("pics/tuner_k_pt.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_kS_pt->SetTitle("kaon SS"); hr_kS_pt->SetFillColor(kGreen); hr_kS_pt->Draw();
 hw_kS_pt->SetFillColor(kRed)  ; hw_kS_pt->Draw("same"); c->cd(2); gPad->SetGrid(); he_kS_pt->Draw();
 c->cd();c->Print("pics/tuner_kS_pt.gif");
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
/////////////////////

//P ////////////////
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_mu_p->SetTitle("Muon"); hr_mu_p->SetFillColor(kGreen); hr_mu_p->Draw();
 hw_mu_p->SetFillColor(kRed)  ; hw_mu_p->Draw("same"); c->cd(2); gPad->SetGrid(); he_mu_p->Draw();
 c->cd();c->Print("pics/tuner_mu_p.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_ele_p->SetTitle("Electrons"); hr_ele_p->SetFillColor(kGreen); hr_ele_p->Draw();
 hw_ele_p->SetFillColor(kRed)  ; hw_ele_p->Draw("same"); c->cd(2); gPad->SetGrid(); he_ele_p->Draw();
 c->cd();c->Print("pics/tuner_ele_p.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_k_p->SetTitle("kaon OS"); hr_k_p->SetFillColor(kGreen); hr_k_p->Draw();
 hw_k_p->SetFillColor(kRed)  ; hw_k_p->Draw("same"); c->cd(2); gPad->SetGrid(); he_k_p->Draw();
 c->cd();c->Print("pics/tuner_k_p.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_kS_p->SetTitle("kaon SS"); hr_kS_p->SetFillColor(kGreen); hr_kS_p->Draw();
 hw_kS_p->SetFillColor(kRed)  ; hw_kS_p->Draw("same"); c->cd(2); gPad->SetGrid(); he_kS_p->Draw();
 c->cd();c->Print("pics/tuner_kS_p.gif");
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
/////////////////////

//ips ////////////////
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_mu_ips->SetTitle("Muon"); hr_mu_ips->SetFillColor(kGreen); hr_mu_ips->Draw();
 hw_mu_ips->SetFillColor(kRed)  ; hw_mu_ips->Draw("same"); c->cd(2); gPad->SetGrid(); he_mu_ips->Draw();
 c->cd();c->Print("pics/tuner_mu_ips.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_ele_ips->SetTitle("Electrons"); hr_ele_ips->SetFillColor(kGreen); hr_ele_ips->Draw();
 hw_ele_ips->SetFillColor(kRed)  ; hw_ele_ips->Draw("same"); c->cd(2); gPad->SetGrid(); he_ele_ips->Draw();
 c->cd();c->Print("pics/tuner_ele_ips.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_k_ips->SetTitle("kaon OS"); hr_k_ips->SetFillColor(kGreen); hr_k_ips->Draw();
 hw_k_ips->SetFillColor(kRed)  ; hw_k_ips->Draw("same"); c->cd(2); gPad->SetGrid(); he_k_ips->Draw();
 c->cd();c->Print("pics/tuner_k_ips.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_kS_ips->SetTitle("kaon SS"); hr_kS_ips->SetFillColor(kGreen); hr_kS_ips->Draw();
 hw_kS_ips->SetFillColor(kRed)  ; hw_kS_ips->Draw("same"); c->cd(2); gPad->SetGrid(); he_kS_ips->Draw();
 c->cd();c->Print("pics/tuner_kS_ips.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
/////////////////////
 
//ippu////////////////
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_mu_ippu->SetTitle("Muon"); hr_mu_ippu->SetFillColor(kGreen); hr_mu_ippu->Draw();
 hw_mu_ippu->SetFillColor(kRed)  ; hw_mu_ippu->Draw("same"); c->cd(2); gPad->SetGrid(); he_mu_ippu->Draw();
 c->cd();c->Print("pics/tuner_mu_ippu.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_ele_ippu->SetTitle("Electrons"); hr_ele_ippu->SetFillColor(kGreen); hr_ele_ippu->Draw();
 hw_ele_ippu->SetFillColor(kRed)  ; hw_ele_ippu->Draw("same"); c->cd(2); gPad->SetGrid(); he_ele_ippu->Draw();
 c->cd();c->Print("pics/tuner_ele_ippu.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_k_ippu->SetTitle("kaon OS"); hr_k_ippu->SetFillColor(kGreen); hr_k_ippu->Draw();
 hw_k_ippu->SetFillColor(kRed)  ; hw_k_ippu->Draw("same"); c->cd(2); gPad->SetGrid(); he_k_ippu->Draw();
 c->cd();c->Print("pics/tuner_k_ippu.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_kS_ippu->SetTitle("kaon SS"); hr_kS_ippu->SetFillColor(kGreen); hr_kS_ippu->Draw();
 hw_kS_ippu->SetFillColor(kRed)  ; hw_kS_ippu->Draw("same"); c->cd(2); gPad->SetGrid(); he_kS_ippu->Draw();
 c->cd();c->Print("pics/tuner_kS_ippu.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
/////////////////////

//lcs////////////////
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_mu_lcs->SetTitle("Muon"); hr_mu_lcs->SetFillColor(kGreen); hr_mu_lcs->Draw();
 hw_mu_lcs->SetFillColor(kRed)  ; hw_mu_lcs->Draw("same"); c->cd(2); gPad->SetGrid(); he_mu_lcs->Draw();
 c->cd();c->Print("pics/tuner_mu_lcs.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_ele_lcs->SetTitle("Electrons"); hr_ele_lcs->SetFillColor(kGreen); hr_ele_lcs->Draw();
 hw_ele_lcs->SetFillColor(kRed)  ; hw_ele_lcs->Draw("same"); c->cd(2); gPad->SetGrid(); he_ele_lcs->Draw();
 c->cd();c->Print("pics/tuner_ele_lcs.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_k_lcs->SetTitle("kaon OS"); hr_k_lcs->SetFillColor(kGreen); hr_k_lcs->Draw();
 hw_k_lcs->SetFillColor(kRed)  ; hw_k_lcs->Draw("same"); c->cd(2); gPad->SetGrid(); he_k_lcs->Draw();
 c->cd();c->Print("pics/tuner_k_lcs.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_kS_lcs->SetTitle("kaon SS"); hr_kS_lcs->SetFillColor(kGreen); hr_kS_lcs->Draw();
 hw_kS_lcs->SetFillColor(kRed)  ; hw_kS_lcs->Draw("same"); c->cd(2); gPad->SetGrid(); he_kS_lcs->Draw();
 c->cd();c->Print("pics/tuner_kS_lcs.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
/////////////////////

//pid////////////////
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_mu_pid->SetTitle("Muon"); hr_mu_pid->SetFillColor(kGreen); hr_mu_pid->Draw();
 hw_mu_pid->SetFillColor(kRed)  ; hw_mu_pid->Draw("same"); c->cd(2); gPad->SetGrid(); he_mu_pid->Draw();
 c->cd();c->Print("pics/tuner_mu_pid.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_ele_pid->SetTitle("Electrons"); hr_ele_pid->SetFillColor(kGreen); hr_ele_pid->Draw();
 hw_ele_pid->SetFillColor(kRed)  ; hw_ele_pid->Draw("same"); c->cd(2); gPad->SetGrid(); he_ele_pid->Draw();
 c->cd();c->Print("pics/tuner_ele_pid.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_k_pid->SetTitle("kaon OS"); hr_k_pid->SetFillColor(kGreen); hr_k_pid->Draw();
 hw_k_pid->SetFillColor(kRed)  ; hw_k_pid->Draw("same"); c->cd(2); gPad->SetGrid(); he_k_pid->Draw();
 c->cd();c->Print("pics/tuner_k_pid.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_kS_pid->SetTitle("kaon SS"); hr_kS_pid->SetFillColor(kGreen); hr_kS_pid->Draw();
 hw_kS_pid->SetFillColor(kRed)  ; hw_kS_pid->Draw("same"); c->cd(2); gPad->SetGrid(); he_kS_pid->Draw();
 c->cd();c->Print("pics/tuner_kS_pid.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
/////////////////////


//tsal////////////////
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_mu_tsal->SetTitle("Muon"); hr_mu_tsal->SetFillColor(kGreen); hr_mu_tsal->Draw();
 hw_mu_tsal->SetFillColor(kRed)  ; hw_mu_tsal->Draw("same"); c->cd(2); gPad->SetGrid(); he_mu_tsal->Draw();
 c->cd();c->Print("pics/tuner_mu_tsal.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_ele_tsal->SetTitle("Electrons"); hr_ele_tsal->SetFillColor(kGreen); hr_ele_tsal->Draw();
 hw_ele_tsal->SetFillColor(kRed)  ; hw_ele_tsal->Draw("same"); c->cd(2); gPad->SetGrid(); he_ele_tsal->Draw();
 c->cd();c->Print("pics/tuner_ele_tsal.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_k_tsal->SetTitle("kaon OS"); hr_k_tsal->SetFillColor(kGreen); hr_k_tsal->Draw();
 hw_k_tsal->SetFillColor(kRed)  ; hw_k_tsal->Draw("same"); c->cd(2); gPad->SetGrid(); he_k_tsal->Draw();
 c->cd();c->Print("pics/tuner_k_tsal.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_kS_tsal->SetTitle("kaon SS"); hr_kS_tsal->SetFillColor(kGreen); hr_kS_tsal->Draw();
 hw_kS_tsal->SetFillColor(kRed)  ; hw_kS_tsal->Draw("same"); c->cd(2); gPad->SetGrid(); he_kS_tsal->Draw();
 c->cd();c->Print("pics/tuner_kS_tsal.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
/////////////////////

//mult////////////////
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_mu_mult->SetTitle("Muon"); hr_mu_mult->SetFillColor(kGreen); hr_mu_mult->Draw();
 hw_mu_mult->SetFillColor(kRed)  ; hw_mu_mult->Draw("same"); c->cd(2); gPad->SetGrid(); he_mu_mult->Draw();
 c->cd();c->Print("pics/tuner_mu_mult.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_ele_mult->SetTitle("Electrons"); hr_ele_mult->SetFillColor(kGreen); hr_ele_mult->Draw();
 hw_ele_mult->SetFillColor(kRed)  ; hw_ele_mult->Draw("same"); c->cd(2); gPad->SetGrid(); he_ele_mult->Draw();
 c->cd();c->Print("pics/tuner_ele_mult.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_k_mult->SetTitle("kaon OS"); hr_k_mult->SetFillColor(kGreen); hr_k_mult->Draw();
 hw_k_mult->SetFillColor(kRed)  ; hw_k_mult->Draw("same"); c->cd(2); gPad->SetGrid(); he_k_mult->Draw();
 c->cd();c->Print("pics/tuner_k_mult.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_kS_mult->SetTitle("kaon SS"); hr_kS_mult->SetFillColor(kGreen); hr_kS_mult->Draw();
 hw_kS_mult->SetFillColor(kRed)  ; hw_kS_mult->Draw("same"); c->cd(2); gPad->SetGrid(); he_kS_mult->Draw();
 c->cd();c->Print("pics/tuner_kS_mult.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
/////////////////////
jump:

//ncand////////////////
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_mu_ncand->SetTitle("Muon"); hr_mu_ncand->SetFillColor(kGreen); hr_mu_ncand->Draw();
 hw_mu_ncand->SetFillColor(kRed)  ; hw_mu_ncand->Draw("same"); c->cd(2); gPad->SetGrid(); he_mu_ncand->Draw();
 c->cd();c->Print("pics/tuner_mu_ncand.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_ele_ncand->SetTitle("Electrons"); hr_ele_ncand->SetFillColor(kGreen); hr_ele_ncand->Draw();
 hw_ele_ncand->SetFillColor(kRed)  ; hw_ele_ncand->Draw("same"); c->cd(2); gPad->SetGrid(); he_ele_ncand->Draw();
 c->cd();c->Print("pics/tuner_ele_ncand.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_k_ncand->SetTitle("kaon OS"); hr_k_ncand->SetFillColor(kGreen); hr_k_ncand->Draw();
 hw_k_ncand->SetFillColor(kRed)  ; hw_k_ncand->Draw("same"); c->cd(2); gPad->SetGrid(); he_k_ncand->Draw();
 c->cd();c->Print("pics/tuner_k_ncand.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
c->Clear(); c->Divide(1,2); c->cd(1);
 hr_kS_ncand->SetTitle("kaon SS"); hr_kS_ncand->SetFillColor(kGreen); hr_kS_ncand->Draw();
 hw_kS_ncand->SetFillColor(kRed)  ; hw_kS_ncand->Draw("same"); c->cd(2); gPad->SetGrid(); he_kS_ncand->Draw();
 c->cd();c->Print("pics/tuner_kS_ncand.gif"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
/////////////////////



}
