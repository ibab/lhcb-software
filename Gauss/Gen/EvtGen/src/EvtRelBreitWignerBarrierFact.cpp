//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 1998      Caltech, UCSB
//
// Module: EvtLineShape.cc
//
// Description: Store particle properties for one particle.
//
// Modification history:
//
//    Lange     March 10, 2001        Module created
//
//------------------------------------------------------------------------
//
#include <iostream>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>

#include "EvtGen/EvtRelBreitWignerBarrierFact.hh"
#include "EvtGen/EvtRandom.hh"


EvtRelBreitWignerBarrierFact::EvtRelBreitWignerBarrierFact() {

}

EvtRelBreitWignerBarrierFact::~EvtRelBreitWignerBarrierFact() {
}

EvtRelBreitWignerBarrierFact::EvtRelBreitWignerBarrierFact(double mass, double width, double maxRange, double mDaug1, double mDaug2, int l) {

  _mass=mass;
  _width=width;
  double maxdelta=0.6;
  if ( 5.0*width > 0.6 ) maxdelta = 5.0*width;

  if ( maxRange > 0.00001 ) {
    _massMax=mass+maxdelta;
    _massMin=mass-maxRange;
  }
  else{
    _massMax=mass+maxdelta;
    _massMin=mass-5.0*width;
  }
 
  _m1 = mDaug1;
  _m2 = mDaug2;
  _l = l;

}

EvtRelBreitWignerBarrierFact::EvtRelBreitWignerBarrierFact(const EvtRelBreitWignerBarrierFact& x)
  : EvtAbsLineShape() {
  _mass=x._mass;
  _width=x._width;
  _massMax=x._massMax;
  _massMin=x._massMin;

  _m1=x._m1;
  _m2=x._m2;
  _l=x._l;

}

EvtRelBreitWignerBarrierFact& EvtRelBreitWignerBarrierFact::operator=(const EvtRelBreitWignerBarrierFact& x){
  _mass=x._mass;
  _massMax=x._massMax;
  _massMin=x._massMin;
  _width=x._width;
  _m1=x._m1;
  _m2=x._m2;
  _l=x._l;
  
  return *this;

}

EvtAbsLineShape* EvtRelBreitWignerBarrierFact::clone() {

  return new EvtRelBreitWignerBarrierFact(*this);
}


double EvtRelBreitWignerBarrierFact::getBlattWeisskof(double p, double pAB) {

  // p is the daughter momentum for this mass
  // pAB is the daughter momentum for the default mass

  //double rad=1.5;
  double rad=3.0;
  double radP = rad*rad*p*p;
  double radPAB = rad*rad*pAB*pAB;
  if ( _l == 0 ) {
    return 1;
  }
  if ( _l == 1 ) {
    return (1.0+radP)/(1.0+radPAB);
  }
  if ( _l == 2 ) {
    return (9.0+3.0*radP+radP*radP)/(1.0+radPAB+radPAB*radPAB);
  }
  std::cout << "in getBlattWeisskof - you gave an l that was unknown\n";
  assert(0);
  return 0;
}

double EvtRelBreitWignerBarrierFact::rollMass() {

  //double ymin, ymax;
  double temp;

  if ( _width < 0.0001 ) {
    return _mass;
  }
  else{
    //    ymin = atan( 2.0*(_massMin-_mass)/_width);
    //    ymax = atan( 2.0*(_massMax-_mass)/_width);

    //    temp= ( _mass + ((_width/2.0)*tan(EvtRandom::Flat(ymin,ymax))));

    //double qr = sqrt( ((_mass*_mass-_m1*_m1-_m2*_m2)/4.0 - _m1*_m1*_m2*_m2)/
    //	      (_mass*_mass+_m1*_m1+_m2*_m2));

    double xtemp= (_mass*_mass-_m1*_m1-_m2*_m2)/2.0;
    double qr = sqrt( (xtemp*xtemp - _m1*_m1*_m2*_m2)/
		      (_mass*_mass));

    int accept=0;

    while ( !accept ) {
      temp = EvtRandom::Flat(_massMin,_massMax);
      
      double xtemp2= (temp*temp-_m1*_m1-_m2*_m2)/2.0;
	//double q = sqrt( ((temp*temp-_m1*_m1-_m2*_m2)/4.0 - _m1*_m1*_m2*_m2)/
	//	      (temp*temp+_m1*_m1+_m2*_m2));
      double q = sqrt( (xtemp2*xtemp2 - _m1*_m1*_m2*_m2)/
			  (temp*temp));

      double phaseSpaceFact = pow((q/qr),(2*_l+1));

      double gamma = phaseSpaceFact*(_mass/temp)*(getBlattWeisskof(qr,q))*_width;

      //I think I'm leaving out a phase space factor....
      //mass -> _temp jul10,2001 - lange
      double sigma_num = _mass*gamma*_mass;
      double sigma_rdem = _mass*_mass - temp*temp;
      double sigma_idem = _mass*gamma;
      double sigma = sigma_num / ( sigma_rdem*sigma_rdem +
				   sigma_idem*sigma_idem );
      double tRand = EvtRandom::Flat();
      if ( sigma > tRand ) { accept=1;}

    }
    return temp;
  }
}








