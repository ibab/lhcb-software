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
// Module: EvtPDL.cc
//
// Description: routines to store particle properties in EvtPDL structure.
//
// Modification history:
//
//    DJL/RYD     September 25, 1996         Module created
//
//------------------------------------------------------------------------
// 
#include <iostream>
#include <fstream.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtPartProp.hh"
#include "EvtGen/EvtParticleNum.hh"
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtReport.hh"

static int first=1;

int EvtPDL::_lastAlias;
int EvtPDL::_nentries;

EvtPartProp *EvtPDL::_partlist;
EvtStringHash<EvtPartProp>* EvtPDL::_particleNameHash;

EvtPDL::EvtPDL() {

  if (first!=0) { 
    first=0;
    _nentries=0;
    _lastAlias=MAX_PART;
    _partlist = new EvtPartProp[MAX_PART];
    _particleNameHash = new EvtStringHash<EvtPartProp>(1023);
  }

}


EvtPDL::~EvtPDL(){
  
  //FIXME!!!!
  //this is an uggly hack as these variables are static!

  delete [] _partlist;
  _partlist=0;
  delete _particleNameHash;
  _particleNameHash=0;

}

void EvtPDL::readPDT(const EvtString fname){


  ifstream indec;
  
  indec.open(fname.value());

  char cmnd[100];
  char xxxx[100];

  char pname[100];
  int  stdhepid;
  double mass;
  double pwidth;
  double pmaxwidth;
  int    chg3;  
  int    spin2;
  double ctau;
  int    lundkc;
  EvtId i;

  if (!indec) {
    report(ERROR,"EvtGen") << "Could not open:"<<fname<<"EvtPDL"<<endl;
    return;
  }

  do{

    char ch,ch1;

    do{

      indec.get(ch);  
      if (ch=='\n') indec.get(ch);
      if (ch!='*') {
	indec.putback(ch);
      }
      else{
	while (indec.get(ch1),ch1!='\n');
      }
    } while(ch=='*');

    indec >> cmnd;

    if (strcmp(cmnd,"end")){

      if (!strcmp(cmnd,"add")){

        indec >> xxxx;
        indec >> xxxx;
        indec >> pname;
        indec >> stdhepid;
        indec >> mass;
        indec >> pwidth;
        indec >> pmaxwidth;
        indec >> chg3;  
        indec >> spin2;
        indec >> ctau;
        indec >> lundkc;


        i=EvtId(_nentries,_nentries);
	
	_nentries++;

        if (i.getId()<MAX_PART && i.getId()>=0 ) {



          _partlist[i.getId()].setSpinType(EvtSpinType::SCALAR);


          if (spin2==0) _partlist[i.getId()].setSpinType(EvtSpinType::SCALAR);
          if (spin2==1) _partlist[i.getId()].setSpinType(EvtSpinType::DIRAC);
          if (spin2==2) _partlist[i.getId()].setSpinType(EvtSpinType::VECTOR);
          if (spin2==3) _partlist[i.getId()].setSpinType(EvtSpinType::RARITASCHWINGER);
          if (spin2==4) _partlist[i.getId()].setSpinType(EvtSpinType::TENSOR);
          if (spin2==5) _partlist[i.getId()].setSpinType(EvtSpinType::SPIN5HALF);
          if (spin2==6) _partlist[i.getId()].setSpinType(EvtSpinType::SPIN3);
          if (spin2==7) _partlist[i.getId()].setSpinType(EvtSpinType::SPIN7HALF);
          if (spin2==8) _partlist[i.getId()].setSpinType(EvtSpinType::SPIN4);
          if (spin2==2 && mass < 0.0001 ) _partlist[i.getId()].setSpinType(EvtSpinType::PHOTON);
          if (spin2==1 && mass < 0.0001 ) _partlist[i.getId()].setSpinType(EvtSpinType::NEUTRINO);


	  if (!strcmp(pname,"string")){
	    _partlist[i.getId()].setSpinType(EvtSpinType::STRING);
	  }

	  if (!strcmp(pname,"vpho")){
	    _partlist[i.getId()].setSpinType(EvtSpinType::VECTOR);
	  }


	  _partlist[i.getId()].setId(i);
	  _partlist[i.getId()].setIdChgConj(EvtId(-1,-1));
	  _partlist[i.getId()].setStdHep(stdhepid);
	  _partlist[i.getId()].setLundKC(lundkc);
	  _partlist[i.getId()].setName(pname);
	  _particleNameHash->add(EvtString(pname),&_partlist[i.getId()]);
	  _partlist[i.getId()].setctau(ctau);
	  _partlist[i.getId()].setChg3(chg3);
	  //report(INFO,"EvtGen") << "particle,chg3:"<<i<<","<<chg3<<endl;

	  //certain lund ids - treat specially...
	  double mpi=0.139570;
	  double mk=0.49368;
	  double mrho=0.770;
	  switch (stdhepid) {
	    //back out this change for now..
	    //case 113: {_partlist[i.getId()].initLineShape(mass,pwidth,pmaxwidth,mpi,mpi,1); break;}
	    //case 213: {_partlist[i.getId()].initLineShape(mass,pwidth,pmaxwidth,mpi,mpi,1); break;}
	    //case -213:{_partlist[i.getId()].initLineShape(mass,pwidth,pmaxwidth,mpi,mpi,1); break;}
	    //case 313: {_partlist[i.getId()].initLineShape(mass,pwidth,pmaxwidth,mpi,mk,1); break;}
	    //case -313:{_partlist[i.getId()].initLineShape(mass,pwidth,pmaxwidth,mpi,mk,1); break;}
	    //case 323: {_partlist[i.getId()].initLineShape(mass,pwidth,pmaxwidth,mpi,mk,1); break;}
	    //case -323:{_partlist[i.getId()].initLineShape(mass,pwidth,pmaxwidth,mpi,mk,1); break;}
	    //case 20113:{_partlist[i.getId()].initLineShape(mass,pwidth,pmaxwidth,mrho,mpi,1); break;}
	    //case 20213:{_partlist[i.getId()].initLineShape(mass,pwidth,pmaxwidth,mrho,mpi,1); break;}
	    //case -20213:{_partlist[i.getId()].initLineShape(mass,pwidth,pmaxwidth,mrho,mpi,1); break;}
	  default:  {_partlist[i.getId()].initLineShape(mass,pwidth,pmaxwidth); break;}
	  }

	}
	else{
	  report(ERROR,"EvtGen") << pname << " is not stored!!!"<<endl;
	}

      }

      // if find a set read information and discard it

      if (!strcmp(cmnd,"set")){

        indec >> xxxx;
        indec >> xxxx;
        indec >> xxxx;
        indec >> xxxx;
      }

    }

  }while(strcmp(cmnd,"end"));
  
  setUpConstsPdt();

}


void EvtPDL::aliasChgConj(EvtId a,EvtId abar){

  if (EvtPDL::chargeConj(EvtId(a.getId(),a.getId()))!=
		 EvtId(abar.getId(),abar.getId())) {

    report(ERROR,"EvtGen")<<"Can't charge conjugate the two aliases:"
			  <<EvtPDL::name(a)<<" and "<<EvtPDL::name(abar)<<endl;
      
    ::abort();

  }

  _partlist[a.getAlias()].setIdChgConj(abar);
  _partlist[abar.getAlias()].setIdChgConj(a);

}

EvtId EvtPDL::chargeConj(EvtId id){

  EvtId idchg=_partlist[id.getAlias()].getIdChgConj();

  if (idchg!=EvtId(-1,-1)) return idchg;

  if (id.getId()!=id.getAlias()){
    if (chargeConj(EvtId(id.getId(),id.getId()))==EvtId(id.getId(),id.getId())){
    
      _partlist[id.getAlias()].setIdChgConj(id);
      return id;
    }
  }

  if (id.getAlias()!=id.getId()) {

    report(ERROR,"EvtGen")<<"Trying to charge conjugate alias particle:"
			  <<name(id)<<" without defining the alias!"<<endl;
      
    ::abort();

  }

  int i;

  for (i=0;i<MAX_PART;i++){
    if (_partlist[i].getStdHep()==-_partlist[id.getId()].getStdHep()){
      _partlist[id.getId()].setIdChgConj(_partlist[i].getId());
      return _partlist[i].getId();
    }
  }
  
  _partlist[id.getId()].setIdChgConj(id);
  return id;
  
}

EvtId EvtPDL::evtIdFromStdHep(int stdhep){

  int i;

  for (i=0;i<MAX_PART;i++){
    if (_partlist[i].getStdHep()==stdhep)
      return _partlist[i].getId();
  }
  
  return EvtId(-1,-1);
  
}



void EvtPDL::alias(EvtId num,const EvtString& newname){

  int i;
  
  for(i=MAX_PART-1;i>=_lastAlias;i--){
    if (newname==_partlist[i].getName()){
      report(WARNING,"EvtGen")<<"Redefining alias:"<<newname<<" will be ignored!"<<endl;
      return;
    }
   }




  _lastAlias--;

  if (_lastAlias<=_nentries) {
      report(WARNING,"EvtGen")<<"You have hit the limit for how many"
			      <<" aliases can be created"<<endl
			      <<"This space is currently not dynamically "
			      <<" allocated."<<endl
			      <<"Will terminate execution."<<endl;
      ::abort();
  }

  _partlist[_lastAlias]=_partlist[num.getId()];
  _partlist[_lastAlias].setName(newname);
  _particleNameHash->add(EvtString(newname),&_partlist[_lastAlias]);
  _partlist[_lastAlias].setId(EvtId(num.getId(),_lastAlias));

}

EvtId EvtPDL::getId(const EvtString& name ){

  int i;

  EvtPartProp* partprop=_particleNameHash->get(name);

  if (partprop==0) return EvtId(-1,-1);

  return partprop->getId();
  
}

void EvtPDL::setUpConstsPdt(){

}


