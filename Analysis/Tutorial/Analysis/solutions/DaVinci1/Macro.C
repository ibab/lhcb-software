TCanvas* c1 = new TCanvas("c1","DaVinci",600,600);
c1->SetLogy();

TFile* F = new TFile("DVHistos_1.root") 
F->ls()
F->cd("TutorialAlgorithm")               
F->ls() 
TH1D* H1 = F->Get("TutorialAlgorithm/P")
TH1D* H2 = F->Get("TutorialAlgorithm/Pt")
TH1D* H3 = F->Get("TutorialAlgorithm/IP")
TH1D* H4 = F->Get("TutorialAlgorithm/IPS")

H1->SetLineColor(2)
H1->SetLineWidth(3)
H1->Draw()
gPad->SaveAs("DV1_P.eps");

H2->SetLineColor(2)
H2->SetLineWidth(3)
H2->Draw()
gPad->SaveAs("DV1_Pt.eps");

H3->SetLineColor(2)
H3->SetLineWidth(3)
H3->Draw()
gPad->SaveAs("DV1_IP.eps");

H4->SetLineColor(2)
H4->SetLineWidth(3)
H4->Draw()
gPad->SaveAs("DV1_IPS.eps");
