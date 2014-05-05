
#include "readPhilipsNtuple.h"
#include "TLorentzVector.h"
#include "TRandom.h"

#include <iostream>
#include <algorithm>
#include <vector>

#include "Mint/DalitzEventPattern.h"
#include "Mint/DalitzEvent.h"

#include "Mint/CLHEPSystemOfUnits.h"
#include "Mint/Utils.h"

using namespace std;
using namespace MINT;

readPhilipsNtuple::readPhilipsNtuple( std::string fname
				      , std::string ntpName
				      , long int maxEvents
				      , const std::string& weightBranchBaseName
				      , int numWeightBranches
				      )
  : _file0(0)
  , _oldTree(0)
  , _tree(0)
  , _fname(fname)
  , _ntpName(ntpName)
  , _maxEvents(maxEvents)
  , _weightBranchBaseName(weightBranchBaseName)
  , _modelWeightArray(0)
  , _numWeightBranches(numWeightBranches)
    //  , _weightNumber(0)
{
  cout << "Hello from readPhilipsNtuple's constructor" << endl;
  _modelWeightArray = new Float_t[max(_numWeightBranches, 1)];
  _BranchPointerArray = new TBranch*[max(_numWeightBranches, 1)];
  for(int i=0; i < max(_numWeightBranches, 1); i++){
    _modelWeightArray[i] = -9999;
    _BranchPointerArray[i] = 0;
  }
  //  setWeightNumber(0);

  getTree();
  cout << "... got tree" << endl;
  //  attachit();
}
readPhilipsNtuple::~readPhilipsNtuple(){
  //  save();
  _file0->Close();
  delete[] _modelWeightArray;
  delete[] _BranchPointerArray;
}


// void readPhilipsNtuple::setWeightNumber(int n){
//   TBranch* prevBranch = getBranch();
//   if(0 != prevBranch)  _tree->ResetBranchAddress(prevBranch);
//   _BranchPointerArray[weightNumber()]=0;

//   if(n < 0 || n >= _numWeightBranches) _weightNumber=0;
//   else _weightNumber=n;
//   _BranchPointerArray[weightNumber()] = setBranch(( weightBranchName()).c_str(), &(modelWeight()));
//   //setBranch((_weightBranchBaseName + postFix).c_str(), &(modelWeight()));
// }

Float_t& readPhilipsNtuple::modelWeight(int i){
  return _modelWeightArray[i];
}
const Float_t& readPhilipsNtuple::modelWeight(int i) const{
  return _modelWeightArray[i];
}

std::string readPhilipsNtuple::newFilename() const{
  string newFilename(_fname);
  newFilename.insert(newFilename.find_last_of('.'), "_reweighted_April2012");
  return newFilename;
}

void readPhilipsNtuple::save(){
  cout << "hello from readPhilipsNtuple::save() " << _file0 << endl;
  _file0->cd();
  cout << "cd'ed to file" << endl;
  if(0 != _tree){
    cout << "writing tree" << endl;
    _tree->Write();//"", TObject::kOverwrite);
  }else{
    cout << "destructor of readPhilipsNtuple, saving " << _fname << endl;
    cout << " ... _tree dead.!" << endl;
  }
}

TBranch* readPhilipsNtuple::setBranch(const std::string& name, Float_t* ptr){
  if(0 == _tree) return 0;
  TBranch* br = (TBranch*) _tree->FindBranch(name.c_str());
  if(0 != br){
    _tree->SetBranchAddress(name.c_str(), ptr);
    return br;
  }
  br = _tree->Branch(name.c_str(), ptr, name.c_str());
  return br;
}

bool readPhilipsNtuple::getTree(){
  cout << "Hello from readPhilipsNtuple::getTree()" << endl;\
  cout << " with: " << _fname << ", treename: " << _ntpName << endl;
  TFile* f = TFile::Open(_fname.c_str(), "READ");
  cout << "open file with f = " << f << endl;
  if(0 == f) return false;
  _oldTree = dynamic_cast<TTree*>(f->Get(_ntpName.c_str()));
  cout << " got _oldTree = " << _oldTree <<endl;
  _oldTree->SetBranchStatus("*", 1);
  cout << "set branch statues " << endl;
  attachit();
  cout << " attached it." << endl;
  
  cout << "readPhilipsNtuple::getTree(): Reading from " << _fname 
       << ", writing to: " << newFilename() << endl;
  _file0 = TFile::Open(newFilename().c_str(), "RECREATE");
  cout << "opened new file " << _file0 << endl;
  if(0 == _file0) return false;
  _file0->cd();
  cout << "cd'ed to new file " << endl;
  _tree = _oldTree->CloneTree(0);
  _tree->SetDirectory(_file0);
  for(int i=0; i < _numWeightBranches; i++){
    _BranchPointerArray[i] = setBranch(( weightBranchName(i)).c_str(), &(_modelWeightArray[i]));
  }
  cout << "cloned tree from old file" << endl;
  return (0 != _tree);
}

TBranch* readPhilipsNtuple::getBranch(int i){
  //  if( i < 0) i = weightNumber();
  return _BranchPointerArray[i];
}
std::string readPhilipsNtuple::weightBranchName(int i) const{
    std::string postFix="";
    //    if( i < 0) i = weightNumber();
    if(0 != i) postFix = anythingToString(i);
    return _weightBranchBaseName + postFix;
}
MINT::counted_ptr<DalitzEvent> 
readPhilipsNtuple::readEntry(unsigned int entry){
  if("GenK3PiTuple/GenK3PiTuple" ==  _ntpName){
    return readEntryReco(entry);
  }else{
    return readEntryMC(entry);
  }
}
MINT::counted_ptr<DalitzEvent> 
readPhilipsNtuple::readEntryMC(unsigned int entry){
  // we read the MC-truth 4-momenta as we want to re-weight
  // according to the trugh.
  // Things to cut on are read from reconstructed data.
  _oldTree->GetEntry(entry);
  for(int i=0; i < _numWeightBranches; i++){
    modelWeight(i) = -1;
  }

  if(0 == _prompt){
    counted_ptr<DalitzEvent> zeroPtr(0);
    return zeroPtr;
  }
  // we'll fill it according to the event pattern 
  // 421 -> -321 +211 -211 +211
  DalitzEventPattern pat(421, -321, +211, -211, +211);

  // if we encounter +321, we'll reverse all charges.
  TLorentzVector K(_K_mcp4arr);
  TLorentzVector Pi0(_Pi0_mcp4arr);
  TLorentzVector Pi1(_Pi1_mcp4arr);
  TLorentzVector Pi2(_Pi2_mcp4arr);
  
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
  _mcpD.SetXYZT(_mcpx, _mcpy, _mcpz, _mcpe);

  counted_ptr<DalitzEvent> evtPtr(new DalitzEvent(pat, PArray));
  return evtPtr;
}
MINT::counted_ptr<DalitzEvent> 
readPhilipsNtuple::readEntryReco(unsigned int entry){
  // we read the MC-truth 4-momenta as we want to re-weight
  // according to the trugh.
  // Things to cut on are read from reconstructed data.
  _oldTree->GetEntry(entry);
  for(int i=0; i < _numWeightBranches; i++){
    modelWeight(i) = -1;
  }

  if(0 == _prompt){
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
  _mcpD.SetXYZT(_px, _py, _pz, _pe);

  counted_ptr<DalitzEvent> evtPtr(new DalitzEvent(pat, PArray));
  return evtPtr;
}

bool readPhilipsNtuple::attachit(){
  _oldTree->SetBranchAddress("K_px",&_K_p4arr[0]);
  _oldTree->SetBranchAddress("K_py",&_K_p4arr[1]);
  _oldTree->SetBranchAddress("K_pz",&_K_p4arr[2]);
  _oldTree->SetBranchAddress("K_e",&_K_p4arr[3]);
  _oldTree->SetBranchAddress("K_pdg",&_K_pdg);

  _oldTree->SetBranchAddress("Pi0_px",&_Pi0_p4arr[0]);
  _oldTree->SetBranchAddress("Pi0_py",&_Pi0_p4arr[1]);
  _oldTree->SetBranchAddress("Pi0_pz",&_Pi0_p4arr[2]);
  _oldTree->SetBranchAddress("Pi0_e",&_Pi0_p4arr[3]);
  _oldTree->SetBranchAddress("Pi0_pdg",&_Pi0_pdg);

  _oldTree->SetBranchAddress("Pi1_px",&_Pi1_p4arr[0]);
  _oldTree->SetBranchAddress("Pi1_py",&_Pi1_p4arr[1]);
  _oldTree->SetBranchAddress("Pi1_pz",&_Pi1_p4arr[2]);
  _oldTree->SetBranchAddress("Pi1_e",&_Pi1_p4arr[3]);
  _oldTree->SetBranchAddress("Pi1_pdg",&_Pi1_pdg);

  _oldTree->SetBranchAddress("Pi2_px",&_Pi2_p4arr[0]);
  _oldTree->SetBranchAddress("Pi2_py",&_Pi2_p4arr[1]);
  _oldTree->SetBranchAddress("Pi2_pz",&_Pi2_p4arr[2]);
  _oldTree->SetBranchAddress("Pi2_e",&_Pi2_p4arr[3]);
  _oldTree->SetBranchAddress("Pi2_pdg",&_Pi2_pdg);

  // mc
  _oldTree->SetBranchAddress("K_mcpx",&_K_mcp4arr[0]);
  _oldTree->SetBranchAddress("K_mcpy",&_K_mcp4arr[1]);
  _oldTree->SetBranchAddress("K_mcpz",&_K_mcp4arr[2]);
  _oldTree->SetBranchAddress("K_mce",&_K_mcp4arr[3]);

  _oldTree->SetBranchAddress("Pi0_mcpx",&_Pi0_mcp4arr[0]);
  _oldTree->SetBranchAddress("Pi0_mcpy",&_Pi0_mcp4arr[1]);
  _oldTree->SetBranchAddress("Pi0_mcpz",&_Pi0_mcp4arr[2]);
  _oldTree->SetBranchAddress("Pi0_mce",&_Pi0_mcp4arr[3]);

  _oldTree->SetBranchAddress("Pi1_mcpx",&_Pi1_mcp4arr[0]);
  _oldTree->SetBranchAddress("Pi1_mcpy",&_Pi1_mcp4arr[1]);
  _oldTree->SetBranchAddress("Pi1_mcpz",&_Pi1_mcp4arr[2]);
  _oldTree->SetBranchAddress("Pi1_mce",&_Pi1_mcp4arr[3]);

  _oldTree->SetBranchAddress("Pi2_mcpx",&_Pi2_mcp4arr[0]);
  _oldTree->SetBranchAddress("Pi2_mcpy",&_Pi2_mcp4arr[1]);
  _oldTree->SetBranchAddress("Pi2_mcpz",&_Pi2_mcp4arr[2]);
  _oldTree->SetBranchAddress("Pi2_mce",&_Pi2_mcp4arr[3]);


  // D-related:
  _oldTree->SetBranchAddress("prompt",&_prompt);

  _oldTree->SetBranchAddress("ycm",&_ycm);
  _oldTree->SetBranchAddress("ylab",&_ylab);
  _oldTree->SetBranchAddress("mcycm",&_mcycm);
  _oldTree->SetBranchAddress("mcylab",&_mcylab);

  _oldTree->SetBranchAddress("plab_e",&_pe);
  _oldTree->SetBranchAddress("plab_x",&_px);
  _oldTree->SetBranchAddress("plab_y",&_py);
  _oldTree->SetBranchAddress("plab_z",&_pz);

  //mc
  _oldTree->SetBranchAddress("mcelab",&_mcpe);
  _oldTree->SetBranchAddress("mcpxlab",&_mcpx);
  _oldTree->SetBranchAddress("mcpylab",&_mcpy);
  _oldTree->SetBranchAddress("mcpzlab",&_mcpz);

  //_oldTree->SetBranchAddress("modelWeight",&_modelWeight);
  //  _modelWeightBranch = setBranch("modelWeight", &_modelWeight);
  
  return true;
}

void readPhilipsNtuple::setAllModelWeights(double w){
  for(int i=0; i < _numWeightBranches; i++) _modelWeightArray[i] = w;
  _tree->Fill();
}
void readPhilipsNtuple::setModelWeights(const std::vector<double>& weightVector){
  int mx=weightVector.size();
  if(mx > _numWeightBranches) mx = _numWeightBranches;
  for(int i=0; i < mx; i++) _modelWeightArray[i] = weightVector[i];
  _tree->Fill();
}

bool readPhilipsNtuple::passCuts(){
  // always returns true, for now - cuts in WeightedCount::doCount().
  //if(_ycm < 1.0 || _ycm > 5.5) return false;

  return true;
}



unsigned int readPhilipsNtuple::numEvents() const{
  int n= _oldTree->GetEntries();
  if(_maxEvents < 0 || n < _maxEvents) return n;
  else return _maxEvents;
}
bool readPhilipsNtuple::readit(DiskResidentEventList* listPtr, int maxEvents){
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

