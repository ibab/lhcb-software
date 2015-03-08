
#include "RooFit.h"

#include "Riostream.h"
#include "Riostream.h"
#include <math.h>

#include "RooProperTime.h"
#include "RooRealVar.h"

ClassImp(RooProperTime)


//_____________________________________________________________________________
RooProperTime::RooProperTime(const char *name, const char *title,
              RooAbsReal& _x, RooAbsReal& _c, RooAbsReal& _a, RooAbsReal& _n, RooAbsReal& _b, RooAbsReal& _t0,int _type) :
  RooAbsPdf(name, title), 
  x("x","m",this,_x),
  c("c","c",this,_c),
  a("a","a",this,_a),
  n("n","n",this,_n),
  b("b","b",this,_b),
  t0("t0","t0",this,_t0),
  type(_type)
{
}


//_____________________________________________________________________________
RooProperTime::RooProperTime(const RooProperTime& other, const char* name) :
  RooAbsPdf(other, name), x("x",this,other.x), c("c",this,other.c), a("a",this,other.a), n("n",this,other.n), b("b",this,other.b), t0("t0",this,other.t0) ,type(other.type) 
{
}


//_____________________________________________________________________________
Double_t RooProperTime::evaluate() const{ 
  if(x-t0<0.)return 0.;
  double f=0;
  if(type==0)f=exp(x/c)*( a*pow(fabs(x-t0),n))/(1.+a*pow(fabs(x-t0),n))*exp(b*x);//*(1.+b*x); 
  if(type==1)f=exp(x/c)*( pow(a*fabs(x-t0),n))/(1.+pow(a*fabs(x-t0),n))*exp(b*x); 

  return (f>0.) ? f : 0. ;
}



