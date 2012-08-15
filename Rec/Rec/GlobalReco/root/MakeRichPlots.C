
{

  // load the file
  TFile * f = new TFile("protoparticles.tuples.root");

  TTree * tree = (TTree*)gDirectory->Get("ChargedProtoTuple/protoPtuple");

  TCut detOK = "RichUsedAero == 1 || RichUsedR1Gas == 1 || RichUsedR2Gas == 1";

  TCut trackSel = "TrackType == 3 && TrackP > 10000 && TrackP < 100000 && TrackPt > 1000 ";

  TCut realK  = "abs(MCParticleType) == 321";
  TCut fakeK  = "abs(MCParticleType) != 321";

  TCut realPi = "abs(MCParticleType) == 211";
  TCut fakePi = "abs(MCParticleType) != 211";

  TCut kAboveThres  = "RichAboveKaThres == 1";
  TCut piAboveThres = "RichAbovePiThres == 1";

  TCanvas * c = new TCanvas( "RICH", "RICH", 1400, 1000 );

  std::vector<double> cuts;
  cuts.push_back(-5);
  cuts.push_back(0);
  cuts.push_back(5);

  for ( std::vector<double>::const_iterator iC = cuts.begin();
        iC != cuts.end(); ++iC )
  {
    std::ostringstream cC; 
    cC << *iC;

    tree->Draw( ("(RichDLLk>"+cC.str()+"?100:0):TrackP>>kIDEff").c_str(),     
                detOK && realK  && trackSel && kAboveThres, "prof" );
    tree->Draw( ("(RichDLLk>"+cC.str()+"?100:0):TrackP>>piMisIDEff").c_str(), 
                detOK && realPi && trackSel && piAboveThres, "prof" );

    kIDEff->SetTitle( ("DLLk>"+cC.str()).c_str() );
    kIDEff->Draw();
    kIDEff->SetMarkerColor(kRed);
    kIDEff->SetLineColor(kRed);

    piMisIDEff->Draw("SAME");
    piMisIDEff->SetMarkerColor(kBlue);
    piMisIDEff->SetLineColor(kBlue);

    c->SaveAs( ("KaonID-DLL"+cC.str()+".png").c_str() );

  }

}
