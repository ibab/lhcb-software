//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 1999    Stanford University
//
// Module: EvtVSSBMix.cc
//
// Description:
//    Routine to decay vector-> scalar scalar with coherent BB-like mixing
//
// Modification history:
//
//    D. Kirkby   October 1, 1999    Module created (adapted from VSSMix)
//    D. Lange    March 16, 2001 Added deltaGamma and q over p 
//
//------------------------------------------------------------------------
// 
#include <stdlib.h>
#include "EvtGen/EvtConst.hh"
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtGenKine.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtVector4C.hh"
#include "EvtGen/EvtVSSBMix.hh"
#include "EvtGen/EvtId.hh"
#include "EvtGen/EvtString.hh"
#include "EvtGen/EvtRandom.hh"

EvtVSSBMix::~EvtVSSBMix() {}

void EvtVSSBMix::getName(EvtString& model_name){
  model_name="VSS_BMIX";
}


EvtDecayBase* EvtVSSBMix::clone(){
  return new EvtVSSBMix;
}

void EvtVSSBMix::init(){
  // check that there we are provided exactly one parameter
  if (getNArg()>3) {
    report(ERROR,"EvtGen") << "EvtVSSBMix generator expected "
                           << " no more than 3 arguments but found:"<<getNArg()<<std::endl;
    report(ERROR,"EvtGen") << "Will terminate execution!"<<std::endl;
    ::abort();
  }
  if (getNArg()<1) {
    report(ERROR,"EvtGen") << "EvtVSSBMix generator expected "
                           << " at least 1 argument (deltam) but found:"<<getNArg()<<std::endl;
    report(ERROR,"EvtGen") << "Will terminate execution!"<<std::endl;
    ::abort();
  }
  // check that we are asked to produced exactly 2 daughters
  //4 are allowed if they are aliased..
  checkNDaug(2,4);

  if ( getNDaug()==4) {
    if ( getDaug(0)!=getDaug(2)||getDaug(1)!=getDaug(3)){
      report(ERROR,"EvtGen") << "EvtVSSBMix generator allows "
			     << " 4 daughters only if 1=3 and 2=4"
			     << " (but 3 and 4 are aliased "<<std::endl;
      report(ERROR,"EvtGen") << "Will terminate execution!"<<std::endl;
      ::abort();
    }
  }
  // check that we are asked to decay a vector particle into a pair
  // of scalar particles

  checkSpinParent(EvtSpinType::VECTOR);

  checkSpinDaughter(0,EvtSpinType::SCALAR);
  checkSpinDaughter(1,EvtSpinType::SCALAR);

  // check that our daughter particles are charge conjugates of each other
  if(!(EvtPDL::chargeConj(getDaug(0)) == getDaug(1))) {
    report(ERROR,"EvtGen") << "EvtVSSBMix generator expected daughters "
			   << "to be charge conjugate." << std::endl
			   << "  Found " << EvtPDL::name(getDaug(0)) << " and "
			   << EvtPDL::name(getDaug(1)) << std::endl;
    report(ERROR,"EvtGen") << "Will terminate execution!"<<std::endl;
    ::abort();
  }
  // check that both daughter particles have the same lifetime
  if(EvtPDL::getctau(getDaug(0)) != EvtPDL::getctau(getDaug(1))) {
    report(ERROR,"EvtGen") << "EvtVSSBMix generator expected daughters "
			   << "to have the same lifetime." << std::endl
			   << "  Found ctau = "
			   << EvtPDL::getctau(getDaug(0)) << " mm and "
			   << EvtPDL::getctau(getDaug(1)) << " mm" << std::endl;
    report(ERROR,"EvtGen") << "Will terminate execution!"<<std::endl;
    ::abort();
  }
  // precompute quantities that will be used to generate events
  // and print out a summary of parameters for this decay
  _freq= 0.5*getArg(0)/EvtConst::c; // mixing frequency in hbar/mm

  if ( getNArg() == 3) _qoverp = EvtComplex(getArg(2),0.0); //q over p 
  if ( getNArg() <= 2) _qoverp = EvtComplex(1.0,0.0); //q over p 

  double tau= 1e12*EvtPDL::getctau(getDaug(0))/EvtConst::c; // in ps

  _dGamma=0.0;
  if ( getNArg() >=2 ) { _dGamma = getArg(1)*(1/EvtPDL::getctau(getDaug(0))); } // arg1 is dgamma/gamma

  double dm= 1e-12*getArg(0); // B0/anti-B0 mass difference in hbar/ps
  double x= dm*tau;
  double chi= 0.5*x*x/(1.0+x*x); // time-averaged probability of mixing
  report(INFO,"EvtGen") << "VSS_BMIX will generate mixing in the decay:"
			<< std::endl << std::endl
			<< "    " << EvtPDL::name(getParentId()) << " --> "
			<< EvtPDL::name(getDaug(0)) << " + "
			<< EvtPDL::name(getDaug(1)) << std::endl << std::endl
			<< "using parameters:" << std::endl << std::endl
			<< "  delta(m)  = " << dm << " hbar/ps" << std::endl
                        << "dGamma      = "  << _dGamma <<" ps-1" <<std::endl
                        << "       q/p  = " << _qoverp << std::endl  
			<< "       tau  = " << tau << " ps" << std::endl
			<< "         x  = " << x << std::endl
			<< "       chi  = " << chi << std::endl << std::endl;
}

void EvtVSSBMix::initProbMax(){
  //protect against large delta gamma / gamma
  if (_dGamma > 0 ) {
    setProbMax(10.0);
  }
  else{
    setProbMax(0.5);
  }
}

void EvtVSSBMix::decay( EvtParticle *p ){

  static EvtId B0=EvtPDL::getId("B0");
  static EvtId B0B=EvtPDL::getId("anti-B0");

  // generate a final state according to phase space

  double rndm= EvtRandom::random();

  if ( getNDaug()==4) {
    EvtId tempDaug[2];

    if ( rndm < 0.5 ) { tempDaug[0]=getDaug(0);  tempDaug[1]=getDaug(3); }
    else{ tempDaug[0]=getDaug(2);  tempDaug[1]=getDaug(1); }

    p->initializePhaseSpace(2,tempDaug);
  }
  else{ //nominal case.
    p->initializePhaseSpace(2,getDaugs());
  }

  EvtParticle *s1,*s2;

  s1 = p->getDaug(0);
  s2 = p->getDaug(1);
  
  EvtVector4R p1= s1->getP4();
  EvtVector4R p2= s2->getP4();

  // throw a random number to decide if this final state should be mixed
  rndm= EvtRandom::random();
  int mixed= (rndm < 0.5) ? 1 : 0;

  // if this decay is mixed, choose one of the 2 possible final states
  // with equal probability (re-using the same random number)
  if(mixed) {
    EvtId mixedId= (rndm < 0.25) ? getDaug(0) : getDaug(1);
    EvtId mixedId2= mixedId;
    if (getNDaug()==4&&rndm<0.25)  mixedId2=getDaug(2);
    if (getNDaug()==4&&rndm>0.25)  mixedId2=getDaug(3);
    s1->init(mixedId, p1);
    s2->init(mixedId2, p2);
  }

  // choose a decay time for each final state particle using the
  // lifetime (which must be the same for both particles) in pdt.table
  // and calculate the lifetime difference for this event
  s1->setLifetime();
  s2->setLifetime();
  double dct= s1->getLifetime() - s2->getLifetime(); // in mm

  // calculate the oscillation factor in the amplitude, based on
  // wether this event is mixed or not

  EvtComplex exp1((_dGamma/4.0)*dct,_freq*dct);

  //Find the flavor of the B that decayed first.
  EvtId firstDec = (dct > 0 ) ? s2->getId() : s1->getId();

  //This tags the flavor of the other particle at that time. 
  EvtId stateAtDeltaTeq0 = ( firstDec==B0 ) ? B0B : B0; 

  EvtComplex osc_amp;
  if ( !mixed&&stateAtDeltaTeq0==B0 ) {
    osc_amp = 0.5*(exp(exp1)+exp(-1.0*exp1));
  }
  if ( !mixed&&stateAtDeltaTeq0==B0B ) {
    osc_amp = 0.5*(exp(exp1)+exp(-1.0*exp1));
  }
  if ( mixed&&stateAtDeltaTeq0==B0 ) {
    osc_amp = 0.5*_qoverp*(exp(exp1)-exp(-1.0*exp1));  
  }
  if ( mixed&&stateAtDeltaTeq0==B0B ) {
    osc_amp = 0.5*(1.0/_qoverp)*(exp(exp1)-exp(-1.0*exp1));  
  }

  // calculate and store the amplitudes for each parent spin basis state
  double norm=1.0/p1.d3mag();
  vertex(0,norm*osc_amp*p1*(p->eps(0)));
  vertex(1,norm*osc_amp*p1*(p->eps(1)));
  vertex(2,norm*osc_amp*p1*(p->eps(2)));

  return ;
}







