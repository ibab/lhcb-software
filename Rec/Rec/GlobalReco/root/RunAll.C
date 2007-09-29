
{
  const Long64_t nTracks = 100;

  // Out-the-box v30r14
  //const std::string dataFile = "/usera/jonesc/RICH/RootFiles/Brv30r14-PIDCheck.tuples.root";
  // Out-the-box v31r8
  //const std::string dataFile = "/usera/jonesc/RICH/RootFiles/Brv31r8-PIDCheck.tuples.root";
  // Out-the-box v31r9
  const std::string dataFile = "/usera/jonesc/RICH/RootFiles/Brv31r9-PIDCheck.tuples.root";

  //gROOT->ProcessLine(".x Calo-ElectronID.C");
  gROOT->ProcessLine(".x Combined-ElectronID-2-100GeV.C");
  //gROOT->ProcessLine(".x Combined-ElectronID-2-10GeV.C");
  //gROOT->ProcessLine(".x Combined-ElectronID-10-50GeV.C");

  //gROOT->ProcessLine(".x Rich-KaonID.C");
  //gROOT->ProcessLine(".x Rich-KaonID-withProtonCut.C");
  //gROOT->ProcessLine(".x Combined-KaonID.C");
  //gROOT->ProcessLine(".x Combined-KaonID-withElectronCut.C");
  //gROOT->ProcessLine(".x Combined-KaonID-withMuonCut.C");
  //gROOT->ProcessLine(".x Combined-ProtonID.C");

  //gROOT->ProcessLine(".x Combined-MuonID.C");

  //gROOT->ProcessLine(".x CompareTwoFiles-KaonID.C");
 
}
