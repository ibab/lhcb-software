#ifndef READ_PHILIPS_GENERATORLEVEL_NTUPLE_HH
#define READ_PHILIPS_GENERATORLEVEL_NTUPLE_HH

#include "TFile.h"
#include "TTree.h"
#include "TLorentzVector.h"

#include <string>

#include "Mint/counted_ptr.h"
#include "Mint/DalitzEvent.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/DiskResidentEventList.h"
#include "Mint/DalitzEventList.h"


class readGenNtp{
  TFile* _file0;
  TTree* _oldTree;
  TTree* _tree;
  std::string _fname;
  std::string _ntpName;
  long int _maxEvents;
  std::string _weightBranchName;

  //  DiskResidentEventList* _listPtr;

  float _K_p4arr[4];
  int _K_pdg;

  float _Pi0_p4arr[4];
  int _Pi0_pdg;

  float _Pi1_p4arr[4];
  int _Pi1_pdg;

  float _Pi2_p4arr[4];
  int _Pi2_pdg;

  int _prompt;
  float _ycm, _ylab;

  float _pe, _px, _py, _pz;

  float _modelWeight;

  TLorentzVector _pD;

  bool getTree();
  bool attachit();

  bool passCuts();

  TBranch* setBranch(const std::string& name, float* ptr);

  std::string newFilename() const;
 public:
  readGenNtp(std::string fname
	     , std::string ntpName = "GenK3PiTuple/GenK3PiTuple"
	     , long int maxEvents = -1 // -1 means: read all
	     , const std::string& weightBranchName = "modelWeight"
	     );

  ~readGenNtp();

  unsigned int numEvents() const;
  MINT::counted_ptr<DalitzEvent> readEntry(unsigned int entry);

  const TLorentzVector& pD() const{return _pD;}
  double pt() const{return pD().Pt();}
  double ycm() const{return _ycm;}
  double ylab() const{return _ylab;}

  bool readit(DiskResidentEventList* listPtr, int nEvents=-1);
  bool readit(DalitzEventList* listPtr, int nEvents=-1);
 
  void save();
  void setModelWeight(double w);

};

#endif
//
