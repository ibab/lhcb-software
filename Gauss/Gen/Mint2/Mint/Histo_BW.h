#ifndef HISTO_BW_LINESHAPE_HH
#define HISTO_BW_LINESHAPE_HH
// author: Philippe d'Argent (p.dargent@cern.ch)
// status: 19 March 2015 GMT
#include "TH1D.h"
#include "TFile.h"
#include <complex>

#include "Mint/ILineshape.h"
#include "Mint/BW_BW.h"
#include "Mint/NamedParameter.h"
#include "Mint/NamedParameterBase.h"
#include "Mint/ResonancePropertiesList.h"
#include "Mint/FitParDependent.h"
#include "ResonancePropertiesFitRef.h"
#include "ResonanceProperties.h"

using namespace MINT;
using namespace std;

class Histo_BW  : public BW_BW, virtual public ILineshape{
 public:
  
  Histo_BW( const AssociatedDecayTree& tree): BW_BW(tree), _phaseSpaceHist(0)
  {
      TFile* f = new TFile(("RunningWidth_"+ (BW_BW::resonanceProperties()->nameFromPid(abs(mumsPID()))) + ".root").c_str());
      _phaseSpaceHist = get_h(f, "phaseSpace");
  }

  virtual std::string name() const{
    return "Histo_BW("+_theDecay.oneLiner() +")";
  }

  virtual ~Histo_BW(){}

 protected:
    TH1D* _phaseSpaceHist;
    virtual double GofM(); 
    TH1D* get_h(TFile* f, const std::string& hname){
        TH1D* h=0;
        if(0 == f) return 0;
        cout << " got file ptr: " << f << endl;
        if(0 != f){
            h = (TH1D*) f->Get(hname.c_str());
            if(0 != h){
                cout << "got phase space histogram " 
                << hname << "." << endl;
            }else{
                cout << "ERROR didn't get phase space histogram " 
                << hname << "!!!" << endl;
            }
        }
        return h;
    }
 
};

#endif
//
