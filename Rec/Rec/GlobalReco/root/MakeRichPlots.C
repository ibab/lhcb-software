
{

  // load the file
  TFile * f = new TFile("protoparticles.tuples.root");

  TTree * tree = (TTree*)gDirectory->Get("ChargedProtoTuple/protoPtuple");

  TCut detOK = "RichUsedAero || RichUsedR1Gas || RichUsedR2Gas";

  const std::string trackSelS = "TrackType == 3 && TrackP > 10000 && TrackP < 100000 && TrackPt > 500";
  TCut trackSel = trackSelS.c_str();

  TCut realPr = "abs(MCParticleType) == 2212";
  TCut realK  = "abs(MCParticleType) == 321";
  TCut realPi = "abs(MCParticleType) == 211";

  TCut prAboveThres = "RichAbovePrThres";
  TCut kAboveThres  = "RichAboveKaThres";
  TCut piAboveThres = "RichAbovePiThres";

  TCanvas * c = new TCanvas( "RICH", "RICH", 1400, 1000 );

  std::vector<double> cuts;
  cuts.push_back(-5);
  cuts.push_back(-2.5);
  cuts.push_back(0);
  cuts.push_back(2.5);
  cuts.push_back(5);

  // Kaon ID plots
  for ( std::vector<double>::const_iterator iC = cuts.begin();
        iC != cuts.end(); ++iC )
  {
    std::ostringstream cC; 
    cC << *iC;

    tree->Draw( ("(RichDLLk>"+cC.str()+"?100:0):TrackP>>kIDEff").c_str(),     
                detOK && realK  && trackSel && kAboveThres, "prof" );
    tree->Draw( ("(RichDLLk>"+cC.str()+"?100:0):TrackP>>piMisIDEff").c_str(), 
                detOK && realPi && trackSel && piAboveThres, "prof" );

    kIDEff->SetTitle( ( "DLLk>"+cC.str() + " | " + trackSelS ).c_str() );
    kIDEff->GetXaxis()->SetTitle( "Momentum / MeV/c" );
    kIDEff->GetYaxis()->SetTitle( "Efficiency / %" );
    kIDEff->Draw();
    kIDEff->SetMarkerColor(kRed);
    kIDEff->SetLineColor(kRed);

    piMisIDEff->Draw("SAME");
    piMisIDEff->SetMarkerColor(kBlue);
    piMisIDEff->SetLineColor(kBlue);

    c->SaveAs( ("KaonID-DLL"+cC.str()+".png").c_str() );

  }

  // Proton ID plots
  for ( std::vector<double>::const_iterator iC = cuts.begin();
        iC != cuts.end(); ++iC )
  {
    std::ostringstream cC; 
    cC << *iC;

    tree->Draw( ("(RichDLLp>"+cC.str()+"?100:0):TrackP>>prIDEff").c_str(),     
                detOK && realPr && trackSel && prAboveThres, "prof" );
    tree->Draw( ("(RichDLLp>"+cC.str()+"?100:0):TrackP>>piMisIDEff").c_str(), 
                detOK && realPi && trackSel && piAboveThres, "prof" );

    prIDEff->SetTitle( ( "DLLp>"+cC.str() + " | " + trackSelS ).c_str() );
    prIDEff->GetXaxis()->SetTitle( "Momentum / MeV/c" );
    prIDEff->GetYaxis()->SetTitle( "Efficiency / %" );
    prIDEff->Draw();
    prIDEff->SetMarkerColor(kRed);
    prIDEff->SetLineColor(kRed);

    piMisIDEff->Draw("SAME");
    piMisIDEff->SetMarkerColor(kBlue);
    piMisIDEff->SetLineColor(kBlue);

    c->SaveAs( ("ProtonID-DLL"+cC.str()+".png").c_str() );

  }

}
