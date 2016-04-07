

void MakeANNReweighting()
{

  // load the data and MC ANN(data/MC) responses
  
  auto * file = TFile::Open( "output-100k/2012DataMCDiff-Long-TMVA.root" );

  auto * dataH = (TH1F*)gDirectory->Get("Method_MLP/MLP-BP-CE/MVA_MLP-BP-CE_S");
  if ( !dataH ) return;
  dataH->SetMarkerColor(kRed);
  dataH->SetLineColor(kRed);
  const auto dataEn = dataH->GetEntries();
  dataH->Scale( 1.0/dataEn );

  dataH->SetTitle( "DataMC Comparison MVA Response" );

  auto * mcH = (TH1F*)gDirectory->Get("Method_MLP/MLP-BP-CE/MVA_MLP-BP-CE_B");
  if ( !mcH ) return;
  mcH->SetMarkerColor(kBlue);
  mcH->SetLineColor(kBlue);
  const auto mcEn = mcH->GetEntries();
  mcH->Scale( 1.0/mcEn );

  TCanvas * c = new TCanvas( "RICH", "RICH", 1400, 1000 );
  gStyle->SetOptStat(0);

  dataH->Draw();
  mcH->Draw("SAME");

  // Legion for plot
  TLegend * legend = new TLegend(0.885,0.9,0.935,0.85);
  legend->SetTextSize(0.02);
  legend->SetMargin(0.12);
  legend->AddEntry( dataH, "Data", "p" );
  legend->AddEntry( mcH, "MC", "p" );
  legend->Draw();

  c->SaveAs( "DataMCANNN-Response.pdf" );

  dataH->Divide(mcH);
  dataH->SetTitle( "Data/MC Comparison MVA Response" );

  dataH->Scale( mcEn/dataEn );
  const auto norma = dataH->GetBinContent(dataH->GetMaximumBin());
  //dataH->Scale( 1.0/norma );

  dataH->Fit("pol8");

  c->SetLogy(true);
  dataH->Draw();
  c->SaveAs( "DataMCANNN-DataOverMC.pdf" );






}
