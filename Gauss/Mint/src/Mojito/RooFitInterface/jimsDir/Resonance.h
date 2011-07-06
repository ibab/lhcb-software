// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:09 GMT
#ifndef DATLITZRESONANCE
#define DATLITZRESONANCE
#include "TObject.h"
#include "Rtypes.h"

#include <complex>

#include "RooRealProxy.h"
#include "RooRealVar.h"
#include "AbsComplex.h"
#include "ComplexProxy.h"
#include "ComplexBW.h"

#define DONT_USE_COMPLEXPROXY

class Resonance : public AbsComplex{
  //  static const RooRealVar dummyM;
 protected:
  bool mset;
  RooRealProxy M;// matrix element
#ifdef DONT_USE_COMPLEXPROXY
  RooRealProxy bw; // breit wigner
#else
  ComplexProxy bw; // breit wigner
#endif
 public:
  Resonance(const char* name
	    , const char* title
	    , RooAbsReal& _M
	    , ComplexBW& _bw)
    : AbsComplex(name, title)
    , bw("bw", "bw", this, _bw)
    , M("M", "M", this, _M)
    , mset(true){
    std::cout << "made one" << std::endl;
  }
  Resonance(const char* name
	    , const char* title
	    , ComplexBW& _bw)
    : AbsComplex(name, title)
    , bw("bw", "bw", this, (RooAbsReal &) _bw)
    , M("M", "M", this,  *(new RooRealVar("dumm", "dumm", 1)))
    , mset(false){
    std::cout << "made another one " << GetName() << std::endl;
  }
  /*
  Resonance(const Resonance& other
	    , const char* name=0){
    cout << "just copied one" << endl;
  }
  */
  
  Resonance(const Resonance& other
	    , const char* name=0) 
    : AbsComplex(other, name)
    , bw("bw",this, other.bw)
    , M("M",this, other.M)
    , mset(other.mset)
    {      
      std::cout << "just copied one: "  
		<< other.GetName() << " --> "
		<< GetName() << std::endl;
    }

  virtual TObject* clone(const char* newname) const{
    return new Resonance(*this, newname);}
  virtual TObject* clone() const{
    return new Resonance(*this);}
  inline virtual ~Resonance(){}

  inline std::complex<Double_t> complexM() const{
    if(!mset) return std::complex<Double_t>(1, 0);
    const AbsComplexBrackets* MPtr = 
      dynamic_cast<const AbsComplexBrackets*>(&M.arg());
    if(! MPtr) std::cout << " Resonancd::complexM() cast failed " 
			 << std::endl;
    return (*MPtr)();
  }
  inline std::complex<Double_t> complexBW() const{
#ifdef DONT_USE_COMPLEXPROXY
    const AbsComplexBrackets* bwPtr = 
      dynamic_cast<const AbsComplexBrackets*>(&bw.arg());
    if(! bwPtr) std::cout << " Resonancd::complexBW() cast failed " 
			  << std::endl;
    return bwPtr->getCVal();
#else
    bw.getCVal();
#endif
  }

  virtual std::complex<Double_t> getCVal() const{
    return complexBW()*complexM();
  }

  Double_t evaluate() const{
    return abs((*this)());
  }

  ClassDef(Resonance, 0)
};

#endif
//
