// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:09 GMT

#include "RooRandom.h"
#include "Roo4BodyPhaseSpace.h"
#include "Mint/Calculate4BodyProps.h"
#include <complex>
#include <iostream>
using namespace std;

Roo4BodyPhaseSpace::Roo4BodyPhaseSpace(const char* name
				       , const char* title
				       , RooAbsReal& t01_in
				       , RooAbsReal& s12_in
				       , RooAbsReal& s23_in
				       , RooAbsReal& s34_in
				       , RooAbsReal& t40_in
				       , RooAbsReal& m0_in
				       , RooAbsReal& m1_in
				       , RooAbsReal& m2_in
				       , RooAbsReal& m3_in
				       , RooAbsReal& m4_in
				       )
  : RooAbsPhaseSpace(name, title)
  , t01("t01", "t01", this, t01_in)
  , s12("s12", "s12", this, s12_in)
  , s23("s23", "s23", this, s23_in)
  , s34("s34", "s34", this, s34_in)
  , t40("t40", "t40", this, t40_in)
  , m0("m0", "m0", this, m0_in)
  , m1("m1", "m1", this, m1_in)
  , m2("m2", "m2", this, m2_in)
  , m3("m3", "m3", this, m3_in)
  , m4("m4", "m4", this, m4_in)
{
  TLorentzVector mumsP4(0, 0, 0, m0);
  Double_t mdgt[] = {m1, m2, m3, m4};
  if(! gps.SetDecay(mumsP4, 4, mdgt)){
    cout << " ERROR in  Roo4BodyPhaseSpace::Roo4BodyPhaseSpace:\n"
	 << " Decay kinematically impossible!!" << endl;
    throw " ERROR in  Roo4BodyPhaseSpace::Roo4BodyPhaseSpace: forbidden decay";
  }
}

Roo4BodyPhaseSpace::Roo4BodyPhaseSpace(const Roo4BodyPhaseSpace& other
					   , const char* name
				       )
  : RooAbsPhaseSpace(other, name)
  , gps(other.gps)
  , t01("t01", this, other.t01)
  , s12("s12", this, other.s12)
  , s23("s23", this, other.s23)
  , s34("s34", this, other.s34)
  , t40("t40", this, other.t40)
  , m0("m0", this, other.m0)
  , m1("m1", this, other.m1)
  , m2("m2", this, other.m2)
  , m3("m3", this, other.m3)
  , m4("m4", this, other.m4)
{
  
}

Int_t Roo4BodyPhaseSpace::getGenerator(const RooArgSet &dirVars
				       , RooArgSet &genVars
				       , Bool_t staticInitOK
				       ) const{

  //cout << " getGenerator got called" << endl;
  //cout << " dirVars: " << dirVars << endl;
  //cout << " genVars: " << genVars << endl;
  RooArgSet allArgs(t01.arg()
		    , s12.arg(), s23.arg(), s34.arg()
		    , t40.arg()
		    );
  //cout << " args to be matched " << allArgs << endl;
  if(matchArgs(dirVars, genVars, allArgs)){
    //    cout << " getGenerator returning 5" << endl;
    return 5;
  }
  RooArgSet allBut_t01(     s12.arg(), s23.arg(), s34.arg(), t40.arg());
  if(matchArgs(dirVars, genVars, allBut_t01)){
    return 1;
  }

  /*
  RooArgSet allBut_s12(t01.arg(),      s23.arg(), s34.arg(), t40.arg());
  RooArgSet allBut_s23(t01.arg(), s12.arg(),      s34.arg(), t40.arg());
  RooArgSet allBut_s34(t01.arg(), s12.arg(), s23.arg(),      t40.arg());
  RooArgSet allBut_t40(t01.arg(), s12.arg(), s23.arg(), s34.arg()     );
  */

  cout << " getGenerator returning 0" << endl;
  return 0;
}

void Roo4BodyPhaseSpace::generateEvent(Int_t code){
  if(code == 5){
    TLorentzVector mumsP4(0, 0, 0, m0);
    Double_t mdgt[] = {m1, m2, m3, m4};
    gps.SetDecay(mumsP4, 4, mdgt);
    double maxWeight = gps.GetWtMax();
    //    cout << "maxWeight = " << maxWeight << endl;
    //    cout << "generating event " << endl;
    double weight=0;
    while((weight=gps.Generate()) < 
	  RooRandom::randomGenerator()->Rndm() * maxWeight);
    // generate until the weight of the event is larger than rnd number
    // (and then give the event the weight 1)

    TLorentzVector p[4];
    //double t01, s12, s23, s34, t40;
    for(int j=0; j<4; j++){
      p[j] = *gps.GetDecay(j);
    }
    TLorentzVector pt01 = mumsP4 - p[0];
    t01 = pt01.M2();

    TLorentzVector ps12 = p[0] + p[1];
    s12 = ps12.M2();

    TLorentzVector ps23 = p[1] + p[2];
    s23 = ps23.M2();

    TLorentzVector ps34 = p[2] + p[3];
    s34 = ps34.M2();

    TLorentzVector pt40 = p[3] - mumsP4;
    t40 = pt40.M2();

    /*
    cout << "generated: " << t01 << ", " << s12
	 << ", " << s23 << ", " << s34 << ", " << t40 << endl;
    cout << " giving in evaluate: " << evaluate() << endl;
    */
  }else if(code == 1){// we know t01, generate the rest.

    cout << "using generator with code = 1!!!" << endl;
    // t01 = (p0 - p1)^2 is given - equivalent to
    // (p0 - p1) = p2 + p3 + p4
    // i.e. decay of particle with m^2 = t01
    // p1 is given by: 
    // (p0 - p1)^2 = m0^2 + m1^2 - 2*E0*E1
    //             = m0^2 + m1^2 - 2*m0*sqrt(m1^2 + |p1|^2);
    // treat like 3 particle decay of mother particle
    // with m = t01, and p = (0,0,-p1)
    //
    double x01 = (m0*m0 + m1*m1 - t01)/2.0;
    double E1  = x01/m0;
    double p1sq = E1*E1 - m1*m1;
    double p1  = sqrt(p1sq);

    TLorentzVector mumsP4(0, 0, -p1, E1 + m0);
    Double_t mdgt[] = {m2, m3, m4};
    if(! gps.SetDecay(mumsP4, 3, mdgt)) return;

    double weight=0;
    while((weight=gps.Generate()) < RooRandom::randomGenerator()->Rndm());
    // generate until the weight of the event is larger than rnd number
    // (and then give the event the weight 1)

    TLorentzVector p[4];
    //double t01, s12, s23, s34, t40;

    p[0].SetXYZT(0,0,p1,E1);

    for(int j=1; j<4; j++){
      p[j] = *gps.GetDecay(j);
    }
    TLorentzVector pt01 = p[0] + p[1] + p[2];
    cout << " t01 = " << t01 << "  =  " << pt01.M2() << " ? " << endl;

    TLorentzVector ps12 = p[0] + p[1];
    s12 = ps12.M2();

    TLorentzVector ps23 = p[1] + p[2];
    s23 = ps23.M2();

    TLorentzVector ps34 = p[2] + p[3];
    s34 = ps34.M2();

    TLorentzVector pt40 = p[1] + p[2] + p[3];
    t40 = pt40.M2();

    /*
    cout << "generated: " << t01 << ", " << s12
	 << ", " << s23 << ", " << s34 << ", " << t40 << endl;
    cout << " giving in evaluate: " << evaluate() << endl;
    */
  }else{
    std::cout << " ERROR - generate event called with code " << code
	      << " - don't know that code." << endl;
    return;
  }
}


void Roo4BodyPhaseSpace::getInvariantsFromPArray(TLorentzVector p[4]){
  TLorentzVector pt01 = p[0] + p[1] + p[2];
  cout << " t01 = " << t01 << "  =  " << pt01.M2() << " ? " << endl;
  
  TLorentzVector ps12 = p[0] + p[1];
  s12 = ps12.M2();
  
  TLorentzVector ps23 = p[1] + p[2];
  s23 = ps23.M2();
  
  TLorentzVector ps34 = p[2] + p[3];
  s34 = ps34.M2();
  
  TLorentzVector pt40 = p[1] + p[2] + p[3];
  t40 = pt40.M2();
}

Int_t Roo4BodyPhaseSpace::getAnalyticalIntegral(RooArgSet& allVars
						, RooArgSet& analVars
						, const char* rangeName
						) const{
  
  //  return 0;
  RooArgSet allArgs(t01.arg()
		    , s12.arg(), s23.arg(), s34.arg()
		    , t40.arg()
		    );
  if(matchArgs(allVars, analVars, allArgs)){
    return 5;
  }
  return 0;
  if(matchArgs(allVars, analVars, t01)){
    return 1;
  }
  if(matchArgs(allVars, analVars, s12)){
    return 12;
  }
  if(matchArgs(allVars, analVars, s23)){
    return 23;
  }
  if(matchArgs(allVars, analVars, s34)){
    return 34;
  }
  if(matchArgs(allVars, analVars, t40)){
    return 40;
  }
  cout << " getAnalyticalIntegral returning 0" << endl;
  return 0;
}

Bool_t Roo4BodyPhaseSpace::decayInLimits(TLorentzVector p[4]) const{
      TLorentzVector pt01 = p[1] + p[2] + p[3];
      double this_t01 = pt01.M2();
      if(this_t01 > t01.max() || this_t01 < t01.min()) return false;
      
      TLorentzVector ps12 = p[0] + p[1];
      double this_s12 = ps12.M2();
      if(this_s12 > s12.max() || this_s12 < s12.min()) return false;
     
      TLorentzVector ps23 = p[1] + p[2];
      double this_s23 = ps23.M2();
      if(this_s23 > s23.max() || this_s23 < s23.min()) return false;

      TLorentzVector ps34 = p[2] + p[3];
      double this_s34 = ps34.M2();
      if(this_s34 > s34.max() || this_s34 < s34.min()) return false;

      TLorentzVector pt40 = p[0] + p[1] + p[2];
      double this_t40 = pt40.M2();
      if(this_t40 > t40.max() || this_t40 < t40.min()) return false;

      return true;
}

Double_t Roo4BodyPhaseSpace::analyticalIntegral(Int_t code
						, const char* rangeName
						) const{

  bool printalot = false;
  if(code == 5){
    TLorentzVector mumsP4(0, 0, 0, m0);
    Double_t mdgt[] = {m1, m2, m3, m4};
    if(! gps.SetDecay(mumsP4, 4, mdgt)) return 0;
    
    double weight=0;
    double delta = 1000;
    double prev=0;
    int n=0;
    double result=0;
    double sum=0;
    double sumsq = 0;

    double precision = 0.001;
    double rms = 9999;
    
    int eventsThisTry=100;
    int nTries=0;
    return 1;

    
    do{
      nTries++;
      for(int i=0; i < eventsThisTry; i++){
	weight=gps.Generate();
	TLorentzVector p[4];
	//double t01, s12, s23, s34, t40;
	for(int j=0; j<4; j++){
	  p[j] = *gps.GetDecay(j);
	}
	if(decayInLimits(p)){
	  n++;
	  sum   += weight;
	  sumsq  = weight*weight;
	}
      }

      result = sum/((double)n);
      cout << "5-integrator, " << nTries << ": " << result << endl;
      delta = fabs( result - prev);
      prev = result;
      Double_t mean   = sum  /((double) nTries);
      Double_t meansq = sumsq/((double) nTries);
      Double_t variance = meansq - mean*mean;
      rms = sqrt(variance);
      
    }while(rms > precision);
    
    return result;
  }else if(code == 1){// integ over t01
    Calculate4BodyProps c4bp(t01
			     , s12, s23, s34
			     , t40
			     , m0
			     , m1, m2, m3, m4
			     );
    Double_t mi, ma;
    c4bp.t40MinMax(mi, ma);
    if(printalot)cout << "1-integrator, for " 
		      << "t01 " << s12 << ", " << s23 
		      << ", " << s34 << ", " << t40
		      <<  " I return: " << ma-mi << endl;
    return (ma-mi);

  }else if(code == 12){
    Calculate4BodyProps c4bp(t01
			     , s12, s23, s34
			     , t40
			     , m0
			     , m1, m2, m3, m4
			     );
    Double_t mi, ma;
    c4bp.s12MinMax(mi, ma);
    if(printalot)cout << "12-integrator, for " 
		      << t01 << ", s12, " << s23 << ", " << s34 << ", " << t40
		      <<  " I return: " << ma-mi << endl;
    return (ma-mi);
    
  }else if(code == 23){
    Calculate4BodyProps c4bp(t01
			     , s12, s23, s34
			     , t40
			     , m0
			     , m1, m2, m3, m4
			     );
    Double_t mi, ma;
    c4bp.s23MinMax(mi, ma);
    if(mi==ma) return 0;
    return (ma-mi);
  }else if(code == 34){
    Calculate4BodyProps c4bp(t01
			     , s12, s23, s34
			     , t40
			     , m0
			     , m1, m2, m3, m4
			     );
    Double_t mi, ma;
    c4bp.s34MinMax(mi, ma);
    if(mi==ma) return 0;
    return (ma-mi);
  }else if(code == 40){
    Calculate4BodyProps c4bp(t01
			     , s12, s23, s34
			     , t40
			     , m0
			     , m1, m2, m3, m4
			     );
    Double_t mi, ma;
    c4bp.t40MinMax(mi, ma);
    if(mi==ma) return 0;
    return (ma-mi);
  }else{
    cout << " ERROR in Roo4BodyPhaseSpace::analyticalIntegral:"
	 << " I got called with unknown code " << code << endl;
    return -9999;
  }
  
}

Double_t Roo4BodyPhaseSpace::evaluate() const{
  Calculate4BodyProps c4bp(t01
			   , s12, s23, s34
			   , t40
			   , m0
			   , m1, m2, m3, m4
			   );
  bool printalot = false;
  if(printalot){
    cout << " for " << t01 << ", "
	 << s12 << ", " << s23 << ", " << s34
	 << ", " << t40 << endl;
    cout << " (and " << m0 << ", " << m1
	 << ", " << m2 << ", " << m3 << ", " << m4
	 << ")" << endl;
    
    cout << "  Grahm 1 " << c4bp.Delta1() 
	 << ", " << c4bp.G(1,1) << endl;
    cout << "  Grahm 2 " << c4bp.Delta2() 
	 << ", " << c4bp.G(1,2, 1,2) <<  endl;
    cout << "  Grahm 3 " << c4bp.Delta3() 
	 << ", " << c4bp.G(1,2,3 , 1,2,3) << endl;
    cout << "  Grahm 4 " << c4bp.Delta4()  
	 << ", " << c4bp.G(1,2,3,4, 1,2,3,4) << endl;

    
    Double_t mi, ma;
    c4bp.t01MinMax(mi, ma);
    cout << "Distance to edge: t01 " 
	 << "mi = " << mi << ", ma= " << ma << "; dist "
	 << t01-mi << ", " << t01-ma << endl;
    c4bp.s12MinMax(mi, ma);
    cout << "Distance to edge: s12 " 
	 << "mi = " << mi << ", ma= " << ma << "; dist "
	 << s12-mi << ", " << s12-ma << endl;
    c4bp.s23MinMax(mi, ma);
    cout << "Distance to edge: s23 "  
	 << "mi = " << mi << ", ma= " << ma << "; dist "
	 << s23-mi << ", " << s23-ma << endl;
    c4bp.s34MinMax(mi, ma);
    cout << "Distance to edge: s34 " 
	 << "mi = " << mi << ", ma= " << ma << "; dist "
	 << s34-mi << ", " << s34-ma << endl;
    c4bp.t40MinMax(mi, ma);
    cout << "Distance to edge: t40 " 
	 << "mi = " << mi << ", ma= " << ma << "; dist "
	 << t40-mi << ", " << t40-ma << endl;

  }

  if( c4bp.Delta1() < 0){
    if(printalot)cout << "returning 0" << endl;
    return 0;
  }
  if( c4bp.Delta2() > 0){
    if(printalot)cout << "returning 0" << endl;
    return 0;
  }
  if( c4bp.Delta3() < 0){
    if(printalot)cout << "returning 0" << endl;
    return 0;
  }
  double d4 = c4bp.Delta4();
  if(printalot)cout << "compare deltas: "
		    << d4 << ", " << c4bp.Delta4_doesntWork()
		    << ", D1234= " << c4bp.Delta(1, 2, 3, 4)
		    << ", D0123= " << c4bp.Delta(0, 1, 2, 3)
		    << endl;
  if(d4 >= -1.e-100){
    if(printalot)cout << "returning 0" << endl;
    return 0;
  }

  if(printalot)cout << " masses : " 
		    << c4bp.p0().M() << "; "
		    << c4bp.p1().M() << ", " << c4bp.p2().M()  << ", "
		    << c4bp.p3().M() << ", " << c4bp.p4().M() << endl;
  
  double returnVal = TMath::Pi()*TMath::Pi()/
    (32.0*m0*m0 * sqrt(-d4));
  if(printalot)cout << "returning " << returnVal << endl;

  return returnVal;
}

//
