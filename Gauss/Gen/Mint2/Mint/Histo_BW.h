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
  
  Histo_BW( const AssociatedDecayTree& tree): BW_BW(tree), _runningWidthHist(0)
  {
      TFile* f = TFile::Open(("RunningWidth_"+ (BW_BW::resonanceProperties()->nameFromPid(abs(mumsPID()))) + ".root").c_str());
      _runningWidthHist = get_histo(f, "RunningWidth");
  }

  virtual std::string name() const{
    return "Histo_BW("+_theDecay.oneLiner() +")";
  }

  virtual ~Histo_BW(){}

 protected:
    TH1D* _runningWidthHist;
    virtual double GofM(); 
    TH1D* get_histo(TFile* f, const std::string& hname); 
    
    //Double_t phaseSpace(Double_t *x, Double_t *par);
    TH1D* producePhaseSpaceHist();
};

#endif
//
