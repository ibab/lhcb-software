TCanvas* c1 = new TCanvas("c1","DaVinci",600,600);
c1->SetLogy();

TFile* F = new TFile("DVHistos_2.root") 
F->ls()
F->cd("Jpsi2MuMu")               
F->ls() 
TH1D* H1 = F->Get("Jpsi2MuMu/AllMuP")
TH1D* H2 = F->Get("Jpsi2MuMu/AllMuPt")
TH1D* H3 = F->Get("Jpsi2MuMu/AllMuIP")
TH1D* H4 = F->Get("Jpsi2MuMu/AllMuIPS")
TH1D* H5 = F->Get("Jpsi2MuMu/DiMuM")
TH1D* H6 = F->Get("Jpsi2MuMu/DiMuChi2")
TH1D* H7 = F->Get("Jpsi2MuMu/SelDiMuChi2")
TH1D* H8 = F->Get("Jpsi2MuMu/SelectedMuP")
TH1D* H9 = F->Get("Jpsi2MuMu/SelectedMuPt")
TH1D* H10 = F->Get("Jpsi2MuMu/SelectedMuIP")
TH1D* H11 = F->Get("Jpsi2MuMu/SelectedMuIPS")

H5->SetLineColor(2)
H5->SetLineWidth(3)
H5->Draw()
gPad->SaveAs("DV2_H5.eps");

H6->SetLineColor(2)
H6->SetLineWidth(3)
H6->Draw()
gPad->SaveAs("DV2_H6.eps");

H1->SetLineColor(2)
H1->SetLineWidth(3)
H1->Draw()
H8->SetLineColor(4)
H8->SetLineWidth(3)
H8->Draw("Same")
gPad->SaveAs("DV2_H8.eps");

H2->SetLineColor(2)
H2->SetLineWidth(3)
H2->Draw()
H9->SetLineColor(4)
H9->SetLineWidth(3)
H9->Draw("Same")
gPad->SaveAs("DV2_H9.eps");

H3->SetLineColor(2)
H3->SetLineWidth(3)
H3->Draw()
H10->SetLineColor(4)
H10->SetLineWidth(3)
H10->Draw("Same")
gPad->SaveAs("DV2_H10.eps");

H4->SetLineColor(2)
H4->SetLineWidth(3)
H4->Draw()
H11->SetLineColor(4)
H11->SetLineWidth(3)
H11->Draw("Same")
gPad->SaveAs("DV2_H11.eps");

