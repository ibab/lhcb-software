TCanvas* c1 = new TCanvas("c1","Bologna",800,800);
c1->SetLogy();

TFile* F = new TFile("DVHistos.root") 
F->ls()
F->cd("Jpsi2MuMu")               
F->ls() 
TH1D* H1 = F->Get("Jpsi2MuMu/1")
TH1D* H2 = F->Get("Jpsi2MuMu/2")
TH1D* H3 = F->Get("Jpsi2MuMu/3")
TH1D* H4 = F->Get("Jpsi2MuMu/4")
TH1D* H5 = F->Get("Jpsi2MuMu/5")
TH1D* H6 = F->Get("Jpsi2MuMu/6")
TH1D* H7 = F->Get("Jpsi2MuMu/7")
TH1D* H8 = F->Get("Jpsi2MuMu/8")
TH1D* H9 = F->Get("Jpsi2MuMu/9")
TH1D* H10 = F->Get("Jpsi2MuMu/10")
TH1D* H11 = F->Get("Jpsi2MuMu/11")

H5->SetLineColor(2)
H5->SetLineWidth(3)
H5->Draw()
gPad->SaveAs("H5.eps");

H6->SetLineColor(2)
H6->SetLineWidth(3)
H6->Draw()
gPad->SaveAs("H6.eps");

H1->SetLineColor(2)
H1->SetLineWidth(3)
H1->Draw()
H8->SetLineColor(4)
H8->SetLineWidth(3)
H8->Draw("Same")
gPad->SaveAs("H8.eps");

H2->SetLineColor(2)
H2->SetLineWidth(3)
H2->Draw()
H9->SetLineColor(4)
H9->SetLineWidth(3)
H9->Draw("Same")
gPad->SaveAs("H9.eps");

H3->SetLineColor(2)
H3->SetLineWidth(3)
H3->Draw()
H10->SetLineColor(4)
H10->SetLineWidth(3)
H10->Draw("Same")
gPad->SaveAs("H10.eps");

H4->SetLineColor(2)
H4->SetLineWidth(3)
H4->Draw()
H11->SetLineColor(4)
H11->SetLineWidth(3)
H11->Draw("Same")
gPad->SaveAs("H11.eps");

