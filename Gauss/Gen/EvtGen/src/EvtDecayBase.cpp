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
// Module: EvtDecayBase.cc
//
// Description: Store decay parameters for one decay.
//
// Modification history:
//
//    RYD     September 30, 1997         Module created
//
//------------------------------------------------------------------------
//
#include <iostream>
#include <fstream.h>
#include <stdlib.h>
#include <ctype.h>
#include "EvtGen/EvtGen.hh"
#include "EvtGen/EvtDecayBase.hh"
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtSpinType.hh"
void EvtDecayBase::checkQ() {
  int i;
  int q=0;
  int qpar;

  //If there are no daughters (jetset etc) then we do not
  //want to do this test.  Why?  Because sometimes the parent
  //will have a nonzero charge.

  if ( _ndaug != 0) {
    for(i=0; i<_ndaug; i++ ) {
      q += EvtPDL::chg3(_daug[i]);
    }
    qpar = EvtPDL::chg3(_parent);

    if ( q != qpar ) {
      report(ERROR,"EvtGen") <<_modelname<< " generator expected "
			     << " charge to be conserved, found:"<<endl;
      report(ERROR,"EvtGen") << "Parent charge of "<<(qpar/3)<<endl;
      report(ERROR,"EvtGen") << "Sum of daughter charge of "<<(q/3)<<endl;
      report(ERROR,"EvtGen") << "The parent is "<< EvtPDL::name(_parent)<<endl;
      for(i=0; i<_ndaug; i++ ) {
      report(ERROR,"EvtGen") << "Daughter "<< EvtPDL::name(_daug[i])<<endl;
      }
      report(ERROR,"EvtGen") << "Will terminate execution!"<<endl;
      
      ::abort();
    }
  }
}
    

double EvtDecayBase::getProbMax( double prob ) {

  int i;

  //diagnostics
  sum_prob+=prob;
  if (prob>max_prob) max_prob=prob;


  if ( defaultprobmax && ntimes_prob<=500 ) { 
    //We are building up probmax with this iteration
     ntimes_prob += 1;
     if ( prob > probmax ) { probmax = prob;}
     if (ntimes_prob==500) { 
       probmax*=1.2;
     }
     return 1000000.0*prob;
  }

  if ( prob> probmax*1.0001) {

    report(INFO,"EvtGen") << "prob > probmax:("<<prob<<">"<<probmax<<")";
    report(INFO,"") << "("<<_modelname<<") ";
    report(INFO,"") << EvtPDL::name(_parent)<<" -> ";
    for(i=0;i<_ndaug;i++){
       report(INFO,"") << EvtPDL::name(_daug[i]) << " ";
    }
    report(INFO,"") << endl;

    if (defaultprobmax) probmax = prob;

  }

  ntimes_prob += 1;

  return probmax;

} //getProbMax


double EvtDecayBase::resetProbMax(double prob) {
  
  report(INFO,"EvtGen") << "Reseting prob max\n"; 
  report(INFO,"EvtGen") << "prob > probmax:("<<prob<<">"<<probmax<<")";
  report(INFO,"") << "("<<_modelname<<")";
  report(INFO,"") << EvtPDL::getStdHep(_parent)<<"->";
  
  for( int i=0;i<_ndaug;i++){
    report(INFO,"") << EvtPDL::getStdHep(_daug[i]) << " ";
  }
  report(INFO,"") << endl;
  
  probmax = 0.0;
  defaultprobmax = 0;
  ntimes_prob = 0;
  
  return prob;

}


EvtString EvtDecayBase::commandName(){
  return EvtString("");
}
void EvtDecayBase::command(EvtString cmd){
  report(ERROR,"EvtGen") << "Should never call EvtDecayBase::command"<<endl;
  ::abort();
}



void EvtDecayBase::init() {

  //This default version of init does nothing;
  //A specialized version of this function can be
  //supplied for each decay model to do initialization.

  return;

}

void EvtDecayBase::initProbMax() {

  //This function is called if the decay does not have a
  //specialized initialization.  
  //The default is to set the maximum
  //probability to 0 and the number of times called to 0
  //and defaultprobmax to 1 such that the decay will be 
  //generated many many times
  //in order to generate a reasonable maximum probability
  //for the decay.

  defaultprobmax=1;
  ntimes_prob = 0;
  probmax = 0.0;

} //initProbMax


void EvtDecayBase::saveDecayInfo(EvtId ipar, int ndaug, EvtId *daug, 
				 int narg,EvtVectorT<double>& args,
				 EvtString name,
				 double brfr) {

  int i;

  _brfr=brfr;
  _ndaug=ndaug;
  _narg=narg;
  _parent=ipar; 


  _dsum=0;

  if (_ndaug>0) {
    _daug=new EvtId [_ndaug];
    for(i=0;i<_ndaug;i++){
      _daug[i]=daug[i];
      _dsum+=daug[i].getAlias();
    }
  }
  else{
    _daug=0;
  }

  if (_narg>0) {
    _args=new double [narg];
    for(i=0;i<_narg;i++){
      _args[i]=args[i];
    }
  }
  else{
     _args = 0;
  }

  _modelname=name;

  this->init();
  this->initProbMax();

  if (_chkCharge){
    this->checkQ();
  }


  if (defaultprobmax){
    report(INFO,"EvtGen") << "No default probmax for ";
    report(INFO,"") << "("<<_modelname<<") ";
    report(INFO,"") << EvtPDL::name(_parent)<<" -> ";
    for(i=0;i<_ndaug;i++){
      report(INFO,"") << EvtPDL::name(_daug[i]) << " ";
    }
    report(INFO,"") << endl;
    report(INFO,"") << "This is fine for development, but must be provided for production."<<endl;
    report(INFO,"EvtGen") << "Never fear though - the decay will use the \n";
    report(INFO,"EvtGen") << "500 iterations to build up a good probmax \n";
    report(INFO,"EvtGen") << "before accepting a decay. "<<endl;
  }

}

EvtDecayBase::EvtDecayBase() {

  //the default is that the user module does _not_ set
  // any probmax.
  defaultprobmax=1;
  ntimes_prob = 0;
  probmax = 0.0;
  
  _photos=0;
  _verbose=0;
  _summary=0;
  _parent=EvtId(-1,-1);
  _ndaug=0;
  _narg=0;
  _daug=0;
  _args=0;
  _modelname="**********";

  //Default is to check that charge is conserved
  _chkCharge=1;

  //statistics collection!

  max_prob=0.0;
  sum_prob=0.0;

}



void EvtDecayBase::printSummary() {

  int i;

  if (ntimes_prob>0) {

    report(INFO,"EvtGen") << "Calls="<<ntimes_prob<<" eff:"<<
      sum_prob/(probmax*ntimes_prob)<<" frac. max:"<<max_prob/probmax;
    report(INFO,"") <<" probmax:"<<probmax<<" max:"<<max_prob<<" : ";

    report(INFO,"") << EvtPDL::name(_parent)<<" -> ";
    for(i=0;i<_ndaug;i++){
      report(INFO,"") << EvtPDL::name(_daug[i]) << " ";
    }
    report(INFO,"") << " ("<<_modelname<<"):"<< endl;
	

  }
  
}


EvtDecayBase::~EvtDecayBase() {

  if (_daug!=0){
     delete [] _daug;
  }

  if (_args!=0){
     delete [] _args;
  }


}

void EvtDecayBase::setProbMax(double prbmx){

  defaultprobmax=0;
  probmax=prbmx;

}

void EvtDecayBase::noProbMax(){

  defaultprobmax=0;

}



// given list of daughters ( by number ) returns a
// list of viable masses. 



void EvtDecayBase::findMasses(EvtParticle *p, int ndaugs, 
				 EvtId daugs[10], double masses[10]) {

  int i;
  double mass_sum;

  int count=0;

  if (!( p->firstornot() )) {
    for (i = 0; i < ndaugs; i++ ) {
      masses[i] = p->getDaug(i)->mass();
    } //for
  } //if
  else {
    p->setFirstOrNot();
    // if only one daughter do it

    if (ndaugs==1) {
      masses[0]=p->mass();
      return;
    }
    
    //until we get a combo whose masses are less than _parent mass.
    do {
      mass_sum = 0.0;

      for (i = 0; i < ndaugs; i++ ) {
        masses[i] = EvtPDL::getMass(daugs[i]);
        mass_sum = mass_sum + masses[i];
      } 

      count++;

     
      if(count==10000) {
        report(ERROR,"EvtGen") <<"Decaying particle:"<<
	  EvtPDL::name(p->getId())<<" (m="<<p->mass()<<")"<<endl;
        report(ERROR,"EvtGen") <<"To the following daugthers"<<endl;
        for (i = 0; i < ndaugs; i++ ) {
          report(ERROR,"EvtGen") <<  
	    EvtPDL::name(daugs[i]) << endl;
        } 
	report(ERROR,"EvtGen") << "Has been rejected "<<count
			       << " times, will now take minimal masses "
			       << " of daugthers"<<endl;
        
	mass_sum=0.;
	for (i = 0; i < ndaugs; i++ ) {
	  masses[i] = EvtPDL::getMinMass(daugs[i]);
	  mass_sum = mass_sum + masses[i];
	} 
	if (mass_sum > p->mass()){
	  report(ERROR,"EvtGen") << "Parent mass="<<p->mass()
				 << "to light for daugthers."<<endl
				 << "Will throw the event away."<<endl;
	  //dont terminate - start over on the event.
	  EvtGen::setRejectFlag();
	  mass_sum=0.;
	  //	  ::abort();
	}

      }
    } while ( mass_sum > p->mass());
  } //else
  
  return;
}       

void EvtDecayBase::checkNArg(int a1, int a2) {

  if ( _narg != a1 && _narg != a2 ) {
    report(ERROR,"EvtGen") << _modelname << " generator expected ";
    report(ERROR,"EvtGen") << a1; 
    if ( a2>-1) {
      report(ERROR,"EvtGen") << " or " << a2; 
    }
    report(ERROR,"EvtGen") << " arguments but found:"<< _narg << endl;
    report(ERROR,"EvtGen") << "Will terminate execution!"<<endl;
    ::abort();

  } 

}
void EvtDecayBase::checkNDaug(int d1, int d2){

  if ( _ndaug != d1 && _ndaug != d2 ) {
    report(ERROR,"EvtGen") << _modelname << " generator expected ";
    report(ERROR,"EvtGen") << d1; 
    if ( d2>-1) {
      report(ERROR,"EvtGen") << " or " << d2; 
    }
    report(ERROR,"EvtGen") << " daughters but found:"<< _ndaug << endl;
    report(ERROR,"EvtGen") << "Will terminate execution!"<<endl;
    ::abort();
  } 

}

void EvtDecayBase::checkSpinParent(EvtSpinType::spintype sp) {

  EvtSpinType::spintype parenttype = EvtPDL::getSpinType(getParentId());
  if ( parenttype != sp ) {
    report(ERROR,"EvtGen") << _modelname 
			   << " did not get the correct parent spin\n";
    report(ERROR,"EvtGen") << "Will terminate execution!"<<endl;
    ::abort();
  } 

}

void EvtDecayBase::checkSpinDaughter(int d1, EvtSpinType::spintype sp) {

  EvtSpinType::spintype parenttype = EvtPDL::getSpinType(getDaug(d1));
  if ( parenttype != sp ) {
    report(ERROR,"EvtGen") << _modelname 
			   << " did not get the correct daughter spin d=" 
			   << d1 << endl;
    report(ERROR,"EvtGen") << "Will terminate execution!"<<endl;
    ::abort();
  } 

}
