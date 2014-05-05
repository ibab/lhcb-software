
#include "readPhilipsNtuple.h"
#include "TLorentzVector.h"
#include "TRandom.h"

#include <iostream>
#include <algorithm>
#include <vector>

#include "DalitzEventPattern.h"
#include "DalitzEvent.h"

#include "CLHEPSystemOfUnits.h"

using namespace std;
using namespace MINT;

readPhilipsNtuple::readPhilipsNtuple(DiskResidentEventList& list
				     , std::string fname
				     , std::string ntpName)
  : _file0(0)
  , _tree(0)
  , _fname(fname)
  , _ntpName(ntpName)
  , _listPtr(&list)
{
  _file0 = TFile::Open(_fname.c_str());
  getTree();
  attachit();
  readit();
}

bool readPhilipsNtuple::getTree(){

  _tree=dynamic_cast<TTree*>(_file0->Get(_ntpName.c_str()));
  return !(0 == _tree);

}

MINT::counted_ptr<DalitzEvent> 
readPhilipsNtuple::readEntry(unsigned int entry){
  
  _tree->GetEntry(entry);

  if(1 != _prompt){
    counted_ptr<DalitzEvent> zeroPtr(0);
    return zeroPtr;
  }
  // we'll fill it according to the event pattern 
  // 421 -> -321 +211 -211 +211
  DalitzEventPattern pat(421, -321, +211, -211, +211);

  // if we encounter +321, we'll reverse all charges.
  TLorentzVector K0(_K0_p4arr);
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
  
  if(_K0_pdg < 0) pdgArray[0] = 421;
  else pdgArray[0] = - 421;

  int *Km_pdg   = &pdgArray[1];
  int *pip1_pdg = &pdgArray[2];
  int *pim_pdg  = &pdgArray[3];
  int *pip2_pdg = &pdgArray[4];

  *Km = K0;
  *Km_pdg = _K0_pdg;

  if(_K0_pdg * _Pi0_pdg > 0){
    *pim  = Pi0;   *pim_pdg  = _Pi0_pdg;
    *pip1 = Pi1;   *pip1_pdg = _Pi1_pdg;
    *pip2 = Pi2;   *pip2_pdg = _Pi2_pdg;
  }else{
    *pip1 =Pi0;    *pip1_pdg = _Pi0_pdg;
    if(_K0_pdg * _Pi1_pdg > 0){
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
  
  counted_ptr<DalitzEvent> evtPtr(new DalitzEvent(pat, PArray));
  return evtPtr;
}

bool readPhilipsNtuple::attachit(){
  _tree->SetBranchAddress("K0_px",&_K0_p4arr[0]);
  _tree->SetBranchAddress("K0_py",&_K0_p4arr[1]);
  _tree->SetBranchAddress("K0_pz",&_K0_p4arr[2]);
  _tree->SetBranchAddress("K0_pe",&_K0_p4arr[3]);
  _tree->SetBranchAddress("K0_pdg",&_K0_pdg);

  _tree->SetBranchAddress("Pi0_px",&_Pi0_p4arr[0]);
  _tree->SetBranchAddress("Pi0_py",&_Pi0_p4arr[1]);
  _tree->SetBranchAddress("Pi0_pz",&_Pi0_p4arr[2]);
  _tree->SetBranchAddress("Pi0_pe",&_Pi0_p4arr[3]);
  _tree->SetBranchAddress("Pi0_pdg",&_Pi0_pdg);

  _tree->SetBranchAddress("Pi1_px",&_Pi1_p4arr[0]);
  _tree->SetBranchAddress("Pi1_py",&_Pi1_p4arr[1]);
  _tree->SetBranchAddress("Pi1_pz",&_Pi1_p4arr[2]);
  _tree->SetBranchAddress("Pi1_pe",&_Pi1_p4arr[3]);
  _tree->SetBranchAddress("Pi1_pdg",&_Pi1_pdg);

  _tree->SetBranchAddress("Pi2_px",&_Pi2_p4arr[0]);
  _tree->SetBranchAddress("Pi2_py",&_Pi2_p4arr[1]);
  _tree->SetBranchAddress("Pi2_pz",&_Pi2_p4arr[2]);
  _tree->SetBranchAddress("Pi2_pe",&_Pi2_p4arr[3]);
  _tree->SetBranchAddress("Pi2_pdg",&_Pi2_pdg);

  _tree->SetBranchAddress("prompt",&_prompt);
  return true;
}

bool readPhilipsNtuple::readit(){
  int maxEvents = 1000000;
  int numEvents = 0;
  for(unsigned int i=0; i < _tree->GetEntries(); i++){
    counted_ptr<DalitzEvent> evtPtr = readEntry(i);
    if(! evtPtr) continue;
    numEvents++;
    _listPtr->Add(*(evtPtr.get()));
    if(maxEvents > 0 && numEvents > maxEvents) break;
  }
  //_listPtr->Start();
  //_listPtr->Next();
  return true;
}
