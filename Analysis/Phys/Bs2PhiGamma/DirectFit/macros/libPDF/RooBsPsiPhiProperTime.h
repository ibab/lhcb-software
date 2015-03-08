#ifndef ROOBSPSIPHIPROPERTIME
#define ROOBSPSIPHIPROPERTIME

#include "RooAbsPdf.h"
#include "RooRealProxy.h"

class RooRealVar;
class RooAbsReal;

class RooBsPsiPhiProperTime : public RooAbsPdf {
public:
  RooBsPsiPhiProperTime() {} ;
  RooBsPsiPhiProperTime(const char *name, const char *title,
                  RooAbsReal& _x, RooAbsReal& _tau, 
                  RooAbsReal& _dG,RooAbsReal& _Rl,RooAbsReal& _Rh,
                  RooAbsReal& _a, RooAbsReal& _n, RooAbsReal& _b, RooAbsReal& _t0,
                  RooAbsReal& _mu,int _type=0);


  RooBsPsiPhiProperTime(const char *name, const char *title,
                  RooAbsReal& _x, RooAbsReal& _tau, 
                  RooAbsReal& _dG,RooAbsReal& _Rl,RooAbsReal& _Rh,
                  RooAbsReal& _a, RooAbsReal& _n, RooAbsReal& _b, RooAbsReal& _t0,int _type=0);

  RooBsPsiPhiProperTime(const RooBsPsiPhiProperTime& other, const char* name=0);
  virtual TObject* clone(const char* newname) const { return new RooBsPsiPhiProperTime(*this,newname); }
  inline virtual ~RooBsPsiPhiProperTime() { }

protected:
  RooRealProxy x;
  RooRealProxy tau;
  RooRealProxy dG;
  RooRealProxy Rl;
  RooRealProxy Rh;
  RooRealProxy a;
  RooRealProxy n;
  RooRealProxy b;
  RooRealProxy t0;
  RooRealProxy mu;  
  int type;
  Double_t evaluate() const;

private:
  ClassDef(RooBsPsiPhiProperTime,1) // ProperTime PDF
};

#endif
