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
// Module: EvtDecayParm.cc
//
// Description: Store decay parameters for one decay.
//
// Modification history:
//
//    RYD     April 5, 1997         Module created
//
//------------------------------------------------------------------------
//
#include "EvtGenBase/EvtPatches.hh"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <ctype.h>
#include "EvtGenBase/EvtParticleDecayList.hh"
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtRandom.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtStatus.hh"

EvtParticleDecayList::EvtParticleDecayList(const EvtParticleDecayList &o) {
  _nmode=o._nmode;
  _rawbrfrsum=o._rawbrfrsum;
  _decaylist=new EvtParticleDecayPtr[_nmode];

  int i;
  for(i=0;i<_nmode;i++){
    _decaylist[i]=new EvtParticleDecay;  

    EvtDecayBase *tModel=o._decaylist[i]->getDecayModel();
    
    EvtDecayBase *tModelNew=tModel->clone();
    if (tModel->getPHOTOS()){
      tModelNew->setPHOTOS();
    }
    if (tModel->verbose()){
      tModelNew->setVerbose();
    }
    if (tModel->summary()){
      tModelNew->setSummary();
    }
    std::vector<std::string> args;
    int j;
    for(j=0;j<tModel->getNArg();j++){
      args.push_back(tModel->getArgStr(j));
    }
    tModelNew->saveDecayInfo(tModel->getParentId(),tModel->getNDaug(),
			     tModel->getDaugs(),
			     tModel->getNArg(),
			     args,
			     tModel->getModelName(),
			     tModel->getBranchingFraction());
    _decaylist[i]->setDecayModel(tModelNew);
    
    //_decaylist[i]->setDecayModel(tModel);
    _decaylist[i]->setBrfrSum(o._decaylist[i]->getBrfrSum());
    _decaylist[i]->setMassMin(o._decaylist[i]->getMassMin());
  }


}


EvtParticleDecayList::~EvtParticleDecayList(){

  int i;
  for(i=0;i<_nmode;i++){
    delete _decaylist[i];
  }
  
  if (_decaylist!=0) delete [] _decaylist;
}

void EvtParticleDecayList::printSummary(){

  int i;
  for(i=0;i<_nmode;i++){
    _decaylist[i]->printSummary();
  }
  
}

void EvtParticleDecayList::removeDecay(){
  
  int i;
  for(i=0;i<_nmode;i++){
    delete _decaylist[i];
  }
  
  delete [] _decaylist; 
  _decaylist=0; 
  _nmode=0; 
  _rawbrfrsum=0.0;
  
}


EvtDecayBase* EvtParticleDecayList::getDecayModel(EvtParticle *p){

  if (p->getNDaug()!=0) {
    assert(p->getChannel()>=0);
    return getDecay(p->getChannel()).getDecayModel();
  }
  if (p->getChannel() >(-1) ) {
    return getDecay(p->getChannel()).getDecayModel();
  }


  if (getNMode()==0 ) {
    return 0;
  }
  if (getRawBrfrSum()<0.00000001 ) {
    return 0;
  }

  if (getNMode()==1) {
    p->setChannel(0);
    return getDecay(0).getDecayModel();
  }
  
  if (p->getChannel() >(-1)) {
    report(ERROR,"EvtGen") << "Internal error!!!"<<std::endl;
    ::abort();
  }

  int j;

  for (j=0;j<1000;j++){

    double u=EvtRandom::Flat();

    int i;
    bool breakL=false;
    for (i=0;i<getNMode();i++) {

      if ( breakL ) continue;
      if (u<getDecay(i).getBrfrSum()) {
	breakL=true;
	//special case for decay of on particel to another
	// e.g. K0->K0S

	if (getDecay(i).getDecayModel()->getNDaug()==1 ) {
	  p->setChannel(i);
	  return getDecay(i).getDecayModel(); 
	} 

	if ( p->hasValidP4() ) {
	  //report(INFO,"EvtGen") << "amazing " << EvtPDL::name(p->getId()) 
    // << " " << getDecay(i).getMassMin() << " "<<p->mass() << " " << i 
    // << std::endl;
	  if (getDecay(i).getMassMin() < p->mass() ) {
	    p->setChannel(i);
	    return getDecay(i).getDecayModel(); 
	  }
	}
	else{
	//Lange apr29-2002 - dont know the mass yet
	    p->setChannel(i);
	    return getDecay(i).getDecayModel(); 
	}
      }
    }
  }

  //Ok, we tried 1000 times above to pick a decay channel that is
  //kinematically allowed! Now we give up and search all channels!
  //if that fails, the particle will not be decayed!
  
  int i;

  for (i=0;i<getNMode();i++) {

    if ( getDecay(i).getMassMin() < p->mass() ) {
      return getDecay(i).getDecayModel(); 
    }
  }

  report(ERROR,"EvtGen") << "Could not decay:"
			 <<EvtPDL::name(p->getId()).c_str()
			 <<" with mass:"<<p->mass()
			 <<" will throw event away! "<<std::endl;
  
  //  report(ERROR,"EvtGen") << "Will terminate execution."<<std::endl;

  //  ::abort();  
  EvtStatus::setRejectFlag();
  return 0;

}


void EvtParticleDecayList::setNMode(int nmode){

  EvtParticleDecayPtr* _decaylist_new= new EvtParticleDecayPtr[nmode];
  //  int i;
  if (_nmode!=0){
    report(ERROR,"EvtGen") << "Error _nmode not equal to zero!!!"<<std::endl;
    ::abort();
  }
  if ( _decaylist != 0 ) delete [] _decaylist ;
  _decaylist=_decaylist_new;
  _nmode=nmode;

}


EvtParticleDecay& EvtParticleDecayList::getDecay(int nchannel) {
  if (nchannel>=_nmode) {
    report(ERROR,"EvtGen") <<"Error getting channel:"
			   <<nchannel<<" with only "<<_nmode
			   <<" stored!"<<std::endl;
    ::abort();
  }
  return *(_decaylist[nchannel]);
}

void EvtParticleDecayList::makeChargeConj(EvtParticleDecayList* conjDecayList){

  _rawbrfrsum=conjDecayList->_rawbrfrsum;

  setNMode(conjDecayList->_nmode);
  
  int i;

  for(i=0;i<_nmode;i++){
    _decaylist[i]=new EvtParticleDecay;
    _decaylist[i]->chargeConj(conjDecayList->_decaylist[i]);
  }

}

void EvtParticleDecayList::addMode(EvtDecayBase* decay, double brfrsum,
				   double massmin){

  EvtParticleDecayPtr* newlist=new EvtParticleDecayPtr[_nmode+1];

  int i;
  for(i=0;i<_nmode;i++){
    newlist[i]=_decaylist[i];
  }

  _rawbrfrsum=brfrsum;

  newlist[_nmode]=new EvtParticleDecay;  

  newlist[_nmode]->setDecayModel(decay);
  newlist[_nmode]->setBrfrSum(brfrsum);
  newlist[_nmode]->setMassMin(massmin);

  if (_nmode!=0){
    delete [] _decaylist;
  }

  if ( ( _nmode == 0 ) && ( _decaylist != 0 ) ) delete [] _decaylist ;

  _nmode++;

  _decaylist=newlist;

}


void EvtParticleDecayList::finalize(){

  if (_nmode>0) {
    if ( _rawbrfrsum< 0.000001 ) {
      report(ERROR,"EvtGen") << "Please give me a "
			     <<    "branching fraction sum greater than 0\n";
      assert(0);
    }
    if (fabs(_rawbrfrsum-1.0)>0.0001) {
      report(INFO,"EvtGen") <<"Warning, sum of branching fractions for "
                            <<EvtPDL::name
        (_decaylist[0]->getDecayModel()->getParentId()).c_str()
      			    <<" is "<<_rawbrfrsum<<std::endl;
      report(INFO,"EvtGen") << "rescaled to one! "<<std::endl;
      
    }

    int i;

    for(i=0;i<_nmode;i++){
      double brfrsum=_decaylist[i]->getBrfrSum()/_rawbrfrsum;
      _decaylist[i]->setBrfrSum(brfrsum);      
    }

  }

}

bool EvtParticleDecayList::isJetSet() 
{
  int i ;
  EvtDecayBase * decayer ;
  
  for ( i = 0 ;
        i < getNMode() ;
        i++ ) {
    decayer = getDecay( i ).getDecayModel ( ) ;
    if ( decayer -> getModelName() == "PYTHIA" ) return true ;
  }
  
  return false ;
}

