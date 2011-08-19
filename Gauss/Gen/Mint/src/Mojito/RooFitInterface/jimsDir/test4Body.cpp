// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:10 GMT
#include <ctime>
#include "RooGlobalFunc.h"
#include "RooGaussian.h"
#include "RooRealVar.h"
#include "RooAbsArg.h"
#include "RooPlot.h"
#include "RooDataSet.h"

#include "TLorentzVector.h"

#include "TGenPhaseSpace.h"
#include "TRandom.h"

#include "Mint/Calculate4BodyProps.h"
#include "fourMxy.h"
#include "Roo4BodyPhaseSpace.h"

#include "DKKpipiResonances.h"
#include "Grahm.h"

#include "Mint/SpinFactors.h"

using namespace RooFit;
using namespace DKKpipiResonances;

std::ostream& operator<<(std::ostream& os, const TLorentzVector& v){
  os << "(" << v.T() << ", " << v.X() << ", " << v.Y() << ", " << v.Z() << ")";
  return os;
}

void test4Body(){
  cout << "Hello from test4Body " << endl;
  
  double mMum    = DZero_M.getVal();
  double mdgt[4] = {Kplus_M.getVal()
		    , Kminus_M.getVal()
		    , piPlus_M.getVal()
		    , piMinus_M.getVal()};


  int N = 10;
  
  TGenPhaseSpace gps;
  
  TRandom rnd;


  RooRealVar t01("t01", "t01", -100, 100);
  RooRealVar s12("s12", "s12", -100, 100);
  RooRealVar s23("s23", "s23", -100, 100);
  RooRealVar s34("s34", "s34", -100, 100);
  RooRealVar t40("t40", "t40", -100, 100);

  RooRealVar m0("m0", "m0", mMum);
  RooRealVar m1("m1", "m1", mdgt[0]);
  RooRealVar m2("m2", "m2", mdgt[1]);
  RooRealVar m3("m3", "m3", mdgt[2]);
  RooRealVar m4("m4", "m4", mdgt[3]);

  fourMxy s0("s0", "MxySq"
	       , t01, s12, s23, s34, t40
	       , m0, m1, m2, m3, m4
	       , 13
	       );
  fourMxy s13(s0, 13, "s14");
  fourMxy s14(s0, 14, "s14");
  fourMxy s24(s0, 24, "s24");
  fourMxy s123(s0, 123, "s123");
  fourMxy s124(s0, 124, "s124");
  fourMxy s134(s0, 134, "s134");
  fourMxy s234(s0, 234, "s234");


  Roo4BodyPhaseSpace phaseSpace("phaseSpace", "phaseSpace"
				, t01
				, s12, s23, s34
				, t40
				, m0
				, m1, m2, m3, m4
				);
  RooRealVar K1("K1", "K1", 1.270);
  RooRealVar rho("rho", "rho", 0.770);
  
  SF_DtoAP1_AtoVP2_VtoP3P4 sf("spinFactor", "spinFactor"
			      , K1, rho
			      , 1, 2, 3, 4
			      , t01
			      , s12, s23, s34
			      , t40
			      , m0
			      , m1, m2, m3, m4
			      );
  
  for(int i=0; i<N; i++){
    double mumsMom = rnd.Exp(5);
    TLorentzVector mumsP4(0, 0, mumsMom, sqrt(mumsMom*mumsMom + mMum*mMum));
    if(! gps.SetDecay(mumsP4, 4, mdgt)){
      cout << "decay not kinematically allowed - bailing out" << endl;
      throw "forbidden decay";
    }
    Double_t weight = gps.Generate();
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

    cout << "spin factor " << sf.getVal() << endl;

    Calculate4BodyProps c4bp(t01.getVal()
			     , s12.getVal(), s23.getVal(), s34.getVal()
			     , t40.getVal()
			     , mMum
			     , mdgt[0], mdgt[1], mdgt[2], mdgt[3]
			     );

    cout << "Grahm 1 " << c4bp.Delta1() << endl;
    cout << "Grahm 2 " << c4bp.Delta2() << endl;
    cout << "Grahm 3 " << c4bp.Delta3() << endl;
    cout << "Grahm 4 " << c4bp.Delta4() << endl;

    TLorentzVector minusP4=p[3];
    minusP4 *= -1;

    cout << " more Grahm: "
	 << c4bp.Delta4_doesntWork() << ", "
	 << symGrahm(4
		     , &mumsP4, &minusP4, &p[0], &p[1])
	 << ", "
	 << symGrahm(4
		     , &mumsP4, &p[3], &p[0], &p[1])
	 << ", "
	 << symGrahm(4
		     , &p[0], &p[1], &p[2], &p[3])
	 << ", "
	 << symGrahm(4
		     , &mumsP4, &p[0], &p[1], &p[2])
	 << endl;

    cout << "Compare... " << endl;
    cout << "s123 " << s123.getVal() << ", "  << c4bp.s123() << ", "
	 << (p[0]+p[1]+p[2]).M2() << endl;

    cout << "s234 " << s234.getVal() << ", "  << c4bp.s234() << ", "
	 << (p[1]+p[2]+p[3]).M2() << endl;

    cout << "s124 " << s124.getVal() << ", "  << c4bp.s124() << ", "
	 << (p[0]+p[1]+p[3]).M2() << endl;

    cout << "s14 " << s14.getVal() << ", "    << c4bp.s14() << ", "
	 << (p[0]+p[3]).M2() << endl;

    cout << "s134 "  << s134.getVal() << ", "  << c4bp.s134() << ", "
	 << (p[0]+p[2]+p[3]).M2() << endl;

    cout << "s13 " << s13.getVal() << ", "  << c4bp.s13() << ", "
	 << (p[0]+p[2]).M2() << endl;
    
    cout << "s24 " << s24.getVal() << ", "  << c4bp.s24() << ", "
	 << (p[1]+p[3]).M2() << endl;


    cout << " the momenta.... " << endl;
    cout << p[0] << ", " << c4bp.p1() 
	 << "Morg " << p[0].M() << " M= " << c4bp.p1().M() << endl;
    cout << p[1] << ", " << c4bp.p2() 
	 << "Morg " << p[1].M()  << " M= " << c4bp.p2().M() << endl;
    cout << p[2] << ", " << c4bp.p3() 
	 << "Morg " << p[2].M()  << " M= " << c4bp.p3().M() << endl;
    cout << p[3] << ", " << c4bp.p4() 
	 << "Morg " << p[3].M()  << " M= " << c4bp.p4().M() << endl;
    

    double ps = phaseSpace.getVal();
    double ratio = -999;
    if(weight > 0) ratio = ps/weight;
    cout << "weight " << weight 
	 << ", phase space " << ps
	 << ", ratio: " << ratio
	 << endl;
    
  }
  
}
//
//

