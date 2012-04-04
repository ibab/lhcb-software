#ifndef READNTUPLE_HH
#define READNTUPLE_HH

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

class ReadNTuple
{
private:
	  TFile* _file0;
	  TTree* _oldTree;
	  TTree* _tree;
	  std::string _fname;
	  std::string _ntpName;
	  long int _maxEvents;

	  TTree* friendTree;
	  // Variables to read in
	  std::vector<std::vector<float> > m_input_var;
	  std::vector<int> m_pdg;
	  std::vector<int> set_pat;
	  std::vector<float> m_mother_var;
	  int m_mother_pdg;

	  std::vector<float> m_slowPion_var;
	  int m_slowPion_pdg;
	  bool slowPion;

	  int m_particle;
	  const char* _cuts;

	  TLorentzVector _pMother;

	  DalitzEventPattern* m_pat;

	  bool _applyFiducalCuts;

public:

	  std::string newFilename() const;

	  ReadNTuple( DalitzEventPattern*
			    , std::string fname
	  		    , std::string ntpName = "DalitzEventList"
	  		    , const char* = ""
	  		    , long int maxEvents = -1 // -1 means: read all
	  		    );

	  ~ReadNTuple();

	  bool getTree();

	  bool SetDaughterBranchAddress(const char* Px, const char* Py, const char* Pz, const char* E, const char* pdg );
	  bool SetMotherBranchAddress(const char* Px, const char* Py, const char* Pz, const char* E, const char* pdg );
	  void AddSlowPion(const char* Px = "SlowPion_PX", const char* Py= "SlowPion_PY",
			  	  	  const char* Pz= "SlowPion_PZ", const char* E= "SlowPion_E",
			  	  	  const char* pdg = "SlowPion_ID" );

	  bool SetEventPattern(DalitzEventPattern*);

	  void ApplyFiducalCuts();
	  bool passFiducalCuts(unsigned int entry);
	  bool passedFidCuts(float dx, float dy, float dz);

	  bool AddFriend(std::string fname
	  	      	  , std::string ntpName);

	  bool getUpdatedTree();

	  MINT::counted_ptr<DalitzEvent> readEntry(unsigned int entry);
	  bool readit(DiskResidentEventList* listPtr, int nEvents=-1, double scale = 1.0);

	  bool testEventPattern();

};


#endif
