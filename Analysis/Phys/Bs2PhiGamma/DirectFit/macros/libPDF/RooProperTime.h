#ifndef ROO_PROPERTIME
#define ROO_PROPERTIME

#include "RooAbsPdf.h"
#include "RooRealProxy.h"

class RooRealVar;
class RooAbsReal;

class RooProperTime : public RooAbsPdf {
public:
  RooProperTime() {} ;
  RooProperTime(const char *name, const char *title,
                RooAbsReal& _x, RooAbsReal& _c, RooAbsReal& _a, RooAbsReal& _n, RooAbsReal& _b, RooAbsReal& _t0,int type=0);
  RooProperTime(const RooProperTime& other, const char* name=0);
  virtual TObject* clone(const char* newname) const { return new RooProperTime(*this,newname); }
  inline virtual ~RooProperTime() { }

protected:
  RooRealProxy x;
  RooRealProxy c;
  RooRealProxy a;
  RooRealProxy n;
  RooRealProxy b;
  RooRealProxy t0;
  int type;

  Double_t evaluate() const;

private:
  ClassDef(RooProperTime,1) // ProperTime PDF
};

#endif
