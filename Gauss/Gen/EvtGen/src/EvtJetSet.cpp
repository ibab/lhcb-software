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
// Module: EvtJetSet.cc
//
// Description: Routine to use JetSet for decaying particles.
//
// Modification history:
//
//    RYD     July 24, 1997        Module created
//
//------------------------------------------------------------------------
// 
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtStringParticle.hh"
#include "EvtGen/EvtDecayTable.hh"
#include "EvtGen/EvtParticleDecay.hh"
#include "EvtGen/EvtParticleDecayList.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtJetSet.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtString.hh"
#include "EvtGen/EvtId.hh"
#include <iostream>
#include <iomanip.h>
#include <fstream.h>
#include <string.h>
#include <stdlib.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <stdio.h>


int EvtJetSet::njetsetdecays=0;
  EvtDecayBasePtr* EvtJetSet::jetsetdecays=0; 
int EvtJetSet::ntable=0;

int EvtJetSet::ncommand=0;
int EvtJetSet::lcommand=0;
EvtString* EvtJetSet::commands=0;

#ifdef WIN32
extern "C" {
    extern void __stdcall EVTJETSETINIT(char* fname, int len);
    extern void __stdcall JETSET1(int *,double *,int *,int *,int *,
		       double *,double *,double *,double *);
  extern void __stdcall PYGIVE(char *cnfgstr,int length);
  extern int __stdcall PYCOMP(int* kf);
  extern void __stdcall EVTJETSETPART(int*, double*, double*, double*, double*);
}
#else
extern "C" {
    extern void evtjetsetinit_(char* fname, int len);
    extern void jetset1_(int *,double *,int *,int *,int *,
		       double *,double *,double *,double *);
  extern void pygive_(char *cnfgstr,int length);
  extern int pycomp_(int* kf);
  extern void evtjetsetpart_(int*, double*, double*, double*, double*);
}

#endif

EvtJetSet::EvtJetSet(){}

EvtJetSet::~EvtJetSet(){


  int i;


  //the deletion of commands is really uggly!

  if (njetsetdecays==0) {
    delete [] commands;
    commands=0;
    return;
  }

  for(i=0;i<njetsetdecays;i++){
    if (jetsetdecays[i]==this){
      jetsetdecays[i]=jetsetdecays[njetsetdecays-1];
      njetsetdecays--;
      if (njetsetdecays==0) {
	delete [] commands;
	commands=0;
      }
      return;
    }
  }

  report(ERROR,"EvtGen") << "Error in destroying JetSet model!"<<std::endl;
 
}


void EvtJetSet::getName(EvtString& model_name){

  model_name="JETSET";     

}

EvtDecayBase* EvtJetSet::clone(){

  return new EvtJetSet;

}


void EvtJetSet::initProbMax(){

  noProbMax();

}


void EvtJetSet::init(){

  checkNArg(1);


  if (getParentId().isAlias()){

    report(ERROR,"EvtGen") << "EvtJetSet finds that you are decaying the"<<std::endl
                           << " aliased particle "
			   << EvtPDL::name(getParentId())
			   << " with the JetSet model"<<std::endl
			   << " this does not work, please modify decay table."
			   << std::endl;
    report(ERROR,"EvtGen") << "Will terminate execution!"<<std::endl;
    ::abort();

  }

  store(this);

}


EvtString EvtJetSet::commandName(){

  return EvtString("JetSetPar");
  
}


void EvtJetSet::command(EvtString cmd){

  if (ncommand==lcommand){

    lcommand=10+2*lcommand;

    EvtString* newcommands=new EvtString[lcommand];
    
    int i;

    for(i=0;i<ncommand;i++){
      newcommands[i]=commands[i];
    }
    
    delete [] commands;

    commands=newcommands;

  }

  commands[ncommand]=cmd;

  ncommand++;

}



void EvtJetSet::decay( EvtParticle *p){


  //added by Lange Jan4,2000
  static EvtId STRNG=EvtPDL::getId("string");

  int istdheppar=EvtPDL::getStdHep(p->getId());

#ifdef WIN32
  if (PYCOMP(&istdheppar)==0){
#else
  if (pycomp_(&istdheppar)==0){
#endif
    report(ERROR,"EvtGen") << "Jetset can not decay:"
      <<EvtPDL::name(p->getId())<<std::endl;
    return;
  }

  double mp=p->mass();

  EvtVector4R p4[20];
  
  int i,more;
  int ip=EvtPDL::getStdHep(p->getId());
  int ndaugjs;
  int kf[100];
  EvtId evtnumstable[100],evtnumparton[100];
  int stableindex[100],partonindex[100];
  int numstable;
  int numparton;
  int km[100];
  EvtId type[MAX_DAUG];

  jetSetInit();

  double px[100],py[100],pz[100],e[100];

  if ( p->getNDaug() != 0 ) { return;}

  int count=0;

  do{

#ifdef WIN32
    JETSET1(&ip,&mp,&ndaugjs,kf,km,px,py,pz,e);
#else
    jetset1_(&ip,&mp,&ndaugjs,kf,km,px,py,pz,e);
#endif


    numstable=0;
    numparton=0;

    for(i=0;i<ndaugjs;i++){

      if (EvtPDL::evtIdFromStdHep(kf[i])==EvtId(-1,-1)) {
	report(ERROR,"EvtGen") << "JetSet returned particle:"<<kf[i]<<std::endl;
	report(ERROR,"EvtGen") << "This can not be translated to evt number"<<std::endl;
	report(ERROR,"EvtGen") << "and the decay will be rejected!"<<std::endl;
	report(ERROR,"EvtGen") << "The decay was of particle:"<<ip<<std::endl;

      }

      //sort out the partons
      if (abs(kf[i])<=6||kf[i]==21){
	partonindex[numparton]=i;
	evtnumparton[numparton]=EvtPDL::evtIdFromStdHep(kf[i]);
	numparton++;
      }
      else{
	stableindex[numstable]=i;
	evtnumstable[numstable]=EvtPDL::evtIdFromStdHep(kf[i]); 
	numstable++;
      }


      // have to protect against negative mass^2 for massless particles
      // i.e. neutrinos and photons.
      // this is uggly but I need to fix it right now....

      if (px[i]*px[i]+py[i]*py[i]+pz[i]*pz[i]>=e[i]*e[i]){

        e[i]=sqrt(px[i]*px[i]+py[i]*py[i]+pz[i]*pz[i])+0.0000000000001;

      }

      p4[i].set(e[i],px[i],py[i],pz[i]);


    }

    int channel=EvtDecayTable::inChannelList(p->getId(),numstable,evtnumstable);


   more=(channel!=-1);

   


  count++;

  }while( more && (count<10000) );

  if (count>9999) {
    report(INFO,"EvtGen") << "Too many loops in EvtJetSet!!!"<<std::endl;
    report(INFO,"EvtGen") << "Parent:"<<EvtPDL::name(getParentId())<<std::endl;
    for(i=0;i<numstable;i++){
      report(INFO,"EvtGen") << "Daug("<<i<<")"<<EvtPDL::name(evtnumstable[i])<<std::endl;
    }

  }



  if (numparton==0){

    p->makeDaughters(numstable,evtnumstable);
  
    for(i=0;i<numstable;i++){
      p->getDaug(i)->init(evtnumstable[i],p4[stableindex[i]]);
    }

    fixPolarizations(p);

    return ;
   
  }
  else{

    //have partons in JETSET

    EvtVector4R p4string(0.0,0.0,0.0,0.0);

    for(i=0;i<numparton;i++){
      p4string+=p4[partonindex[i]];
    }

    int nprimary=1;
    type[0]=STRNG;
    for(i=0;i<numstable;i++){
      if (km[stableindex[i]]==0){
	type[nprimary++]=evtnumstable[i];
      }
    }

    p->makeDaughters(nprimary,type);

    p->getDaug(0)->init(STRNG,p4string);

    EvtVector4R p4partons[10];

    for(i=0;i<numparton;i++){
      p4partons[i]=p4[partonindex[i]];
    }

    ((EvtStringParticle*)p->getDaug(0))->initPartons(numparton,p4partons,evtnumparton);



    nprimary=1;

    for(i=0;i<numstable;i++){

      if (km[stableindex[i]]==0){
	p->getDaug(nprimary++)->init(evtnumstable[i],p4[stableindex[i]]);
      }
    }


    int nsecond=0;
    for(i=0;i<numstable;i++){
      if (km[stableindex[i]]!=0){
	type[nsecond++]=evtnumstable[i];
      }
    }


    p->getDaug(0)->makeDaughters(nsecond,type);

    nsecond=0;
    for(i=0;i<numstable;i++){
      if (km[stableindex[i]]!=0){
	p4[stableindex[i]]=boostTo(p4[stableindex[i]],p4string);
	p->getDaug(0)->getDaug(nsecond)->init(evtnumstable[i],p4[stableindex[i]]);
	p->getDaug(0)->getDaug(nsecond)->setDiagonalSpinDensity();
	p->getDaug(0)->getDaug(nsecond)->decay();
	nsecond++;
      }
    }

    fixPolarizations(p);

    return ;

  }

}

void EvtJetSet::fixPolarizations(EvtParticle *p){

  //special case for now to handle the J/psi polarization

  int ndaug=p->getNDaug();
  
  int i;

  static EvtId Jpsi=EvtPDL::getId("J/psi");

  for(i=0;i<ndaug;i++){
    if(p->getDaug(i)->getId()==Jpsi){
  
      EvtSpinDensity rho;
      
      rho.SetDim(3);
      rho.Set(0,0,0.5);
      rho.Set(0,1,0.0);
      rho.Set(0,2,0.0);

      rho.Set(1,0,0.0);
      rho.Set(1,1,1.0);
      rho.Set(1,2,0.0);

      rho.Set(2,0,0.0);
      rho.Set(2,1,0.0);
      rho.Set(2,2,0.5);

      EvtVector4R p4Psi=p->getDaug(i)->getP4();

      double alpha=atan2(p4Psi.get(2),p4Psi.get(1));
      double beta=acos(p4Psi.get(3)/p4Psi.d3mag());


      p->getDaug(i)->setSpinDensityForwardHelicityBasis(rho,alpha,beta,0.0);
      setDaughterSpinDensity(i);

    }
  }

}

void EvtJetSet::store(EvtDecayBase* jsdecay){

  if (njetsetdecays==ntable){

    EvtDecayBasePtr* newjetsetdecays=new EvtDecayBasePtr[2*ntable+10];
    int i;
    for(i=0;i<ntable;i++){
      newjetsetdecays[i]=jetsetdecays[i];
    }
    ntable=2*ntable+10;
    delete [] jetsetdecays;
    jetsetdecays=newjetsetdecays;
  }

  jetsetdecays[njetsetdecays++]=jsdecay;



}


void EvtJetSet::WriteJetSetEntryHeader(ofstream &outdec, int lundkc,
			       EvtId evtnum,EvtString name,
			       int chg, int cchg, int spin2,double mass,
			       double width, double maxwidth,double ctau,
			       int stable,double rawbrfrsum){


  char sname[100];

  int namelength=16;

  int i,j;
  int temp;
  temp = spin2;

  if (ctau>1000000.0) ctau=0.0;

  strcpy(sname,name.value());

  i=0;

  while (sname[i]!=0){
    i++;
  }

  // strip up to two + or -
 
  if(evtnum.getId()>=0) {
    if (sname[i-1]=='+'||sname[i-1]=='-'){ 
      sname[i-1]=0;
      i--;
    }
    if (sname[i-1]=='+'||sname[i-1]=='-'){ 
      sname[i-1]=0;
      i--;
    }
    // strip 0 except for _0 and chi...0
    if (sname[i-1]=='0' && sname[i-2]!='_' && !(sname[0]=='c' && sname[1]=='h')){
      sname[i-1]=0;
      i--;
    }
  }

  if (i>namelength) {
    for(j=1;j<namelength;j++){
      sname[j]=sname[j+i-namelength];
    }
    sname[namelength]=0;
  }

  cchg=0;

  if(evtnum.getId()>=0) {
    if (abs(EvtPDL::getStdHep(evtnum))==21) cchg=2;
    if (abs(EvtPDL::getStdHep(evtnum))==90) cchg=-1;
    if ((abs(EvtPDL::getStdHep(evtnum))<=8)&&
	(abs(EvtPDL::getStdHep(evtnum))!=0)) cchg=1;

  }

  outdec << setw(10) << lundkc << "  ";
  outdec.width(namelength+2);
  outdec << setiosflags(ios::left) << sname << resetiosflags(ios::left);
  outdec.width(namelength);
  outdec << setiosflags(ios::left) << sname << resetiosflags(ios::left);
  outdec << setw(3) << chg;
  outdec << setw(3) << cchg;
  outdec.width(3);
  if (evtnum.getId()>=0) {
    if (EvtPDL::chargeConj(evtnum)==evtnum) {
      outdec << 0;
    }
    else{
      outdec << 1;
    }
  }
  else{
    outdec << 0;
  }
  outdec.setf(ios::fixed);
  outdec.precision(5);
  outdec << setw(12) << mass;
  outdec.setf(ios::fixed);
  outdec.precision(5);
  outdec << setw(12) << width;
  outdec.width(12);
  if (fabs(width)<0.0000000001) {
    outdec.setf(ios::fixed);
    outdec.precision(5);
    outdec << 0.0 ;
  }
  else{
    outdec.setf(ios::fixed);
    outdec.precision(5);
    outdec << maxwidth;
  }
  outdec.unsetf(ios::fixed);
  outdec.setf(ios::scientific | ios::uppercase);
  outdec.precision(5);
  outdec.width(13);
  outdec << ctau;
  outdec.unsetf(ios::scientific);
  outdec.unsetf(ios::uppercase);
  outdec.width(3);
  if (evtnum.getId()>=0) {
    if (ctau>1.0 || rawbrfrsum<0.000001) {  
      stable=0;
    }
  }
  outdec << stable;
  outdec.width(3);
  outdec << 0;
  outdec << endl;
  outdec.width(0);

}

void EvtJetSet::WriteJetSetParticle(ofstream &outdec,EvtId ipar,
				    EvtId iparname,int &first){

  int ijetset;

  double br_sum=0.0;

  for(ijetset=0;ijetset<njetsetdecays;ijetset++){
   
    if (jetsetdecays[ijetset]->getParentId()==ipar){
      br_sum+=jetsetdecays[ijetset]->getBranchingFraction();
    }
    if (jetsetdecays[ijetset]->getParentId()!=
	EvtPDL::chargeConj(jetsetdecays[ijetset]->getParentId())&&
	EvtPDL::chargeConj(jetsetdecays[ijetset]->getParentId())==ipar){
      br_sum+=jetsetdecays[ijetset]->getBranchingFraction();
    }


  }

  double br_sum_true=br_sum;

  if (br_sum<0.000001) br_sum=1.0;

  for(ijetset=0;ijetset<njetsetdecays;ijetset++){
    if (jetsetdecays[ijetset]->getParentId()==ipar){

      double br=jetsetdecays[ijetset]->getBranchingFraction();
    
      int i,daugs[5];
      EvtId cdaugs[5];
    
      for(i=0;i<5;i++){
      
	if(i<jetsetdecays[ijetset]->getNDaug()){
	  daugs[i]=EvtPDL::getStdHep(
			 jetsetdecays[ijetset]->getDaugs()[i]);
	  cdaugs[i]=EvtPDL::chargeConj(jetsetdecays[ijetset]->getDaugs()[i]);
	}
	else{
	  daugs[i]=0;
	}
      }

      int channel;

      channel=EvtDecayTable::findChannel(EvtPDL::chargeConj(ipar),
			     jetsetdecays[ijetset]->getModelName(),
			     jetsetdecays[ijetset]->getNDaug(),
			     cdaugs,
			     jetsetdecays[ijetset]->getNArg(),
			     jetsetdecays[ijetset]->getArgs());     

      if (jetsetdecays[ijetset]->getModelName()=="JETSET"){
	
	if (first) {
	  first=0;      
	  WriteJetSetEntryHeader(outdec,
				 EvtPDL::getStdHep(iparname),
				 iparname,
				 EvtPDL::name(iparname), 
				 EvtPDL::chg3(iparname),
				 0,0,EvtPDL::getNominalMass(ipar),
				 EvtPDL::getWidth(ipar),
				 EvtPDL::getNominalMass(ipar)-EvtPDL::getMinMass(ipar),
				 EvtPDL::getctau(ipar),1,br_sum_true);
	}
	
	int dflag=2;

	if (EvtPDL::getStdHep(ipar)<0) {
	  dflag=3;
	  for(i=0;i<jetsetdecays[ijetset]->getNDaug();i++){
	    daugs[i]=EvtPDL::getStdHep(cdaugs[i]);
	  }

	}

	//now lets check to make sure that jetset, lucomp, knows
        //about all particles!
	int unknown=0;
	for(i=0;i<jetsetdecays[ijetset]->getNDaug();i++){
#ifdef WIN32
	  if (PYCOMP(&daugs[i])==0) {
#else
	  if (pycomp_(&daugs[i])==0) {
#endif
	    unknown=1;
	    report(ERROR,"EvtGen") << "JetSet (pycomp) does not "
				  << "know the particle:"<<
	      EvtPDL::name(jetsetdecays[ijetset]->getDaugs()[i])<<std::endl;
	  }
	}

	int istdheppar=EvtPDL::getStdHep(ipar);

#ifdef WIN32
	if (PYCOMP(&istdheppar)==0){
#else
	if (pycomp_(&istdheppar)==0){
#endif
	  unknown=1;
	  report(ERROR,"EvtGen") << "JetSet (pycomp) does not "
                           << "know the particle:"<<
      EvtPDL::name(ipar)<<endl;
	}



	if (unknown){
	  report(ERROR,"EvtGen") << "Therfore the decay:"<<std::endl;
	  report(ERROR,"EvtGen") << EvtPDL::name(jetsetdecays[ijetset]->getParentId())<<" -> ";
	  for(i=0;i<jetsetdecays[ijetset]->getNDaug();i++){
	    report(ERROR,"") << EvtPDL::name(jetsetdecays[ijetset]->getDaugs()[i])<<" ";
	  }
	  report(ERROR,"")<<std::endl;
	  report(ERROR,"EvtGen")<<"Will not be generated."<<std::endl;
	  return;
	}


	if (EvtPDL::chargeConj(ipar)==ipar) {
	  dflag=1;
	  //report(INFO,"EvtGen") << EvtPDL::name(iparname) << " dflag=1 because C(ipar)=ipar!"<<std::endl;
	}


	//if (channel>=0) {
	//  dflag=1;
	  //report(INFO,"EvtGen") << EvtPDL::name(iparname) << " dflag=1 because channel>=0"<<std::endl;
	//}
 
	//	if (!(EvtPDL::getStdHep(ipar)<0&&channel>=0)){
	if (1){

	  outdec.width(15);
	  outdec <<dflag;
	  outdec.width(5);
	  outdec <<(int)jetsetdecays[ijetset]->getArgs()[0];
          outdec.setf(ios::fixed);
	  outdec.width(12);
          outdec.precision(6);
	  if (fabs(br)<0.000000001) {
	    outdec <<"0.00000";
	  }
	  else{
	    outdec <<br/br_sum;
	  }
	  outdec.width(10);
	  outdec <<daugs[0];
	  outdec.width(10);
	  outdec <<daugs[1];
	  outdec.width(10);
	  outdec <<daugs[2];
	  outdec.width(10);
	  outdec <<daugs[3];
	  outdec.width(10);
	  outdec <<daugs[4];
	  outdec<<endl;
	  outdec.width(0);
	}
      }
    }
  }
}


void EvtJetSet::MakeJetSetFile(char* fname){
  
  EvtId ipar;
  int lundkc;
  
  //int part_list[MAX_PART];
  
  ofstream outdec;
 
  outdec.open(fname);
  
  //outdec << ";"<<std::endl;
  //outdec << ";This decayfile has been automatically created by"<<std::endl;
  //outdec << ";EvtGen from the DECAY.DEC file"<<std::endl;
  //outdec << ";"<<std::endl;

  int nokcentry;
  int logpr = 0; 

  for(lundkc=1;lundkc<=500;lundkc++){

    nokcentry=1;

    int iipar;

    for(iipar=0;iipar<EvtPDL::entries();iipar++){

      ipar=EvtId(iipar,iipar);
      if (EvtDecayTable::isJetSet(ipar)) {
//    if (logpr == 0)  
//     cout << iipar << " EvtPDL name " << EvtPDL::name(ipar) << "Lund code " 
//      << EvtPDL::getLundKC(ipar) << " StdHep code " <<
//      EvtPDL::getStdHep(ipar)<< endl;


      if (lundkc==EvtPDL::getLundKC(ipar)){
        
        nokcentry=0;

	int first=1;
    
	WriteJetSetParticle(outdec,ipar,ipar,first);

	
	EvtId ipar2=EvtPDL::chargeConj(ipar);


        if (ipar2!=ipar){
	  WriteJetSetParticle(outdec,ipar2,ipar,first);
	}

        if (first){
	  WriteJetSetEntryHeader(outdec, 
				    EvtPDL::getStdHep(ipar),
				    ipar,
				    EvtPDL::name(ipar),
				    EvtPDL::chg3(ipar),
				    0,0,EvtPDL::getNominalMass(ipar),
				    EvtPDL::getWidth(ipar),
				    EvtPDL::getNominalMass(ipar)-EvtPDL::getMinMass(ipar),
				    EvtPDL::getctau(ipar),0,0.0);

	}
      }
      }
    }
    logpr = 1;
    if (nokcentry){
        int part_code = EvtPDL::getStdHep(ipar);
/*
        if (part_code == 0) 
              WriteJetSetEntryHeader(outdec, 
                   10,EvtId(-1,-1),"  ",
                   0,0,0,EvtPDL::getNominalMass(ipar),0.0,0.0,
                   EvtPDL::getctau(ipar),0,0.0);
        else
	  WriteJetSetEntryHeader(outdec, 
				EvtPDL::getStdHep(ipar),EvtId(-1,-1),"  ",
				0,0,0,EvtPDL::getNominalMass(ipar),0.0,0.0,
				EvtPDL::getctau(ipar),0,0.0);
*/
        if (part_code != 0) 
	  WriteJetSetEntryHeader(outdec, 
				EvtPDL::getStdHep(ipar),EvtId(-1,-1),"  ",
				0,0,0,EvtPDL::getNominalMass(ipar),0.0,0.0,
				EvtPDL::getctau(ipar),0,0.0);
    }
  }
    outdec.close();
}

void EvtJetSet::jetSetInit(){

  EvtId ipar;
  int lundkc;
  double mass, width, maxwidth, ctau;
  static int first=1;

  if (first){

    first=0;

    report(INFO,"EvtGen") << "Will initialize JetSet."<<std::endl;

    char fname[200];

    char hostBuffer[100];
    
// NBrook - update masses, width etc in PYTHIA commom
    int iipar;
    for(iipar=0;iipar<EvtPDL::entries();iipar++){
     ipar=EvtId(iipar,iipar);
     lundkc=EvtPDL::getLundKC(ipar);
     if(lundkc!=0) {
       mass = EvtPDL::getNominalMass(ipar);
       width = EvtPDL::getWidth(ipar);
       maxwidth = mass-EvtPDL::getMinMass(ipar);
       ctau = EvtPDL::getctau(ipar);
#ifdef WIN32
       EVTJETSETPART(&lundkc,&mass,&width,&maxwidth,&ctau);
#else
       evtjetsetpart_(&lundkc,&mass,&width,&maxwidth,&ctau);
#endif
     }
    }


//
    strcpy(fname,"EvtGenJetSet.newdata");
    MakeJetSetFile(fname);
#ifdef WIN32
    EVTJETSETINIT(fname,strlen(fname));
#else
    evtjetsetinit_(fname,strlen(fname));
#endif

    if (0==getenv("EVTSAVEJETD")){
      char delcmd[300];
      strcpy(delcmd,"rm -f ");
      strcat(delcmd,fname);
      //      system(delcmd);
    }

    int i;

    for(i=0;i<ncommand;i++){

#ifdef WIN32
      PYGIVE(commands[i].value(),strlen(commands[i].value()));
#else
      pygive_(commands[i].value(),strlen(commands[i].value()));
#endif

    }

    report(INFO,"EvtGen") << "Done initializing JetSet."<<std::endl;


  }

}
