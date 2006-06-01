TCanvas* c1 = new TCanvas("c1","Bologna",800,800);
c1->SetLogy();

TFile* F = new TFile("DVHistos-BB.root") 
F->ls()
F->cd("TutorialAlgorithm")               
F->ls() 
TH1D* H1 = F->Get("TutorialAlgorithm/1")
TH1D* H2 = F->Get("TutorialAlgorithm/2")
TH1D* H3 = F->Get("TutorialAlgorithm/3")
TH1D* H4 = F->Get("TutorialAlgorithm/4")

H1->SetLineColor(2)
H1->SetLineWidth(3)
H1->Draw()
gPad->SaveAs("H1.eps");

H2->SetLineColor(2)
H2->SetLineWidth(3)
H2->Draw()
gPad->SaveAs("H2.eps");

H3->SetLineColor(2)
H3->SetLineWidth(3)
H3->Draw()
gPad->SaveAs("H3.eps");

H4->SetLineColor(2)
H4->SetLineWidth(3)
H4->Draw()
gPad->SaveAs("H4.eps");
