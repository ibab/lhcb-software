
#include "RooFit.h"

#include "Riostream.h"
#include "Riostream.h"
#include <math.h>

#include "RooBsProperTime.h"
#include "RooRealVar.h"
#include "RooRealConstant.h"

ClassImp(RooBsProperTime)


//_____________________________________________________________________________
RooBsProperTime::RooBsProperTime(const char *name, const char *title,
                                 RooAbsReal& _x, RooAbsReal& _tau,  
                                 RooAbsReal& _dG,RooAbsReal& _aD,
                                 RooAbsReal& _a, RooAbsReal& _n, RooAbsReal& _b, RooAbsReal& _t0,
                                 RooAbsReal& _mu,int _type) :
  RooAbsPdf(name, title), 
  x("x","m",this,_x),
  tau("tau","tau",this,_tau),
  dG("dG","dG",this,_dG),
  aD("aD","aD",this,_aD),
  a("a","a",this,_a),
  n("n","n",this,_n),
  b("b","b",this,_b),
  t0("t0","t0",this,_t0),
  mu("mu","mu",this,_mu),
  type(_type)
{
}
RooBsProperTime::RooBsProperTime(const char *name, const char *title,
                                 RooAbsReal& _x, RooAbsReal& _tau,  
                                 RooAbsReal& _dG,RooAbsReal& _aD,
                                 RooAbsReal& _a, RooAbsReal& _n, RooAbsReal& _b, RooAbsReal& _t0,int _type) :
  RooAbsPdf(name, title), 
  x("x","m",this,_x),
  tau("tau","tau",this,_tau),
  dG("dG","dG",this,_dG),
  aD("aD","aD",this,_aD),
  a("a","a",this,_a),
  n("n","n",this,_n),
  b("b","b",this,_b),
  t0("t0","t0",this,_t0),
  mu("mu","mu",this,(RooRealVar&)RooRealConstant::value(0.)),
  type(_type)
{
}

//_____________________________________________________________________________
RooBsProperTime::RooBsProperTime(const RooBsProperTime& other, const char* name) :
  RooAbsPdf(other, name), x("x",this,other.x), tau("tau",this,other.tau), 
  dG("dG",this,other.dG), aD("aD",this,other.aD), 
  a("a",this,other.a), n("n",this,other.n), b("b",this,other.b), t0("t0",this,other.t0) ,
  mu("mu",this,other.mu),type(other.type)
{
}


//_____________________________________________________________________________
Double_t RooBsProperTime::evaluate() const{ 

  double_t t= x + mu;
  if(t-t0<=0.)return 0.;

  double rate = exp(t/tau)* ( cosh( dG/2. * t ) - aD * sinh ( dG/2. * t ) );
  double acc  = 0.;
  if(type==0)acc=a*pow(fabs(t-t0),n)/(1.+a*pow(fabs(t-t0),n))*exp(b*t);
  if(type==1)acc=pow(a*fabs(t-t0),n)/(1.+pow(a*fabs(t-t0),n))*exp(b*t);
  double f  = rate * acc;
  return (f>0.) ? f : 0. ;
}



