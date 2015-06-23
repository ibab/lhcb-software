#ifndef VIRTUAL_BW_HH
#define VIRTUAL_BW_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT

#include "Mint/NamedParameter.h"
#include "Mint/ILineshape.h"
#include "Mint/BW_BW.h"
#include "Mint/AssociatedDecayTree.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/CLHEPSystemOfUnits.h"

class VBW : public BW_BW, virtual public ILineshape{

 protected:
    double effectiveMass(int i, double mumsMass) const;
    virtual double twoBody_recodgtPsq_in_MumsPDGFrame() const;
    virtual double twoBody_dgtPsq_in_MumsRecoFrame() ;
    virtual double twoBody_dgtPsq_in_MumsPDGFrame() const;
    MINT::NamedParameter<int> _scaleQ;
    
 public:
  
  VBW( const AssociatedDecayTree& decay);
    
  //virtual std::complex<double> getVal(IDalitzEvent& evt);

  virtual std::string name() const{
    return "VBW("+_theDecay.oneLiner() +")";
  }

  virtual ~VBW(){}
};

#endif
//
