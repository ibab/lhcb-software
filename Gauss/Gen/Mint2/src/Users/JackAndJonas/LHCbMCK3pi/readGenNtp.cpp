
#include "readGenNtp.h"
#include "TLorentzVector.h"
#include "TRandom.h"

#include <iostream>
#include <algorithm>
#include <vector>

#include "Mint/DalitzEventPattern.h"
#include "Mint/DalitzEvent.h"

#include "Mint/CLHEPSystemOfUnits.h"

using namespace std;
using namespace MINT;

readGenNtp::readGenNtp( std::string fname
				      , std::string ntpName
				      , long int maxEvents
				      , const std::string& weightBranchName
				      )
  : _file0(0)
  , _oldTree(0)
  , _tree(0)
  , _fname(fname)
  , _ntpName(ntpName)
  , _maxEvents(maxEvents)
  , _weightBranchName(weightBranchName)
{
  getTree();
  //  attachit();
}
readGenNtp::~readGenNtp(){
  //  save();
  _file0->Close();
}

std::string readGenNtp::newFilename() const{
  string newFilename(_fname);
  newFilename.insert(newFilename.find_last_of('.'), "_reweighted");
  return newFilename;
}

void readGenNtp::save(){
  cout << "hello from readGenNtp::save() " << _file0 << endl;
  _file0->cd();
  cout << "cd'ed to file" << endl;
  if(0 != _tree){
    cout << "writing tree" << endl;
    _tree->Write();//"", TObject::kOverwrite);
  }else{
    cout << "destructur of readGenNtp, saving " << _fname << endl;
    cout << " ... _tree dead.!" << endl;
  }
}

TBranch* readGenNtp::setBranch(const std::string& name, float* ptr){
  if(0 == _tree) return 0;
  TBranch* br = (TBranch*) _tree->FindBranch(name.c_str());
  if(0 != br){
    _tree->SetBranchAddress(name.c_str(), ptr);
    return br;
  }
  br = _tree->Branch(name.c_str(), ptr, name.c_str());
  return br;
}

bool readGenNtp::getTree(){
  cout << "hello from readGenNtp::getTree" << endl;
  cout << "about to open file " << _fname << endl;
  TFile* f = TFile::Open(_fname.c_str(), "READ");
  if(0 == f) return false;
  cout << " file ptr " << f << endl;
  cout << " will now retrieve tree with name " << _ntpName << endl;
  _oldTree = dynamic_cast<TTree*>(f->Get(_ntpName.c_str()));
  cout << "tree pointer: " << _oldTree << endl;
  _oldTree->SetBranchStatus("*", 1);
  attachit();

  cout << "readGenNtp::getTree(): Reading from " << _fname 
       << ", writing to: " << newFilename() << endl;
  _file0 = TFile::Open(newFilename().c_str(), "RECREATE");
  cout << "opened new file " << _file0 << endl;
  if(0 == _file0) return false;
  _file0->cd();
  cout << "cd'ed to new file " << endl;
  _tree = _oldTree->CloneTree(0);
  _tree->SetDirectory(_file0);
  setBranch(_weightBranchName.c_str(), &_modelWeight);
  //_tree->CopyEntries(ot, _maxEvents);
  cout << "cloned tree from old file" << endl;
  return (0 != _tree);
}

MINT::counted_ptr<DalitzEvent> 
readGenNtp::readEntry(unsigned int entry){
  // we read the MC-truth 4-momenta as we want to re-weight
  // according to the trugh.
  // Things to cut on are read from reconstructed data.
  _oldTree->GetEntry(entry);
  _modelWeight = -1;


  if(1 != _prompt){
    counted_ptr<DalitzEvent> zeroPtr(0);
    return zeroPtr;
  }


  // we'll fill it according to the event pattern 
  // 421 -> -321 +211 -211 +211
  DalitzEventPattern pat(421, -321, +211, -211, +211);

  // if we encounter +321, we'll reverse all charges.
  TLorentzVector K(_K_p4arr);
  TLorentzVector Pi0(_Pi0_p4arr);
  TLorentzVector Pi1(_Pi1_p4arr);
  TLorentzVector Pi2(_Pi2_p4arr);
  
  vector<TLorentzVector> PArray(5);

  TLorentzVector* pD   = &(PArray[0]);
  TLorentzVector* Km   = &(PArray[1]);
  TLorentzVector* pip1 = &(PArray[2]);
  TLorentzVector* pim  = &(PArray[3]);
  TLorentzVector* pip2 = &(PArray[4]);

  int pdgArray[5];
  
  if(_K_pdg < 0) pdgArray[0] = 421;
  else pdgArray[0] = - 421;

  int *Km_pdg   = &pdgArray[1];
  int *pip1_pdg = &pdgArray[2];
  int *pim_pdg  = &pdgArray[3];
  int *pip2_pdg = &pdgArray[4];

  *Km = K;
  *Km_pdg = _K_pdg;

  if(_K_pdg * _Pi0_pdg > 0){
    *pim  = Pi0;   *pim_pdg  = _Pi0_pdg;
    *pip1 = Pi1;   *pip1_pdg = _Pi1_pdg;
    *pip2 = Pi2;   *pip2_pdg = _Pi2_pdg;
  }else{
    *pip1 =Pi0;    *pip1_pdg = _Pi0_pdg;
    if(_K_pdg * _Pi1_pdg > 0){
      *pim  = Pi1;   *pim_pdg  = _Pi1_pdg;
      *pip2 = Pi2;   *pip2_pdg = _Pi2_pdg;
    }else{
      *pim  = Pi2;   *pim_pdg  = _Pi2_pdg;
      *pip2 = Pi1;   *pip2_pdg = _Pi1_pdg;
    }
  }
  *pD = PArray[1] +  PArray[2] +  PArray[3] + PArray[4];

  //for(int i=0; i < 5; i++) PArray[i] *= GeV;
  // randomise pi+
  if(gRandom->Rndm() >0.5){
    swap(*pip1, *pip2);
    swap(*pip1_pdg, *pip2_pdg);
  }
  if(entry < 5){
    for(unsigned int i = 0; i < 5; i++){
      cout << " mass " << i << ") " << PArray[i].M() 
	   << ", pdg " << pdgArray[i] << endl;
    }
  }
  
  _pD.SetXYZT(_px, _py, _pz, _pe);

  counted_ptr<DalitzEvent> evtPtr(new DalitzEvent(pat, PArray));
  return evtPtr;
}

bool readGenNtp::attachit(){
  _oldTree->SetBranchAddress("K_px",&_K_p4arr[0]);
  _oldTree->SetBranchAddress("K_py",&_K_p4arr[1]);
  _oldTree->SetBranchAddress("K_pz",&_K_p4arr[2]);
  _oldTree->SetBranchAddress("K_e",&_K_p4arr[3]);
  _oldTree->SetBranchAddress("K_pdg",&_K_pdg);
  //_K_pdg = -321;

  _oldTree->SetBranchAddress("Pi0_px",&_Pi0_p4arr[0]);
  _oldTree->SetBranchAddress("Pi0_py",&_Pi0_p4arr[1]);
  _oldTree->SetBranchAddress("Pi0_pz",&_Pi0_p4arr[2]);
  _oldTree->SetBranchAddress("Pi0_e",&_Pi0_p4arr[3]);
  _oldTree->SetBranchAddress("Pi0_pdg",&_Pi0_pdg);
  //_Pi0_pdg = 211;

  _oldTree->SetBranchAddress("Pi1_px",&_Pi1_p4arr[0]);
  _oldTree->SetBranchAddress("Pi1_py",&_Pi1_p4arr[1]);
  _oldTree->SetBranchAddress("Pi1_pz",&_Pi1_p4arr[2]);
  _oldTree->SetBranchAddress("Pi1_e",&_Pi1_p4arr[3]);
  _oldTree->SetBranchAddress("Pi1_pdg",&_Pi1_pdg);
  //_Pi1_pdg = -211;

  _oldTree->SetBranchAddress("Pi2_px",&_Pi2_p4arr[0]);
  _oldTree->SetBranchAddress("Pi2_py",&_Pi2_p4arr[1]);
  _oldTree->SetBranchAddress("Pi2_pz",&_Pi2_p4arr[2]);
  _oldTree->SetBranchAddress("Pi2_e",&_Pi2_p4arr[3]);
  _oldTree->SetBranchAddress("Pi2_pdg",&_Pi2_pdg);
  //_Pi2_pdg = 211;

  _oldTree->SetBranchAddress("prompt",&_prompt);

  //_oldTree->SetBranchAddress("modelWeight",&_modelWeight);
  //  _modelWeightBranch = setBranch("modelWeight", &_modelWeight);

  cout << " attached tree" << endl;
  return true;
}

void readGenNtp::setModelWeight(double w){
  _modelWeight = w;
  _tree->Fill();
  //_modelWeightBranch->Fill();
}

bool readGenNtp::passCuts(){
  // always returns true, for now - cuts in WeightedCount::doCount().
  //if(_ycm < 1.0 || _ycm > 5.5) return false;

  return true;
}



unsigned int readGenNtp::numEvents() const{
  int n= _oldTree->GetEntries();
  if(_maxEvents < 0 || n < _maxEvents) return n;
  else return _maxEvents;
}
bool readGenNtp::readit(DiskResidentEventList* listPtr, int maxEvents){
  if(0 == listPtr) return false;

  int numEvents = 0;
  for(unsigned int i=0; i < _oldTree->GetEntries(); i++){
    counted_ptr<DalitzEvent> evtPtr = readEntry(i);
    //    setModelWeight(-9999);
    if(! evtPtr) continue;
    if(! passCuts()) continue;
    numEvents++;
    listPtr->Add(*(evtPtr.get()));
    if(maxEvents > 0 && numEvents > maxEvents) break;
  }
  //_listPtr->Start();
  //_listPtr->Next();
  return true;
}
bool readGenNtp::readit(DalitzEventList* listPtr, int maxEvents){
  if(0 == listPtr) return false;
  cout << "_oldTree->GetEntries() " << _oldTree->GetEntries()
       << " maxEvents " << maxEvents << endl;
  int numEvents = 0;
  for(unsigned int i=0; i < _oldTree->GetEntries(); i++){
    counted_ptr<DalitzEvent> evtPtr = readEntry(i);
    //    setModelWeight(-9999);
    if(! evtPtr) continue;
    if(! passCuts()) continue;
    numEvents++;
    listPtr->Add(*(evtPtr.get()));
    if(maxEvents > 0 && numEvents > maxEvents) break;
  }
  //_listPtr->Start();
  //_listPtr->Next();
  return true;
}

