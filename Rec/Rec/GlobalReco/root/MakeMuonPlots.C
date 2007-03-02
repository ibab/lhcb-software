
{

  // load the file
  TFile * f = new TFile("gpid-1000evts.root");

  TTree * tree = (TTree*)gDirectory->Get("ChargedProtoTuple/protoPtuple");

  TCut detOK = "MuonInAcc==1";

  TCut trackSel = "TrackType == 3 && TrackP > 2000 && TrackP < 100000";

  TCut realM = "fabs(MCParticleType) == 13";
  TCut fakeM = "fabs(MCParticleType) != 13";

  //new TCanvas();
  //tree->Draw( "MuonMuLL-MuonBkgLL", trackSel && detOK );

  //new TCanvas();
  //tree->Draw( "MuonMuLL", trackSel && detOK && realM );

  //new TCanvas();
  //tree->Draw( "MuonMuLL", trackSel && detOK && fakeM );

  //new TCanvas();
  //tree->Draw( "MuonBkgLL", trackSel && detOK && realM );

  //new TCanvas();
  //tree->Draw( "MuonBkgLL", trackSel && detOK && fakeM );

  //new TCanvas();
  //tree->Draw( "MuonMuLL-MuonBkgLL", trackSel && detOK && realM );

  new TCanvas();
  tree->Draw( "MuonNShared", trackSel && detOK );

  //new TCanvas();
  //tree->Draw( "CombDLLmu-CombDLLpi", trackSel && detOK && realM );

  //new TCanvas();
  //tree->Draw( "CombDLLmu-CombDLLpi", trackSel && detOK && fakeM );

}
