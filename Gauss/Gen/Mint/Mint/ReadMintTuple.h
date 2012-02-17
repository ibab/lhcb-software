#ifndef READMINTTUPLE_HH
#define READMINTTUPLE_HH

#include "TFile.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include "TString.h"
#include "TTree.h"
#include "TBranch.h"
#include "TEntryList.h"

#include <vector>

#include <string>

#include "Mint/counted_ptr.h"
#include "Mint/DalitzEvent.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/DiskResidentEventList.h"
#include "Mint/DalitzEventList.h"
#include "Mint/DalitzEventPattern.h"

using namespace std;
using namespace MINT;

class ReadMintTuple
{
private:
	  TFile* _file0;
	  TTree* _oldTree;
	  TTree* _tree;
	  std::string _fname;
	  std::string _ntpName;
	  long int _maxEvents;

	  // Variables to read in
	  std::vector<std::vector<double> > m_input_var;
	  std::vector<double> m_pdg;
	  std::vector<int> set_pat;
	  std::vector<double> m_mother_var;
	  double m_mother_pdg;

	  int m_particle;
	  const char* _cuts;

	  TLorentzVector _pMother;

	  DalitzEventPattern* m_pat;

	  bool _applyFiducalCuts;

public:

	  std::string newFilename() const;

	  ReadMintTuple( DalitzEventPattern*
			    , std::string fname
	  		    , std::string ntpName = "DalitzEventList"
	  		    , const char* = ""
	  		    , long int maxEvents = -1 // -1 means: read all
	  		    );

	  ~ReadMintTuple();

	  bool getTree();

	  bool SetDaughterBranchAddress(const char* Px, const char* Py, const char* Pz, const char* E, const char* pdg );
	  bool SetMotherBranchAddress(const char* Px, const char* Py, const char* Pz, const char* E, const char* pdg );

	  bool SetEventPattern(DalitzEventPattern*);

	  void ApplyFiducalCuts();
	  bool passFiducalCuts();

	  bool AddFriend(std::string fname
	  	      	  , std::string ntpName);

	  bool getUpdatedTree();

	  MINT::counted_ptr<DalitzEvent> readEntry(unsigned int entry);
	  bool readit(DiskResidentEventList* listPtr, int nEvents=10);


};

#endif
