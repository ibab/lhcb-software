{
/*-----------------------------------------------------------
 *---  DRAW PLOTS -------------------------------------------
 *-----------------------------------------------------------*/
gROOT->Reset();
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

c->Clear(); c->Divide(1,2);
TH1F* hom = plot_omega(homphi_mu_r,  homphi_mu_w,  "mu_distphi"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

TH1F* hom = plot_omega(homphi_kO_r,  homphi_kO_w,  "kO_distphi"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

TH1F* hom = plot_omega(homphi_kS_r,  homphi_kS_w,  "kS_distphi"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

TH1F* hom = plot_omega(homphi_vtx1_r,  homphi_vtx1_w,  "vtx1_distphi"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

TH1F* hom = plot_omega(homphi_vtx2_r,  homphi_vtx2_w,  "vtx2_distphi"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

TH1F* hom = plot_omega(homphi_vtx3_r,  homphi_vtx3_w,  "vtx3_distphi"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;



// c->Clear(); c->Divide(1,2);
// c->cd(1); h80.DrawNormalized(); h80_3.SetLineColor(kRed); h80_3.DrawNormalized("same");
// c->cd(2); h81.DrawNormalized(); h81_3.SetLineColor(kRed); h81_3.DrawNormalized("same");
// c->Print("pics/conf_1.gif");
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
 heffec->Draw();
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
TH1F* hom = plot_omega(h6rSS,  h6wSS,  "Pt B SS"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
TH1F* hom = plot_omega(h6rOS,  h6wOS,  "Pt B OS"); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;

TH1F* hom = plot_omega(h100,  h102,  "prob");  if( waitandstop() ) return;
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

TH1F* hom = plot_omega(h540,  h640,  "vtx");  
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
TH1F* homp= plot_omega(h540p, h640p, "vtx_om", 1); 
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;


// goto end;

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
