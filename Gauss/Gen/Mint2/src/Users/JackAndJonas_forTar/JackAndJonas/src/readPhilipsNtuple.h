#ifndef READ_PHILIPS_NTUPLE_HH
#define READ_PHILIPS_NTUPLE_HH

#include "TFile.h"
#include "TTree.h"
#include "TLorentzVector.h"

#include <string>

#include "Mint/counted_ptr.h"
#include "Mint/DalitzEvent.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/DiskResidentEventList.h"
//#include "Mint/DiskResidentEventList.h"

class readPhilipsNtuple{
  TFile* _file0;
  TTree* _oldTree;
  TTree* _tree;
  std::string _fname;
  std::string _ntpName;
  long int _maxEvents;
  std::string _weightBranchBaseName;

  //  DiskResidentEventList* _listPtr;

  Float_t _K_p4arr[4];
  Float_t _K_mcp4arr[4];
  int _K_pdg;

  Float_t _Pi0_p4arr[4];
  Float_t _Pi0_mcp4arr[4];
  int _Pi0_pdg;

  Float_t _Pi1_p4arr[4];
  Float_t _Pi1_mcp4arr[4];
  int _Pi1_pdg;

  Float_t _Pi2_p4arr[4];
  Float_t _Pi2_mcp4arr[4];
  int _Pi2_pdg;

  int _prompt;
  Float_t _ycm, _ylab;
  Float_t _mcycm, _mcylab;

  Float_t _pe, _px, _py, _pz;
  Float_t _mcpe, _mcpx, _mcpy, _mcpz;

  Float_t* _modelWeightArray;
  int _numWeightBranches;
  //  int _weightNumber;

  TBranch** _BranchPointerArray;

  TLorentzVector _pD;
  TLorentzVector _mcpD;

  bool getTree();
  bool attachit();

  bool passCuts();

  TBranch* setBranch(const std::string& name, Float_t* ptr);

  std::string newFilename() const;
 public:
  readPhilipsNtuple(std::string fname
		    , std::string ntpName = "mc_k3pi"
		    , long int maxEvents = -1 // -1 means: read all
		    , const std::string& weightBranchBaseName = "modelWeight"
		    , int numWeightBranches = 0
		    );

  ~readPhilipsNtuple();

  unsigned int numEvents() const;
  MINT::counted_ptr<DalitzEvent> readEntryMC(unsigned int entry);
  MINT::counted_ptr<DalitzEvent> readEntryReco(unsigned int entry);
  MINT::counted_ptr<DalitzEvent> readEntry(unsigned int entry);

  const TLorentzVector& pD() const{return _pD;}
  const TLorentzVector& mcpD() const{return _mcpD;}
  double pt() const{return pD().Pt();}
  double mcpt() const{return mcpD().Pt();}
  Float_t ycm() const{return _ycm;}
  Float_t mcycm() const{return _mcycm;}
  Float_t ylab() const{return _ylab;}
  Float_t mcylab() const{return _mcylab;}
  double mcplab() const{return mcpD().P();}
  double plab() const{return pD().P();}

  bool readit(DiskResidentEventList* listPtr, int nEvents=10);
 
  void save();
  void setModelWeights(const std::vector<double>& wV);
  void setAllModelWeights(double w);


  //  void setWeightNumber(int i);
  //  int weightNumber() const{return _weightNumber;}

  const Float_t& modelWeight(int i) const;
  Float_t& modelWeight(int i);
 
  std::string weightBranchName(int i) const;

  TBranch* getBranch(int i = -9999);

};

#endif
//
