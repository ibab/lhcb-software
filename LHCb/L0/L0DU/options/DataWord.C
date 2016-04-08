#include "TTree.h"
#include "TFile.h"
#include "TBranch.h"
#include <iostream>
#include <fstream>
#include <sstream>

void DataWord(){
  TFile* file=TFile::Open("L0Pattern.root");
  TTree* tree=(TTree*) file->Get("L0Pattern/500");
  tree->LoadTree(0);

  static const int NTCK=9;  // number of config to test
  static const int NFIB=24; // number of input fibre

  // Tree structure :
  Int_t   nConfig;
  Float_t rsda[NTCK];
  Float_t tck[NTCK];
  Int_t   nData;
  Float_t lsb[NFIB];
  Float_t msb[NFIB];
  Float_t fiber[NFIB];
  
  TBranch        *b_nConfig;   //!
  TBranch        *b_rsda;   
  TBranch        *b_tck;   
  TBranch        *b_nData;   //!
  TBranch        *b_lsb;   
  TBranch        *b_msb;   
  TBranch        *b_fiber;   
  
  tree->SetBranchAddress("nConfig", &nConfig, &b_nConfig);
  tree->SetBranchAddress("rsda",rsda,&b_rsda);
  tree->SetBranchAddress("tck",tck,&b_tck);
  tree->SetBranchAddress("nData", &nData, &b_nData);
  tree->SetBranchAddress("lsb",lsb,&b_lsb);
  tree->SetBranchAddress("msb",msb,&b_msb);
  tree->SetBranchAddress("fiber", fiber, &b_fiber);

  // == output files
  // -- input(fiber)
  ofstream oFIB[NFIB];
  for(int ifib=0;ifib< NFIB;ifib++){
    std::ostringstream fname(""); 
    fname << "fiber"<< (ifib+1) << "_256_hex.mif";
    oFIB[ifib].open  (fname.str().c_str());
    oFIB[ifib] << "CONTENT BEGIN" << endl;
  }
  // -- rsda(tck)
  ofstream oTCK[NTCK];
  for(int itck=0;itck< NTCK;itck++){
    std::ostringstream fname(""); 
    fname << "rsda"<< (itck+1) << "_256_hex.mif";
    oTCK[itck].open  (fname.str().c_str());
    oTCK[itck] << "CONTENT BEGIN" << endl;
  }

  // == scan tree and write pattern files
  int nentries=tree->GetEntries();
  int k=0;
  for ( int ient = 0; ient < nentries ; ient++ ) {
    if(k==256)continue;
    tree -> LoadTree(ient);
    tree -> GetEntry(ient);    

    for(int itck=0;itck< NTCK;itck++){
      long _tck  = long(tck[itck]);
      long _rsda = long(rsda[itck]);
      oTCK[itck] << "        " << std::setfill('0') << std::setw(3) << std::hex << k  << "  :    " << std::setw(8) << _rsda <<";"<< endl;
    }    

    for(int ifib=0;ifib<NFIB;ifib++){ 
      long word = long(lsb[ifib]) | (long(msb[ifib]) << 16);
      oFIB[ifib] << "        " << std::setfill('0') << std::setw(3) << std::hex << k  << "  :    " << std::setw(8) << word <<";"<< endl;
    }
    k++;
  }
  for(int ifib=0;ifib< NFIB;ifib++){oFIB[ifib] << "END;" << endl;}
  for(int itck=0;itck< NTCK;itck++){oTCK[itck] << "END;" << endl;}
  std::cout <<"L0DU PATTERN CREATED FOR " << k << " EVENTS "<<endl;
}
