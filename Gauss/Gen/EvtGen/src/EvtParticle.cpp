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
// Module: EvtParticle.cc
//
// Description: Class to describe all particles
//
// Modification history:
//
//    DJL/RYD     September 25, 1996         Module created
//
//------------------------------------------------------------------------
// 
#include <iostream>
#include <math.h>
#include <fstream.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#ifdef WIN32
#include <strstrea.h>
#else
#include <strstream.h>
#endif
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtParticleNum.hh"
#include "EvtGen/EvtRandom.hh"
#include "EvtGen/EvtPHOTOS.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtDecayTable.hh"
#include "EvtGen/EvtDiracParticle.hh"
#include "EvtGen/EvtScalarParticle.hh"
#include "EvtGen/EvtVectorParticle.hh"
#include "EvtGen/EvtTensorParticle.hh"
#include "EvtGen/EvtPhotonParticle.hh"
#include "EvtGen/EvtNeutrinoParticle.hh"
#include "EvtGen/EvtStringParticle.hh"
#include "EvtGen/EvtStdHep.hh"
#include "EvtGen/EvtSecondary.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtGenKine.hh"
#include "EvtGen/EvtCPUtil.hh"
#include "EvtGen/EvtParticleFactory.hh"


EvtParticle::~EvtParticle() {}

EvtParticle::EvtParticle() {
   _ndaug=0;
   _parent=0;
   _channel=-10;
   _t=0.0;
   _genlifetime=1;
   _first=1;
}

void EvtParticle::setFirstOrNot() {
  _first=0;
}
void EvtParticle::resetFirstOrNot() {
  _first=1;
}

void EvtParticle::setChannel( int i ) { _channel=i;}

EvtParticle *EvtParticle::getDaug(int i) { return _daug[i]; }

EvtParticle *EvtParticle::getParent() { return _parent;}

void EvtParticle::setLifetime(double tau){
  _t=tau;
}

void EvtParticle::setLifetime(){
  if (_genlifetime){
    _t=-log(EvtRandom::Flat())*EvtPDL::getctau(getId());
  }
}

double EvtParticle::getLifetime(){

  return _t;
}

void EvtParticle::addDaug(EvtParticle *node) {
  node->_daug[node->_ndaug++]=this;
  _ndaug=0;
  _parent=node; 
}


int EvtParticle::firstornot() const { return _first;}

EvtId EvtParticle::getId() const { return _id;}

EvtSpinType::spintype EvtParticle::getSpinType() const 
      { return EvtPDL::getSpinType(_id);}

int EvtParticle::getSpinStates() const 
  { return EvtSpinType::getSpinStates(EvtPDL::getSpinType(_id));}

const EvtVector4R& EvtParticle::getP4() const { return _p;}

int EvtParticle::getChannel() const { return _channel;}

int EvtParticle::getNDaug() const { return _ndaug;}

double EvtParticle::mass() const {

     return _p.mass();
}


void EvtParticle::setDiagonalSpinDensity(){

  _rhoForward.SetDiag(getSpinStates());
}

void EvtParticle::setVectorSpinDensity(){

  if (getSpinStates()!=3) {
    report(ERROR,"EvtGen")<<"Error in EvtParticle::setVectorSpinDensity"<<endl;
    report(ERROR,"EvtGen")<<"spin_states:"<<getSpinStates()<<endl;
    report(ERROR,"EvtGen")<<"particle:"<<EvtPDL::name(_id)<<endl;
    ::abort();
  }

  EvtSpinDensity rho;

  //Set helicity +1 and -1 to 1.
  rho.SetDiag(getSpinStates());
  rho.Set(1,1,EvtComplex(0.0,0.0));

  setSpinDensityForwardHelicityBasis(rho);

}


void EvtParticle::setSpinDensityForwardHelicityBasis(const EvtSpinDensity& rho){

  EvtSpinDensity R=rotateToHelicityBasis();

  assert(R.GetDim()==rho.GetDim());

  int n=rho.GetDim();

  _rhoForward.SetDim(n);

  int i,j,k,l;

  for(i=0;i<n;i++){
    for(j=0;j<n;j++){
      EvtComplex tmp=0.0;
      for(k=0;k<n;k++){
	for(l=0;l<n;l++){
	  tmp+=R.Get(l,i)*rho.Get(l,k)*conj(R.Get(k,j));
	}
      }
      _rhoForward.Set(i,j,tmp);
    }
  }

  //cout << "_rhoForward:"<<_rhoForward<<endl;

}

void EvtParticle::setSpinDensityForwardHelicityBasis(const EvtSpinDensity& rho,
						     double alpha,
						     double beta,
						     double gamma){

  EvtSpinDensity R=rotateToHelicityBasis(alpha,beta,gamma);

  assert(R.GetDim()==rho.GetDim());

  int n=rho.GetDim();

  _rhoForward.SetDim(n);

  int i,j,k,l;

  for(i=0;i<n;i++){
    for(j=0;j<n;j++){
      EvtComplex tmp=0.0;
      for(k=0;k<n;k++){
	for(l=0;l<n;l++){
	  tmp+=R.Get(l,i)*rho.Get(l,k)*conj(R.Get(k,j));
	}
      }
      _rhoForward.Set(i,j,tmp);
    }
  }

  //cout << "_rhoForward:"<<_rhoForward<<endl;

}


void EvtParticle::decay(){

  //P is particle to decay, typically 'this' but sometime
  //modified by mixing 
  EvtParticle* p=this;


  //Will include effects of mixing here
  //added by Lange Jan4,2000
  static EvtId BS0=EvtPDL::getId("B_s0");
  static EvtId BSB=EvtPDL::getId("anti-B_s0");

  if (getId()==BS0||getId()==BSB){
    double t;
    int mix;
    EvtCPUtil::incoherentMix(getId(), t, mix);
    setLifetime(t);
    
    if (mix) {

      EvtScalarParticle* scalar_part;
    
      scalar_part=new EvtScalarParticle;
      if (getId()==BS0) {
	EvtVector4R p_init(EvtPDL::getMass(BSB),0.0,0.0,0.0);
	scalar_part->init(BSB,p_init);
      }
      else{
	EvtVector4R p_init(EvtPDL::getMass(BS0),0.0,0.0,0.0);
	scalar_part->init(BS0,p_init);
      }

      scalar_part->setLifetime(0);

      scalar_part->setDiagonalSpinDensity();      
    
      insertDaugPtr(0,scalar_part);

      _ndaug=1;

      p=scalar_part;

    }


  }


  EvtDecayBase *decayer;
  decayer = EvtDecayTable::GetDecayFunc(p);

  if (p->_ndaug>0) {
    report(INFO,"EvtGen") <<"Is decaying particle with daughters!!!!!"<<endl;
    ::abort();
    return;
  }


  if ( decayer==0) {
    p->_rhoBackward.SetDiag(p->getSpinStates());
    return;    
  }

  decayer->makeDecay(p);

}


void EvtParticle::deleteTree(){
  int i;

  for(i=0;i<_ndaug;i++){
    _daug[i]->deleteTree();
  }
  
  delete this;
  
}

EvtVector4C EvtParticle::epsParent(int i) const {
  EvtVector4C temp;
  printParticle();
  report(ERROR,"EvtGen") << "and you have asked for the:"<<i
			 <<"th polarization vector."
			 <<" I.e. you thought it was a"
			 <<" vector particle!" << endl;
  ::abort();
  return temp;
}

EvtVector4C EvtParticle::eps(int i) const {
  EvtVector4C temp;
  printParticle();
  report(ERROR,"EvtGen") << "and you have asked for the:"<<i
			 <<"th polarization vector."
			 <<" I.e. you thought it was a"
			 <<" vector particle!" << endl;
  ::abort();
  return temp;
}

EvtVector4C EvtParticle::epsParentPhoton(int i){
  EvtVector4C temp;
  printParticle();
  report(ERROR,"EvtGen") << "and you have asked for the:"<<i
			 <<"th polarization vector of photon."
			 <<" I.e. you thought it was a"
			 <<" photon particle!" << endl;
  ::abort();
  return temp;
}

EvtVector4C EvtParticle::epsPhoton(int i){
  EvtVector4C temp;
  printParticle();
  report(ERROR,"EvtGen") << "and you have asked for the:"<<i
			 <<"th polarization vector of a photon."
			 <<" I.e. you thought it was a"
			 <<" photon particle!" << endl;
  ::abort();
  return temp;
}

EvtDiracSpinor EvtParticle::spParent(int i) const {
  EvtDiracSpinor tempD;
  int temp;
  temp = i;
  printParticle();
  report(ERROR,"EvtGen") << "and you have asked for the:"<<i
			 <<"th dirac spinor."
			 <<" I.e. you thought it was a"
			 <<" Dirac particle!" << endl;
  ::abort();
  return tempD;
}

EvtDiracSpinor EvtParticle::sp(int i) const {
  EvtDiracSpinor tempD;
  int temp;
  temp = i;
  printParticle();
  report(ERROR,"EvtGen") << "and you have asked for the:"<<i
			 <<"th dirac spinor."
			 <<" I.e. you thought it was a"
			 <<" Dirac particle!" << endl;
  ::abort();
  return tempD;
}

EvtDiracSpinor EvtParticle::spParentNeutrino() const {
  EvtDiracSpinor tempD;
  printParticle();
  report(ERROR,"EvtGen") << "and you have asked for the "
			 <<"dirac spinor."
			 <<" I.e. you thought it was a"
			 <<" neutrino particle!" << endl;
  ::abort();
  return tempD;
}

EvtDiracSpinor EvtParticle::spNeutrino() const {
  EvtDiracSpinor tempD;
  printParticle();
  report(ERROR,"EvtGen") << "and you have asked for the "
			 <<"dirac spinor."
			 <<" I.e. you thought it was a"
			 <<" neutrino particle!" << endl;
  ::abort();
  return tempD;
}

EvtTensor4C EvtParticle::epsTensorParent(int i) const {
  int temp;
  temp = i;
  EvtTensor4C tempC; 
  printParticle();
  report(ERROR,"EvtGen") << "and you have asked for the:"<<i
			 <<"th tensor."
			 <<" I.e. you thought it was a"
			 <<" Tensor particle!" << endl;
  ::abort();
  return tempC;
}

EvtTensor4C EvtParticle::epsTensor(int i) const {
  int temp;
  temp = i;
  EvtTensor4C tempC; 
  printParticle();
  report(ERROR,"EvtGen") << "and you have asked for the:"<<i
			 <<"th tensor."
			 <<" I.e. you thought it was a"
			 <<" Tensor particle!" << endl;
  ::abort();
  return tempC;
}

EvtVector4R EvtParticle::getP4Lab() {
  EvtVector4R temp,mom;
  EvtParticle *ptemp;
  
  temp=this->getP4();
  ptemp=this;
  
  while (ptemp->getParent()!=0) {
    ptemp=ptemp->getParent();
    mom=ptemp->getP4();
    temp=boostTo(temp,mom);   
  } 
  return temp;
}

EvtVector4R EvtParticle::getP4Restframe() {

  return EvtVector4R(mass(),0.0,0.0,0.0);

}

EvtVector4R EvtParticle::get4Pos() {

  EvtVector4R temp,mom;
  EvtParticle *ptemp;
  
  temp.set(0.0,0.0,0.0,0.0);
  ptemp=getParent();

  if (ptemp==0) return temp;

  temp=(ptemp->_t/ptemp->mass())*(ptemp->getP4());

  while (ptemp->getParent()!=0) {
    ptemp=ptemp->getParent();
    mom=ptemp->getP4();
    temp=boostTo(temp,mom);
    temp=temp+(ptemp->_t/ptemp->mass())*(ptemp->getP4());
  } 
  
  return temp;
}


EvtParticle * EvtParticle::nextIter(EvtParticle *rootOfTree) {

  EvtParticle *bpart;
  EvtParticle *current;

  current=this;
  int i;

  if (_ndaug!=0) return _daug[0];

  do{
    bpart=current->_parent;
    if (bpart==0) return 0;
    i=0;
    while (bpart->_daug[i]!=current) {i++;}

    if ( bpart==rootOfTree ) {
      if ( i== bpart->_ndaug-1 ) return 0;
    }

    i++;
    current=bpart;

  }while(i>=bpart->_ndaug);

  return bpart->_daug[i];

}


void EvtParticle::makeStdHep(EvtStdHep& stdhep,EvtSecondary& secondary,
			     EvtId *list_of_stable){

  //first add particle to the stdhep list;
  stdhep.createParticle(getP4Lab(),get4Pos(),-1,-1,
			EvtPDL::getStdHep(getId()));

  int ii=0;

  //lets see if this is a longlived particle and terminate the 
  //list building!
  
  while (list_of_stable[ii]!=EvtId(-1,-1)) {
    if (getId()==list_of_stable[ii]){
      secondary.createSecondary(0,this);
      return;
    }
    ii++;
  }




  int i;
  for(i=0;i<_ndaug;i++){
    stdhep.createParticle(_daug[i]->getP4Lab(),_daug[i]->get4Pos(),0,0,
			  EvtPDL::getStdHep(_daug[i]->getId()));
  }

  for(i=0;i<_ndaug;i++){
    _daug[i]->makeStdHepRec(1+i,1+i,stdhep,secondary,list_of_stable);
  }
  return;

}

void EvtParticle::makeStdHep(EvtStdHep& stdhep){

  //first add particle to the stdhep list;
  stdhep.createParticle(getP4Lab(),get4Pos(),-1,-1,
			EvtPDL::getStdHep(getId()));

  int i;
  for(i=0;i<_ndaug;i++){
    stdhep.createParticle(_daug[i]->getP4Lab(),_daug[i]->get4Pos(),0,0,
			  EvtPDL::getStdHep(_daug[i]->getId()));
  }

  for(i=0;i<_ndaug;i++){
    _daug[i]->makeStdHepRec(1+i,1+i,stdhep);
  }
  return;

}


void EvtParticle::makeStdHepRec(int firstparent,int lastparent,
				EvtStdHep& stdhep,
				EvtSecondary& secondary,
				EvtId *list_of_stable){


  int ii=0;

  //lets see if this is a longlived particle and terminate the 
  //list building!
  
  while (list_of_stable[ii]!=EvtId(-1,-1)) {
    if (getId()==list_of_stable[ii]){
      secondary.createSecondary(firstparent,this);
      return;
    }
    ii++;
  }



  int i;
  int parent_num=stdhep.getNPart();
  for(i=0;i<_ndaug;i++){
    stdhep.createParticle(_daug[i]->getP4Lab(),_daug[i]->get4Pos(),
			  firstparent,lastparent,
			  EvtPDL::getStdHep(_daug[i]->getId()));
  }

  for(i=0;i<_ndaug;i++){
    _daug[i]->makeStdHepRec(parent_num+i,parent_num+i,stdhep,
			   secondary,list_of_stable);
  }
  return;

}

void EvtParticle::makeStdHepRec(int firstparent,int lastparent,
				EvtStdHep& stdhep){

  int i;
  int parent_num=stdhep.getNPart();
  for(i=0;i<_ndaug;i++){
    stdhep.createParticle(_daug[i]->getP4Lab(),_daug[i]->get4Pos(),
			  firstparent,lastparent,
			  EvtPDL::getStdHep(_daug[i]->getId()));
  }

  for(i=0;i<_ndaug;i++){
    _daug[i]->makeStdHepRec(parent_num+i,parent_num+i,stdhep);
  }
  return;

}

void EvtParticle::printTreeRec(int level) const {

  int newlevel,i;
  newlevel = level +1;

  
  if (_ndaug!=0) {
    if ( level > 0 ) {
      for (i=0;i<(5*level);i++) {
	report(INFO,"") <<" ";
      }
    }
    report(INFO,"") << EvtPDL::name(_id);  
    report(INFO,"") << " -> ";
    for(i=0;i<_ndaug;i++){
      report(INFO,"") << EvtPDL::name(_daug[i]->getId())<<" ";
    }
    report(INFO,"")<<endl;
    for(i=0;i<_ndaug;i++){
      _daug[i]->printTreeRec(newlevel);
    }
  }
}

void EvtParticle::printTree() const {
  
  report(INFO,"EvtGen") << "This is the current decay chain"<<endl;
  report(INFO,"") << "This top particle is "<<
    EvtPDL::name(_id)<<endl;  

  this->printTreeRec(0);
  report(INFO,"EvtGen") << "End of decay chain."<<endl;

}

void EvtParticle::printParticle() const {

  switch (EvtPDL::getSpinType(_id)){ 
  case EvtSpinType::SCALAR:
    report(INFO,"EvtGen") << "This is a scalar particle:"<<EvtPDL::name(_id)<<"\n";
    break;     
  case EvtSpinType::VECTOR:
    report(INFO,"EvtGen") << "This is a vector particle:"<<EvtPDL::name(_id)<<"\n";
    break;     
  case EvtSpinType::TENSOR:
    report(INFO,"EvtGen") << "This is a tensor particle:"<<EvtPDL::name(_id)<<"\n";
    break;
  case EvtSpinType::DIRAC:
    report(INFO,"EvtGen") << "This is a dirac particle:"<<EvtPDL::name(_id)<<"\n";
    break;
  case EvtSpinType::PHOTON:
    report(INFO,"EvtGen") << "This is a photon:"<<EvtPDL::name(_id)<<"\n";
    break;
  case EvtSpinType::NEUTRINO:
    report(INFO,"EvtGen") << "This is a neutrino:"<<EvtPDL::name(_id)<<"\n";
    break;
  case EvtSpinType::STRING:
    report(INFO,"EvtGen") << "This is a string:"<<EvtPDL::name(_id)<<"\n";
    break;
  default:
    report(INFO,"EvtGen") <<"Unknown particle type in EvtParticle::printParticle()"<<endl;
    break;
  }
  report(INFO,"EvtGen") << "Number of daughters:"<<_ndaug<<"\n";


}



void init_vector( EvtParticle **part ){
  *part = (EvtParticle *) new EvtVectorParticle;
} 


void init_scalar( EvtParticle **part ){
  *part = (EvtParticle *) new EvtScalarParticle;
} 

void init_tensor( EvtParticle **part ){
  *part = (EvtParticle *) new EvtTensorParticle;
} 

void init_dirac( EvtParticle **part ){
  *part = (EvtParticle *) new EvtDiracParticle;
} 

void init_photon( EvtParticle **part ){
  *part = (EvtParticle *) new EvtPhotonParticle;
} 

void init_neutrino( EvtParticle **part ){
  *part = (EvtParticle *) new EvtNeutrinoParticle;
} 

void init_string( EvtParticle **part ){
  *part = (EvtParticle *) new EvtStringParticle;
} 

void EvtParticle::initializePhaseSpace(
                   int numdaughter,EvtId *daughters) {

  double m_b;
  int i;
  this->makeDaughters(numdaughter,daughters);

  static EvtVector4R p4[100];
  static double mass[100];

  m_b = this->mass();

  EvtDecayBase::findMasses( this, numdaughter, daughters, mass );

  EvtGenKine::PhaseSpace( numdaughter, mass, p4, m_b );

  for(i=0;i<numdaughter;i++){
     this->getDaug(i)->init(daughters[i],p4[i]);
  }


  return;
}

void EvtParticle::makeDaughters( int ndaugstore, EvtId *id){

  int i;

  EvtParticle* pdaug;  
  if (_ndaug!=0 ){
    if (_ndaug!=ndaugstore){
      report(ERROR,"EvtGen") << "Asking to make a different number of "
			     << "daughters than what was previously created."
			     << endl<<"Will terminate."<<endl;
      ::abort();
    }
  } 
  else{
    for(i=0;i<ndaugstore;i++){
      pdaug=EvtParticleFactory::particleFactory(EvtPDL::getSpinType(id[i]));
      pdaug->addDaug(this);	
    }

  } //else
} //makeDaughters

