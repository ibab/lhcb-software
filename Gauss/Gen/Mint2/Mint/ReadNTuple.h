#ifndef READNTUPLE_HH
#define READNTUPLE_HH

#include "TFile.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include "TString.h"
#include "TTree.h"
#include "TBranch.h"
#include "TEntryList.h"
#include "TRandom.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>

#include "Mint/counted_ptr.h"
#include "Mint/DalitzEvent.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/MinimalEventList.h"
#include "Mint/DiskResidentEventList.h"
#include "Mint/DalitzEventList.h"
#include "Mint/DalitzEventPattern.h"
#include "Mint/ReadNTupleBase.h"

using namespace std;
using namespace MINT;

template <typename T, typename N>
class ReadNTuple: public ReadNTupleBase
{
private:
  TFile* _file0;
  TTree* _oldTree;
  TTree* _tree;
  std::string _fname;
  std::string _ntpName;
  long int _maxEvents;



  TRandom Rand;
  TTree* friendTree;
  // Variables to read in
  std::vector<std::vector<N> > m_input_var;
  std::vector<T> m_pdg; // pattern from events
  std::vector<int> set_pat; // pattern fed in
  std::vector<N> m_mother_var;
  T m_mother_pdg; // change for DTF

  bool m_useWeights;
  std::string m_weightName;

  std::vector<N> m_slowPion_var;
  int m_slowPion_pdg;
  bool slowPion;

  int m_particle;
  const char* _cuts;

  Long64_t _entries;
  Long64_t _firstentry;

  TLorentzVector _pMother;

  DalitzEventPattern* m_pat;

  bool _applyFiducalCuts;

public:

  std::string newFilename() const;

  ReadNTuple( DalitzEventPattern* pat
      ,	std::string fname
      , std::string ntpName  = "DalitzEventList"
          , const char* cuts = ""
              , long int maxEvents = -1
  )
  : _file0(0)
  , _oldTree(0)
  , _tree(0)
  , _fname(fname)
  , _ntpName(ntpName)
  , _maxEvents(maxEvents)
  , m_useWeights(false)
  , m_particle(0)
  , _firstentry(0)
  , _entries(1000000000)
  {
    m_weightName = "";

    for (int i = 0; i < 5; i++) {
        vector<N> row; // Create an empty row
        for (int j = 0; j < 5; j++) {
            row.push_back(i * j); // Add an element (column) to the row
        }
        m_input_var.push_back(row); // Add the row to the main vector
    }

    for (int i = 0; i < 4; i++) {
        m_pdg.push_back(0);
        m_mother_var.push_back(0);
        m_slowPion_var.push_back(0);

    }

    //	    SetEventPattern(pat);

    slowPion = false;
    _cuts = cuts;
    _applyFiducalCuts = false;

    SetEventPattern(pat);
    getTree();
  }

  ~ReadNTuple()
  {

  }

  bool getTree(){
    TFile* f = TFile::Open(_fname.c_str(), "READ");
    if(0 == f) return false;
    _oldTree = dynamic_cast<TTree*>(f->Get(_ntpName.c_str()));
    return (0 != _oldTree);
  }


  bool SetDaughterBranchAddress(const char* Px, const char* Py, const char* Pz, const char* E, const char* pdg );
  bool SetMotherBranchAddress(const char* Px, const char* Py, const char* Pz, const char* E, const char* pdg );
  void AddSlowPion(const char* Px = "SlowPion_PX", const char* Py= "SlowPion_PY",
      const char* Pz= "SlowPion_PZ", const char* E= "SlowPion_E",
      const char* pdg = "SlowPion_ID" );

  void SetEventPattern(DalitzEventPattern* pat)
  {
    m_pat = pat;
    set_pat = pat->getVectorOfInts();
  }

  void ApplyFiducalCuts();
  bool passFiducalCuts(unsigned int entry);
  bool passedFidCuts(float dx, float dy, float dz);

  bool AddFriend(std::string fname
      , std::string ntpName);

  bool getUpdatedTree();

  void useWeights(bool Weights);

  void weightVarName(std::string weightName);

  MINT::counted_ptr<DalitzEvent> readEntry(unsigned int entry);
  bool readit(MinimalEventList<DalitzEvent>* listPtr, int nEvents=-1, double scale = 1.0);
  //
  bool testEventPattern();
  bool passETAcut(unsigned int entry);
  float Random()
  {
    return Rand.Rndm();
  }

  void SetEntries(int entries){ _entries = entries;}
  void SetFirstEntry(int firstEntry){ _firstentry = firstEntry;}

};


template <typename T, typename N>
std::string ReadNTuple<T,N>::newFilename() const{
  bool exists = true;
  ofstream checkFile;
  int i=0;
  string newFilename(_fname);
  newFilename.insert(newFilename.find_last_of('.'), "_reweighted");
  do{
      newFilename.insert(newFilename.find_last_of('.'), anythingToString(i++));
      checkFile.open( newFilename.c_str(), ios::in );
      exists = ! checkFile.fail();
      checkFile.close();
  }while(exists);

  return newFilename;
}

template <typename T, typename N>
bool ReadNTuple<T,N>::AddFriend(std::string fname
    , std::string ntpName)
    {
  if (fname == "" || ntpName == "")
    {
      std::cout << "No friend Specified" << std::endl;
      return true;
    }
  TFile* f = TFile::Open(fname.c_str(), "READ");
  if(0 == f) return false;
  friendTree = dynamic_cast<TTree*>(f->Get(ntpName.c_str()));
  _oldTree->AddFriend(friendTree);
  return (0 != _oldTree);

    }
//template<>
//bool ReadNTuple<float>::getTree(){
//  TFile* f = TFile::Open(_fname.c_str(), "READ");
//  if(0 == f) return false;
//  _oldTree = dynamic_cast<TTree*>(f->Get(_ntpName.c_str()));
//  return (0 != _oldTree);
//
//}

template <typename T, typename N>
bool ReadNTuple<T,N>::getUpdatedTree()
{

  TEntryList* elist=0;

  _oldTree->SetBranchStatus("*",1);
  if (_cuts&&strcmp(_cuts,"")!=0) {
      _oldTree->Draw(">>elist",_cuts,"entryList");
      elist = (TEntryList*)gDirectory->Get("elist");
      if (!elist)
        {
          std::stringstream msg;
          msg << "Failed to retrieve TEntryList for cuts " << _cuts;

        }
      _oldTree->SetEntryList(elist);
  }
  Long64_t nentries = !elist?_oldTree->GetEntries():elist->GetN();
  if (nentries==0) {
      std::cout << "No entries after cuts " << _cuts << std::endl;
  }

  //  attachit();
  //  cout << "ReadNTuple::getTree(): Reading from " << _fname
  //       << ", writing to: " << newFilename() << endl;
  _file0 = TFile::Open(newFilename().c_str(), "RECREATE");
  cout << "opened new file " << _file0 << endl;
  if(0 == _file0) return false;
  _file0->cd();
  cout << "cd'ed to new file " << endl;
  //  _tree = _oldTree->CloneTree(1000);
  std::cout << "Cuts: " << _cuts << std::endl;

  _tree = _oldTree->CopyTree(_cuts, "",_entries,_firstentry);

  _tree->Write();


  return (0 != _tree);
}

//template <>
//void ReadNTuple<float>::SetEventPattern(DalitzEventPattern* pat)
//{
//	m_pat = pat;
//	set_pat = pat->getVectorOfInts();
//}

template <typename T, typename N>
void ReadNTuple<T,N>::ApplyFiducalCuts()
{
  _applyFiducalCuts = true;
}

template <typename T, typename N>
bool ReadNTuple<T,N>::passedFidCuts(float dx, float dy, float dz)
{
  bool passCuts = true;
  if (fabs(dx) > 0.317*(dz - 2400))
    {
      passCuts = false;
    }
  if (fabs(dy/dz) < 0.02){
      double p1 = 398;//418
      double p2 = 517;//497
      double beta1 = 0.01397;
      double beta2 = 0.01605;
      if( ( (p1 - beta1*dz) < fabs(dx) ) || ( fabs(dx) < ( p2+ beta2*dz ) ))
        //|| (fabs(dx) > 600 && fabs(dx) < 700)) //add this back in if needed
        {
          passCuts = false;
        }
  }
  //		if ( (fabs(dx) > 600 && fabs(dx) < 700)) passCuts = false;

  //	if (sqrt(dx*dx+dy*dy+dz*dz) > 100000) passCuts = false;

  return passCuts;
}
// Fiducal Cuts
template <typename T, typename N>
bool ReadNTuple<T,N>::passFiducalCuts(unsigned int entry)
{
  _tree->GetEntry(entry);
  bool passCuts = true;


  for (int i = 0; i<4; i++)
    {
      float dx = m_input_var[0][i];
      float dy = m_input_var[1][i];
      float dz = m_input_var[2][i];
      if (!passedFidCuts(dx,dy,dz)) passCuts = false;
    }

  if (slowPion)
    {
      if (!passedFidCuts(m_slowPion_var[0],m_slowPion_var[1],m_slowPion_var[2])) passCuts = false;
    }
  return passCuts;
}

template <typename T, typename N>
bool ReadNTuple<T,N>::SetDaughterBranchAddress(const char* Px, const char* Py, const char* Pz, const char* E, const char* pdg )
{

  _oldTree->SetBranchAddress(Px,&m_input_var[0][m_particle]);
  _oldTree->SetBranchAddress(Py,&m_input_var[1][m_particle]);
  _oldTree->SetBranchAddress(Pz,&m_input_var[2][m_particle]);
  _oldTree->SetBranchAddress(E,&m_input_var[3][m_particle]);
  _oldTree->SetBranchAddress(pdg,&m_pdg[m_particle]);
  m_particle++;

  return true;
}

template <typename T, typename N>
bool ReadNTuple<T,N>::SetMotherBranchAddress(const char* Px, const char* Py, const char* Pz, const char* E, const char* pdg )
{

  _oldTree->SetBranchAddress(Px,&m_mother_var[0]);
  _oldTree->SetBranchAddress(Py,&m_mother_var[1]);
  _oldTree->SetBranchAddress(Pz,&m_mother_var[2]);
  _oldTree->SetBranchAddress(E,&m_mother_var[3]);
  _oldTree->SetBranchAddress(pdg,&m_mother_pdg);

  return true;
}

template <typename T, typename N>
MINT::counted_ptr<DalitzEvent> ReadNTuple<T,N>::readEntry(unsigned int entry){

  // we read the MC-truth 4-momenta as we want to re-weight
  // according to the trugh.
  // Things to cut on are read from reconstructed data.

  bool dbThis = false;

  _tree->GetEntry(entry);

  if (set_pat.size() == 0)
    {
      std::cout << "Could not find EventPattern Did you set it?" << std::endl;
      counted_ptr<DalitzEvent> zeroPtr(0);
      return zeroPtr;
    }
  //   if (set_pat.size() != m_pdg.size())
  //	{
  //	   std::cout << "Event Patterns not the same" << std::endl;
  //	   counted_ptr<DalitzEvent> zeroPtr(0);
  //	   return zeroPtr;
  //	}
  //   for (int i = 0; i < 5; i++)
  //	   {
  //		   if (m_pdg[i] != set_pat[i])
  //		   {
  //			   std::cout << "ReadNTuple: Warning input Pattern does not match selected Events: Setting pattern to match Events " << m_pdg << std::endl;
  //			   set_pat = m_pdg;
  //			   break;
  //		   }
  //
  //	   }

  //  OrderParticles();
  // we'll fill it according to the event pattern
  // 421 -> -321 +211 -211 +211
  //  DalitzEventPattern pat(421, -211, +211, +211, -211);

  TLorentzVector mother(m_mother_var[0],m_mother_var[1],m_mother_var[2],m_mother_var[3]);

  // Get daughter LorentzVectors
  TLorentzVector daughter1(m_input_var[0][0],m_input_var[1][0],m_input_var[2][0],m_input_var[3][0]);
  TLorentzVector daughter2(m_input_var[0][1],m_input_var[1][1],m_input_var[2][1],m_input_var[3][1]);
  TLorentzVector daughter3(m_input_var[0][2],m_input_var[1][2],m_input_var[2][2],m_input_var[3][2]);
  TLorentzVector daughter4(m_input_var[0][3],m_input_var[1][3],m_input_var[2][3],m_input_var[3][3]);

  std::vector<TLorentzVector> daughter;
  daughter.push_back(daughter1);
  daughter.push_back(daughter2);
  daughter.push_back(daughter3);
  daughter.push_back(daughter4);

  if (dbThis)
    {
      if (!(entry%10000))
        {
          cout << "PDG: " << (int)m_pdg[0] << " Px: " << m_input_var[0][0] << std::endl;
          cout << "PDG: " << (int)m_pdg[1] << " Px: " << m_input_var[0][1] << std::endl;
          cout << "PDG: " << (int)m_pdg[2] << " Px: " << m_input_var[0][2] << std::endl;
          cout << "PDG: " << (int)m_pdg[3] << " Px: " << m_input_var[0][3] << std::endl;
          cout << "Mother PDG: " << m_mother_pdg << " Px: " << m_mother_var[0] << std::endl;

        }
    }
  // vector of Lorentz Vectors to great Dalitz Event
  vector<TLorentzVector> PArray(5);

  TLorentzVector* pMother   = &(PArray[0]);
  *pMother = mother;

  int pdgArray[5];
  pdgArray[0] = (int)m_mother_pdg;
  //  std::cout << m_mother_pdg <<

  if (dbThis)
    {
      if (!(entry%10000))
        {
          std::cout << "PArray Px: " << PArray[0].Px() << std::endl;
          std::cout << "Mother Px: " << mother.Px() << std::endl;
        }
    }
  std::vector<int> passed_pat = set_pat;
  std::vector<T> this_pat = m_pdg; //change this for DTF
  if (dbThis)
    {
      if (!(entry%1000))
        {
          for (int i = 0; i < 4; i++) // loop over particles read in
            {
              cout << "this_pat: " << (int)this_pat[i] << " passed pat:" << passed_pat[i];
              cout << std::endl;
            }
        }
    }

  // Order daughter particles accordinging to passed_pat
  for (int j =1; j<5; j++) // loop over particles in pattern given
    {
      for (int i = 0; i < 4; i++) // loop over particles read in
        {
          if (dbThis)
            {
              if (!(entry%1000))
                {
                  cout << "Pdg: " << passed_pat[j] << " " << (int)this_pat[i];
                  cout << std::endl;
                }
            }
          if (passed_pat[j] == (int)this_pat[i]) // if particle equal to the one in pattern
            {
              pdgArray[j] = (int)this_pat[i];
              PArray[j] = daughter[i];
              this_pat[i] = 999; // dont use this particle again
              if (dbThis)
                {
                  if (!(entry%1000))
                    {
                      cout << "Pdg: " << passed_pat[j];
                      cout << " Px: " << PArray[j].Px() << std::endl;
                    }
                }
              break;
            }
        }
    }

  //   Randomize indentical final state particles
  for (int i = 1; i < 4; i++)
    {
      for (int j = i+1; j < 5; j++)
        {
          if( pdgArray[i] == pdgArray[j])
            {
              //				if(gRandom->Rndm() >0.5)
              if (Random() > 0.5)
                {
                  swap(PArray[i],PArray[j] );//swap 4-momenta
                  swap(pdgArray[i],pdgArray[j] );//shouldn't need this
                }
            }
        }
    }


  if (dbThis)
    {
      if(entry < 5){
          for(unsigned int i = 0; i < 5; i++){
              cout << " mass " << i << ") " << PArray[i].M()
												                   << " Px " << i << ") " << PArray[i].Px()
												                   << " Py " << i << ") " << PArray[i].Py()
												                   << " Pz " << i << ") " << PArray[i].Pz()
												                   << " E " << i << ") " << PArray[i].E()
												                   << ", pdg " << pdgArray[i] << endl;
          }
      }

      if (!(entry%10000))
        {
          for(unsigned int i = 0; i < 5; i++){
              cout << " mass " << i << ") " << PArray[i].M()
														                 << " Px " << i << ") " << PArray[i].Px()
														                 << ", pdg " << pdgArray[i] << endl;
          }
        }
    }
  _pMother.SetPxPyPzE(m_mother_var[0], m_mother_var[1], m_mother_var[2], m_mother_var[3]);

  //  DalitzEvent* devt = new DalitzEvent(*m_pat, PArray);
  MINT::counted_ptr<DalitzEvent> evtPtr(new DalitzEvent(*m_pat, PArray));
  return evtPtr;
}

template <typename T, typename N>
void ReadNTuple<T,N>::useWeights(bool Weights)
{
  m_useWeights = Weights;
}

template <typename T, typename N>
void ReadNTuple<T,N>::weightVarName(std::string weightName)
{
  m_weightName = weightName;
}

template <typename T, typename N>
bool ReadNTuple<T,N>::readit(DiskResidentEventList* listPtr, int maxEvents, double scale){
  getUpdatedTree();
  if(0 == listPtr) return false;
  int numEvents = 0;
  TRandom Rand(1987);

  if (!testEventPattern()) return false;

  double weight(0);
  if (m_useWeights)
    {
      _tree->SetBranchAddress(m_weightName.c_str(),&weight);
    }
  std::cout << "Entries " << _oldTree->GetEntries() << " " << _tree->GetEntries() << std::endl;
  for(unsigned int i=0; i < _tree->GetEntries(); i++){

      if (Rand.Rndm() < scale)
        {
          if (_applyFiducalCuts)
            {
              if (passFiducalCuts(i))
                {
                  if (passETAcut(i))
                    {
                      counted_ptr<DalitzEvent> evtPtr = readEntry(i);
                      if (m_useWeights)
                        {
                          evtPtr->setWeight(weight);
                        }

                      numEvents++;
                      listPtr->Add(*(evtPtr.get()));
                    }
                }
            }
          if (!_applyFiducalCuts)
            {
              counted_ptr<DalitzEvent> evtPtr = readEntry(i);
              if (m_useWeights)
                {
                  evtPtr->setWeight(weight);
                }

              if (evtPtr->kinematicallyAllowed(5.e-2))
                {
                  numEvents++;
                  listPtr->Add(*(evtPtr.get()));
                }
              else if (!evtPtr->kinematicallyAllowed(5.e-2))
                {
                  std::cout << "Not allowed " << std::endl;
                  evtPtr->print();
                }
            }
          if(maxEvents > 0 && numEvents > maxEvents) break;
        }
  }
  return true;
}

template <typename T, typename N>
bool ReadNTuple<T,N>::testEventPattern()
{
  _tree->GetEntry(0);
  if ((int)set_pat[0] != (int)m_mother_pdg)
    {
      cout << "Mother Pattern not the same try CC" << endl;
      cout << " set_pat " << set_pat[0] << " " << set_pat[1];
      cout << " " << set_pat[2] << " " << set_pat[3] << std::endl;

      cout << " m_mother_pdg " << m_mother_pdg << " " << endl;

      DalitzEventPattern pat = (m_pat->makeCPConjugate());
      m_pat = new DalitzEventPattern(pat);
      set_pat = m_pat->getVectorOfInts();
    }
  if ((int)set_pat[0] != (int)m_mother_pdg)
    {
      cerr << " set_pat " << set_pat[0] << std::endl;
      cerr << "Mother Pattern STILL not the same" << endl;
      cerr << "Will try other entry" << endl;
      _tree->GetEntry(10);
      if (set_pat[0] != m_mother_pdg)
        {
          cout << "Mother Pattern not the same try CC" << endl;
          cout << " set_pat " << set_pat[0] << " " << set_pat[1];
          cout << " " << set_pat[2] << " " << set_pat[3] << std::endl;

          cout << " m_mother_pdg " << m_mother_pdg << " " << endl;

          DalitzEventPattern pat = (m_pat->makeCPConjugate());
          m_pat = new DalitzEventPattern(pat);
          set_pat = m_pat->getVectorOfInts();
        }
      if (set_pat[0] != m_mother_pdg)
        {
          cerr << " set_pat " << set_pat[0] << std::endl;
          cerr << "Mother Pattern STILL not the same" << endl;
          return false;
        }
    }

  return true;
}


template <typename T, typename N>
void ReadNTuple<T,N>::AddSlowPion(const char* Px, const char* Py, const char* Pz, const char* E, const char* pdg )
{

  _oldTree->SetBranchAddress(Px,(float*)&m_slowPion_var[0]);
  _oldTree->SetBranchAddress(Py,(float*)&m_slowPion_var[1]);
  _oldTree->SetBranchAddress(Pz,(float*)&m_slowPion_var[2]);
  _oldTree->SetBranchAddress(E,(float*)&m_slowPion_var[3]);
  _oldTree->SetBranchAddress(pdg,(int*)&m_slowPion_pdg);
  slowPion = true;

}

template <typename T, typename N>
bool ReadNTuple<T,N>::passETAcut(unsigned int entry)
{
  bool passETAcut = false;
  _tree->GetEntry(entry);

  float dx = m_mother_var[0];
  float dy = m_mother_var[1];
  float dz = m_mother_var[2];
  float P = sqrt(dx*dx+dy*dy+dz*dz);
  float ETA = 0.5*log( (P + dz)/(P - dz) );
  if (ETA < 5.0 && ETA > 2.0 ) passETAcut = true;
  if (P < 300000 ) passETAcut = true;
  return passETAcut;
}

#endif
