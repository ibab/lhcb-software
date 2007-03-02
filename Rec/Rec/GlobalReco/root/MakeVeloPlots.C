
{

  // load the file
  TFile * f = new TFile("gpid-1000evts.root");

  TTree * tree = (TTree*)gDirectory->Get("ChargedProtoTuple/protoPtuple");

  TCut detOK = "RichUsedAero==1 || RichUsedR1Gas==1 || RichUsedR2Gas==1";

  TCut trackSel = "TrackType == 3 && TrackP > 2000 && TrackP < 100000";

  TCut realK = "fabs(MCParticleType) == 321";
  TCut fakeK = "fabs(MCParticleType) != 321";

  TCut realE = "fabs(MCParticleType) == 11";
  TCut fakeE = "fabs(MCParticleType) != 11";

  TCut realMu = "fabs(MCParticleType) == 13";
  TCut fakeMu = "fabs(MCParticleType) != 13";

  TCut realPi = "fabs(MCParticleType) == 211";
  TCut fakePi = "fabs(MCParticleType) != 211";

  TCut realPr = "fabs(MCParticleType) == 2212";
  TCut fakePr = "fabs(MCParticleType) != 2212";

  new TCanvas();
  tree->Draw( "VeloCharge", trackSel && detOK && realE );

  new TCanvas();
  tree->Draw( "VeloCharge", trackSel && detOK && realMu );

  new TCanvas();
  tree->Draw( "VeloCharge", trackSel && detOK && realPi );

  new TCanvas();
  tree->Draw( "VeloCharge", trackSel && detOK && realK );

  new TCanvas();
  tree->Draw( "VeloCharge", trackSel && detOK && fakePr );

}
