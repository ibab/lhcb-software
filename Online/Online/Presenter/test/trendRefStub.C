// GauchoJob/MonitorSvc/monRate/Counternumber2
// GauchoJob/MonitorSvc/monRate/Eventsaccepted-flag
// GauchoJob/MonitorSvc/monRate/Eventsaccepted-nEntries
// GauchoJob/MonitorSvc/monRate/NUMBER_OF_PROCESSES
// GauchoJob/MonitorSvc/monRate/Counternumber1
// GauchoJob/MonitorSvc/monRate/NUMBER_OF_PROCESSES

void trendRefStub() {
TFile *refFile = new TFile("TCK_1.root","recreate");

// create a subdirectory for each algo ...for "new" fileformat
TDirectory *algo1= refFile->mkdir("GauchoJob");
 algo1->cd(); 
TDirectory *algo2= algo1->mkdir("MonitorSvc");
 algo2->cd(); 
TDirectory *algo3= algo2->mkdir("monRate");
 algo3->cd(); 

const Int_t nbins = 200;
const Double_t refValue1 = 1.5;
const Double_t refValue2 = 2.3;
TH1D* h1 = new TH1D("Counternumber1","GauchoJob/MonitorSvc/monRate/Counternumber1",nbins,0,nbins);
TH1D* h2 = new TH1D("Counternumber2","GauchoJob/MonitorSvc/monRate/Counternumber2",nbins,0,nbins);
Int_t i;
for (i=1 ; i < nbins; i++) {
  h1->SetBinContent(i, refValue1);
  h2->SetBinContent(i, refValue2);
}   
refFile->Write();
delete refFile;
}

