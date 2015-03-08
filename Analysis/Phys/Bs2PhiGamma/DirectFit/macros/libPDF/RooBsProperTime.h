#ifndef ROOBSPROPERTIME
#define ROOBSPROPERTIME

#include "RooAbsPdf.h"
#include "RooRealProxy.h"

class RooRealVar;
class RooAbsReal;

class RooBsProperTime : public RooAbsPdf {
public:
  RooBsProperTime() {} ;
  RooBsProperTime(const char *name, const char *title,
                  RooAbsReal& _x, RooAbsReal& _tau, 
                  RooAbsReal& _dG,RooAbsReal& _aD,
                  RooAbsReal& _a, RooAbsReal& _n, RooAbsReal& _b, RooAbsReal& _t0,
                  RooAbsReal& _mu,int _type=0);


  RooBsProperTime(const char *name, const char *title,
                  RooAbsReal& _x, RooAbsReal& _tau, 
                  RooAbsReal& _dG,RooAbsReal& _aD,
                  RooAbsReal& _a, RooAbsReal& _n, RooAbsReal& _b, RooAbsReal& _t0,int _type=0);

  RooBsProperTime(const RooBsProperTime& other, const char* name=0);
  virtual TObject* clone(const char* newname) const { return new RooBsProperTime(*this,newname); }
  inline virtual ~RooBsProperTime() { }

protected:
  RooRealProxy x;
  RooRealProxy tau;
  RooRealProxy dG;
  RooRealProxy aD;
  RooRealProxy a;
  RooRealProxy n;
  RooRealProxy b;
  RooRealProxy t0;
  RooRealProxy mu;  
  int type;
  Double_t evaluate() const;

private:
  ClassDef(RooBsProperTime,1) // ProperTime PDF
};

#endif
