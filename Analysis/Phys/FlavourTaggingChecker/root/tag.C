{
/*-----------------------------------------------------------
 *---  DRAW PLOTS -------------------------------------------
 *-----------------------------------------------------------*/
gROOT->Reset();
//goto end;

TFile fhisto("tag.root");

gStyle->SetDrawBorder(0);
gStyle->SetOptFit(1);
gStyle->SetOptStat(0);
gStyle->SetTitleFillColor(5);
TCanvas* c = new TCanvas("c","Tagging",0,0,600,600);
char *s = new char[1];
c->SetBorderMode(0);
c->SetFrameBorderMode(0);
c->SetFillColor(10);

//goto jump;

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



/////////////////////////////////////////////////////////////
c->Clear(); c->Divide(1,2);
gStyle->SetOptStat(0);

c->cd(1);
h100->SetFillColor(3); h100->SetFillStyle(3001);
h100->SetTitle(";Probability output;Events");
h100->Draw();
h102->SetFillColor(kRed); h102->SetFillStyle(3001);
h102->Draw("same");
c->cd(2); gPad->SetGrid();
TH1F *hom = (TH1F*) h100->Clone(); hom->SetName("hom");
calculateOmega(h100, h102, hom);
hom->SetTitle(";Probability output;ratio");
hom->Draw(); hom->Fit("pol1");
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;


c->Clear(); c->Divide(1,2);
c->cd(1);
h613->SetFillColor(3); h613->SetFillStyle(3001);
h613->Draw();
h613->SetTitle(";NN output (muon);Events");
h513->SetFillColor(kRed); h513->SetFillStyle(3001);
h513->SetLineWidth(1); 
h513->Draw("same");
c->cd(2); gPad->SetGrid();
hom->Delete(); hom = (TH1F*) h513->Clone(); hom->SetName("hom");
calculateOmega(h613, h513, hom);
hom->SetTitle(";NN output (muon);Wrong tag fraction");
hom->Draw();hom->Fit("pol1");
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
/////////////////////////////////////////////////////////////

c->cd(1);
h611->SetFillColor(3); h611->SetFillStyle(3001);
h611->Draw();
h611->SetTitle(";NN output (electron);Events");
h511->SetFillColor(kRed); h511->SetFillStyle(3001);
h511->SetLineWidth(1); 
h511->Draw("same");
c->cd(2); gPad->SetGrid();
hom->Delete(); hom = (TH1F*) h511->Clone(); hom->SetName("hom");
calculateOmega(h611, h511, hom);
hom->SetTitle(";NN output (electron);Wrong tag fraction");
hom->Draw();hom->Fit("pol1");
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
/////////////////////////////////////////////////////////////

c->cd(1);
h6321->SetFillColor(3); h6321->SetFillStyle(3001);
h6321->Draw();
h6321->SetTitle(";NN output (OS kaon);Events");
h5321->SetFillColor(kRed); h5321->SetFillStyle(3001);
h5321->SetLineWidth(1); 
h5321->Draw("same");
c->cd(2); gPad->SetGrid();
hom->Delete(); hom = (TH1F*) h5321->Clone(); hom->SetName("hom");
calculateOmega(h6321, h5321, hom);
hom->SetTitle(";NN output (OS kaon);Wrong tag fraction");
hom->Draw();hom->Fit("pol1");
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
/////////////////////////////////////////////////////////////

c->cd(1);
h53210->SetFillColor(3); h53210->SetFillStyle(3001);
h53210->Draw();
h53210->SetTitle(";NN output (SS pion/kaon);Events");
h63210->SetFillColor(kRed); h63210->SetFillStyle(3001);
h63210->SetLineWidth(1); 
h63210->Draw("same");
c->cd(2); gPad->SetGrid();
hom->Delete(); hom = (TH1F*) h53210->Clone(); hom->SetName("hom");
calculateOmega(h53210, h63210, hom);
hom->SetTitle(";NN output (SS pion/kaon);Wrong tag fraction");
hom->Draw();hom->Fit("pol1");
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
/////////////////////////////////////////////////////////////

c->cd(1);
h540->SetFillColor(3); h540->SetFillStyle(3001);
h540->Draw();
h540->SetTitle(";vtx charge;Events");
h640->SetFillColor(kRed); h640->SetFillStyle(3001);
h640->SetLineWidth(1); 
h640->Draw("same");
c->cd(2); gPad->SetGrid();
hom->Delete(); hom = (TH1F*) h540->Clone(); hom->SetName("hom");
calculateOmega(h540, h640, hom);
hom->SetTitle(";vtx charge ;Wrong tag fraction");
hom->Draw();hom->Fit("pol1");
c->Update();cout<<"--> Hit return to continue ";gets(s);if(*s=='q')return;
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
