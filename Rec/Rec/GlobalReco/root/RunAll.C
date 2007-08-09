
{
  const Long64_t nTracks = -1;

  const std::string dataFile = "/var/work/pcgs/jonesc/fastreco/Offline-AllRads.tuples.root";

  //gROOT->ProcessLine(".x Calo-ElectronID.C");
  //gROOT->ProcessLine(".x Combined-ElectronID-2-100GeV.C");
 
  //gROOT->ProcessLine(".x Rich-KaonID.C");
  //gROOT->ProcessLine(".x Rich-KaonID-withProtonCut.C");
  //gROOT->ProcessLine(".x Combined-KaonID.C");
  //gROOT->ProcessLine(".x Combined-KaonID-withElectronCut.C");
  //gROOT->ProcessLine(".x Combined-KaonID-withMuonCut.C");
  //gROOT->ProcessLine(".x Combined-ProtonID.C");
  gROOT->ProcessLine(".x CompareTwoFiles-KaonID.C");

  //gROOT->ProcessLine(".x Combined-MuonID.C");
 
}
