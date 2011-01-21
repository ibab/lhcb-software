void rootlogon(){
  //  gSystem->Load("/home/hep/mneedham/tbeam/STtbana/libEvent");
  TStyle *plain  = new TStyle("Plain","Plain Style (no colors/fill areas)");

  //  plain->SetCanvasBorderMode(0);
  // plain->SetPadBorderMode(0);
 //plain->SetPadColor(0);
  //plain->SetCanvasColor(0);
  //plain->SetTitleColor(0);
  //plain->SetTitleStyle(0);
  //plain->SetStatColor(0); 
  //plain->SetStatStyle(0);
  plain->SetOptStat(0);
  plain->SetOptFit(10);
  gROOT->SetStyle("Plain");
  gROOT->ForceStyle();
}

