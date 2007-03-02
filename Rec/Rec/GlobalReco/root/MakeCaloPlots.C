
{

  // load the file 
  TFile * f = new TFile("gpid-1000evts.root");

  TTree * tree = (TTree*)gDirectory->Get("ChargedProtoTuple/protoPtuple");

  TCut ecalOK = "InAccEcal == 1";
  TCut hcalOK = "InAccHcal == 1";
  TCut prsOK  = "InAccPrs == 1";
  TCut bremOK = "InAccBrem == 1";
  
  TCut trackSel = "TrackType == 3 && TrackP > 2000 && TrackP < 100000";

  TCut realEl = "fabs(MCParticleType) == 11";
  TCut fakeEl = "fabs(MCParticleType) != 11";

  TCut realMu = "fabs(MCParticleType) == 13";
  TCut fakeMu = "fabs(MCParticleType) != 13";

  //new TCanvas();
  //tree->Draw( "EcalPIDe+PrsPIDe", trackSel && ecalOK && prsOK );
  //tree->Draw( "EcalPIDe+HcalPIDe+PrsPIDe+BremPIDe", trackSel && ecalOK && prsOK && realEl, "same" );

  //new TCanvas();
  //tree->Draw( "EcalPIDe", trackSel && ecalOK && fakeEl );
  //tree->Draw( "EcalPIDe", trackSel && ecalOK && realEl,"same" );

  //new TCanvas();
  //tree->Draw( "HcalPIDe", trackSel && hcalOK && fakeEl );

  //new TCanvas();
  //tree->Draw( "HcalPIDe", trackSel && hcalOK && realEl );

  //new TCanvas();
  //tree->Draw( "PrsPIDe", trackSel && prsOK && realEl );

  //new TCanvas();
  //tree->Draw( "PrsPIDe", trackSel && prsOK && fakeEl );

  //new TCanvas();
  //tree->Draw( "BremPIDe", trackSel && bremOK && realEl );

  //new TCanvas();
  //tree->Draw( "BremPIDe", trackSel && bremOK && fakeEl );

  //new TCanvas();
  //tree->Draw( "EcalPIDmu", trackSel && ecalOK && realMu );

  //new TCanvas();
  //tree->Draw( "EcalPIDmu", trackSel && ecalOK && fakeMu );

  //new TCanvas();
  //tree->Draw( "HcalPIDmu", trackSel && hcalOK && realMu );

  //new TCanvas();
  //tree->Draw( "HcalPIDmu", trackSel && hcalOK && fakeMu );

  new TCanvas();
  tree->Draw( "EcalPIDmu+HcalPIDmu", trackSel && hcalOK && ecalOK );

}
