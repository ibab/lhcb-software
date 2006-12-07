TCanvas* c1 = new TCanvas("c1","Edinburgh",800,800);
c1->SetLogy(1);

TFile* F = new TFile("DVHistos_4b.root") 
F->ls()
F->cd("Bout")               
F->ls() 
TH1D* H1 = F->Get("Bout/1")
TH1D* H2 = F->Get("Bout/2")
TH1D* H3 = F->Get("Bout/3")
TH1D* H4 = F->Get("Bout/4")
TH1D* H5 = F->Get("Bout/5")
TH1D* H12 = F->Get("Bout/12")

H1->SetLineColor(2)
H1->SetLineWidth(3)
H1->Draw()
gPad->SaveAs("DV4_H1.eps");

H2->SetLineColor(2)
H2->SetLineWidth(3)
H2->Draw()
gPad->SaveAs("DV4_H2.eps");

H3->SetLineColor(2)
H3->SetLineWidth(3)
H3->Draw()
gPad->SaveAs("DV4_H3.eps");

H4->SetLineColor(2)
H4->SetLineWidth(3)
H4->Draw()
gPad->SaveAs("DV4_H4.eps");

H5->SetLineColor(2)
H5->SetLineWidth(3)
H5->Draw()
gPad->SaveAs("DV4_H5.eps");

H12->SetLineColor(2)
H12->SetLineWidth(3)
H12->Draw()
gPad->SaveAs("DV4_H12.eps");

