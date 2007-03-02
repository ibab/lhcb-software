
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

  new TCanvas();
  tree->Draw( "7*(tanh(RichDLLk/40)-tanh(RichDLLpi/50))", trackSel && detOK && realK );

  new TCanvas();
  tree->Draw( "7*(tanh(RichDLLk/40)-tanh(RichDLLpi/50))", trackSel && detOK && fakeK );

  new TCanvas();
  tree->Draw( "RichDLLk-RichDLLpi", trackSel && detOK && realK );

  new TCanvas();
  tree->Draw( "RichDLLk-RichDLLpi", trackSel && detOK && fakeK );

  //new TCanvas();
  //tree->Draw( "tanh((RichDLLk-RichDLLpi)/100)", trackSel && detOK && realK );

  //new TCanvas();
  //tree->Draw( "tanh((RichDLLk-RichDLLpi)/100)", trackSel && detOK && fakeK );

  //new TCanvas();
  //tree->Draw( "RichDLLmu-RichDLLpi", trackSel && detOK );

  //new TCanvas();
  //tree->Draw( "7*(tanh(RichDLLmu/10)-tanh(RichDLLpi/10))", trackSel && detOK );

  //new TCanvas();
  //tree->Draw( "RichDLLe-RichDLLpi", trackSel && detOK );

  //new TCanvas();
  //tree->Draw( "RichDLLe-RichDLLpi", trackSel && detOK && fakeE );

  //new TCanvas();
  //tree->Draw( "7*(tanh(RichDLLe/40)-tanh(RichDLLpi/40))", trackSel && detOK );

  //new TCanvas();
  //tree->Draw( "7*(tanh(RichDLLe/40)-tanh(RichDLLpi/40))", trackSel && detOK && fakeE );


}
