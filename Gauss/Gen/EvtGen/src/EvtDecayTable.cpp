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
// Module: EvtDecayTable.cc
//
// Description:
//
// Modification history:
//
//    DJL/RYD     September 25, 1996         Module created
//
//------------------------------------------------------------------------
// 

#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtRandom.hh"
#include "EvtGen/EvtDecayTable.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtDecayParm.hh"
#include "EvtGen/EvtSymTable.hh"
#include "EvtGen/EvtDecayBase.hh"
#include "EvtGen/EvtModel.hh"
#include "EvtGen/EvtParticleDecayList.hh"
#include "EvtGen/EvtParser.hh"
#include "EvtGen/EvtVectorT.hh"
#include "EvtGen/EvtReport.hh"


static EvtModel modelist;

static EvtSymTable symtable;

static EvtParticleDecayList decaytable[MAX_PART];

int EvtDecayTable::getNMode(int ipar){
   return decaytable[ipar].getNMode();
} 


void EvtDecayTable::printSummary(){

  int i;
  
  for(i=0;i<MAX_PART;i++){
    decaytable[i].printSummary();
  }

}

EvtDecayBase* EvtDecayTable::GetDecayFunc(EvtParticle *p){

  int partnum;
  
  partnum=p->getId().getAlias();

  return decaytable[partnum].getDecayModel(p);

}

void EvtDecayTable::readDecayFile(const EvtString dec_name){

  int i;
  int j;

  int noPhotos=0;
  int yesPhotos=0;

  report(INFO,"EvtGen") << "In readDecayFile, reading:"<<dec_name<<endl;
  
  ifstream fin;
  
  fin.open(dec_name.value());
  if (!fin) {
    report(ERROR,"EvtGen") << "Could not open "<<dec_name<<endl;
  }
  fin.close();

  EvtParser parser;
  parser.Read(dec_name);

  int itok;

  int hasend=0;

  EvtString token;

  for(itok=0;itok<parser.getNToken();itok++){

    token=parser.getToken(itok);
    
    if (token=="End") hasend=1;

  }

  if (!hasend){
    report(ERROR,"EvtGen") << "Could not find an 'End' in "<<dec_name<<endl;
    report(ERROR,"EvtGen") << "Will terminate execution."<<endl;
    ::abort();
  }



  EvtString model,parent,sdaug;  

  EvtId ipar;
  int narg;
  int n_daugh;
  EvtId daught[MAX_DAUG];
  double brfr;

  static int first=1;

  if (first){
    modelist.ModelsStored();
    first=0;
  }

  int itoken=0;
  
  do{

    token=parser.getToken(itoken++);

    //Easy way to turn off photos... Lange September 5, 2000
    if (token=="noPhotos"){ 
      noPhotos=1; 
      yesPhotos=0;
      report(INFO,"EvtGen") 
	<< "As requested, PHOTOS will be turned off."<<endl; 
    }
    else if (token=="yesPhotos"){ 
      yesPhotos=1;
      noPhotos=0;
      report(INFO,"EvtGen") 
	<< "As requested, PHOTOS will be turned on."<<endl; 
    }
    else if (token=="Alias"){

      EvtString newname;
      EvtString oldname;

      newname=parser.getToken(itoken++);
      oldname=parser.getToken(itoken++);

      EvtId id=EvtPDL::getId(oldname);

      if (id==EvtId(-1,-1)) {
	report(ERROR,"EvtGen") <<"Unknown particle name:"<<oldname
			       <<" on line "<<parser.getLineofToken(itoken)<<endl;
	report(ERROR,"EvtGen") <<"Will terminate execution!"<<endl;
	::abort();
      }

      EvtPDL::alias(id,newname);
    } else if (token=="ChargeConj"){

      EvtString aname;
      EvtString abarname;

      aname=parser.getToken(itoken++);
      abarname=parser.getToken(itoken++);

      EvtId a=EvtPDL::getId(aname);
      EvtId abar=EvtPDL::getId(abarname);

      if (a==EvtId(-1,-1)) {
	report(ERROR,"EvtGen") <<"Unknown particle name:"<<aname
			       <<" on line "<<parser.getLineofToken(itoken)<<endl;
	report(ERROR,"EvtGen") <<"Will terminate execution!"<<endl;
	::abort();
      }

      if (abar==EvtId(-1,-1)) {
	report(ERROR,"EvtGen") <<"Unknown particle name:"<<abarname
			       <<" on line "<<parser.getLineofToken(itoken)<<endl;
	report(ERROR,"EvtGen") <<"Will terminate execution!"<<endl;
	::abort();
      }


      EvtPDL::aliasChgConj(a,abar);

    } else if (modelist.isCommand(token)){

      EvtString cnfgstr;

      cnfgstr=parser.getToken(itoken++);

      modelist.storeCommand(token,cnfgstr);

    } else if (token=="CDecay"){

      int k;
      EvtString name;

      name=parser.getToken(itoken++);
      ipar=EvtPDL::getId(name);

      if (ipar==EvtId(-1,-1)) {
	report(ERROR,"EvtGen") <<"Unknown particle name:"<<name
			       <<" on line "
			       <<parser.getLineofToken(itoken-1)<<endl;
	report(ERROR,"EvtGen") <<"Will terminate execution!"<<endl;
	::abort();
      }

      EvtId cipar=EvtPDL::chargeConj(ipar);

      if (decaytable[ipar.getAlias()].getNMode()!=0) {

	report(DEBUG,"EvtGen") << 
	  "Redefined decay of "<<name<<" in CDecay"<<endl;

	decaytable[ipar.getAlias()].removeDecay();
      }

      //take contents of cipar and conjugate and store in ipar
      decaytable[ipar.getAlias()].makeChargeConj(&decaytable[cipar.getAlias()]);

    } else if (token=="Define"){

      EvtString name;
      double value;

      name=parser.getToken(itoken++);
      value=atof(parser.getToken(itoken++).value());

      symtable.Define(name,value);

      //New code Lange April 10, 2001 - allow the user
      //to change particle definitions of EXISTING
      //particles on the fly
    } else if (token=="Particle"){

      EvtString pname;
      pname=parser.getToken(itoken++);
      cout << pname << endl;
      //There should be at least the mass 
      double newMass=atof(parser.getToken(itoken++).value());
      EvtId thisPart = EvtPDL::getId(pname);
      double newWidth=EvtPDL::getNominalMass(thisPart);
      if ( parser.getNToken() > 3 ) newWidth=atof(parser.getToken(itoken++).value());

      //Now make the change!
      EvtPDL::reSetMass(thisPart, newMass);
      EvtPDL::reSetWidth(thisPart, newWidth);

      report(INFO,"EvtGen") << "Changing particle properties of " <<
	pname << " Mass=" << newMass << " Width="<<newWidth<<endl;

      

    } else if (token=="Decay") {

      EvtString temp_fcn_new_model;
      int temp_fcn_new_ndaug;
      EvtVectorT<double> temp_fcn_new_args(500);
      int temp_fcn_new_narg;
      EvtId temp_fcn_new_daug[MAX_DAUG];
      EvtDecayBase* temp_fcn_new;
      double temp_sbrfr;
      double temp_massmin;
      int temp_nmode;
      
      double brfrsum=0.0;

  

      parent=parser.getToken(itoken++);
      ipar=EvtPDL::getId(parent);

      if (ipar==EvtId(-1,-1)) {
	report(ERROR,"EvtGen") <<"Unknown particle name:"<<parent
			       <<" on line "
			       <<parser.getLineofToken(itoken-1)<<endl;
	report(ERROR,"EvtGen") <<"Will terminate execution!"<<endl;
	::abort();
      }

      if (decaytable[ipar.getAlias()].getNMode()!=0) {
	report(DEBUG,"EvtGen") <<"Redefined decay of "
			       <<parent<<endl;
	decaytable[ipar.getAlias()].removeDecay();
      }


      do{

        token=parser.getToken(itoken++);

        if (token!="Enddecay"){

	  i=0;
	  while (token.value()[i++]!=0){
	    if (isalpha(token.value()[i])){
	      report(ERROR,"EvtGen") << 
		"Expected to find a branching fraction or Enddecay "<<
		"but found:"<<token<<" on line "<<
		parser.getLineofToken(itoken-1)<<endl;
	      report(ERROR,"EvtGen") << "Possibly to few arguments to model "<<
		"on previous line!"<<endl;
	      report(ERROR,"EvtGen") << "Will terminate execution!"<<endl;
	      ::abort();
	    }
	  }
	  
          brfr=atof(token.value());

	  int isname=EvtPDL::getId(parser.getToken(itoken)).getId()>=0;
          int ismodel=modelist.isModel(parser.getToken(itoken));

          if (!(isname||ismodel)){

	    report(INFO,"EvtGen") << parser.getToken(itoken)
	     << " is neither a particle name nor "
	     << "the name of a model. "<<endl;
	    report(INFO,"EvtGen") << "It was encountered on line "<<
	      parser.getLineofToken(itoken)<<" of the decay file."<<endl;
	    report(INFO,"EvtGen") << "Please fix it. Thank you."<<endl;
	    report(INFO,"EvtGen") << "Be sure to check that the "
	     << "correct case has been used. \n";
	    report(INFO,"EvtGen") << "Terminating execution. \n";
	    ::abort();

	    itoken++;
	  }

          n_daugh=0;

	  while(EvtPDL::getId(parser.getToken(itoken)).getId()>=0){
            sdaug=parser.getToken(itoken++);
            daught[n_daugh++]=EvtPDL::getId(sdaug);
	    if (daught[n_daugh-1]==EvtId(-1,-1)) {
	      report(ERROR,"EvtGen") <<"Unknown particle name:"<<sdaug
				     <<" on line "<<parser.getLineofToken(itoken)<<endl;
	      report(ERROR,"EvtGen") <<"Will terminate execution!"<<endl;
	      ::abort();
	    }
          }

	  
          model=parser.getToken(itoken++);


          int photos=0;
          int verbose=0;
          int summary=0;
	  
	  do{
	    if (model=="PHOTOS"){
	      if ( noPhotos==0 ) photos=1;
	      model=parser.getToken(itoken++);
	    }
	    if (model=="VERBOSE"){
	      verbose=1;
	      model=parser.getToken(itoken++);
	    }
	    if (model=="SUMMARY"){
	      summary=1;
	      model=parser.getToken(itoken++);
	    }
	  }while(model=="PHOTOS"||
		 model=="VERBOSE"||
		 model=="SUMMARY");

	  if ( yesPhotos==1) photos=1;


	  if(!modelist.isModel(model)){
	    report(ERROR,"EvtGen") << 
	      "Expected to find a particle or model name,"<<
	      "found:"<<model<<" on line "<<
	      parser.getLineofToken(itoken)<<endl;
	    report(ERROR,"EvtGen") << "Will terminate execution!"<<endl;
	    ::abort();

	  }

	  temp_fcn_new_model=model;
          temp_fcn_new=modelist.GetFcn(model);

          if (photos){
	    temp_fcn_new->setPHOTOS();
	  }
          if (verbose){
	    temp_fcn_new->setVerbose();
	  }
          if (summary){
	    temp_fcn_new->setSummary();
	  }
	  

	  narg=0;
	  EvtString name;
	  int ierr;

          do{
            name=parser.getToken(itoken++);
	    if (name!=";") {
	      temp_fcn_new_args[narg++]=symtable.Get(name,ierr);
	      if (ierr) {
		report(ERROR,"EvtGen")
		  <<"Reading arguments and found:"<<
		  name<<" on line:"<<
		  parser.getLineofToken(itoken-1)<<endl;
		report(ERROR,"EvtGen") 
		  << "Will terminate execution!"<<endl;
		::abort();
	      }
	    }
	    //int isname=EvtPDL::getId(name).getId()>=0;
	    int ismodel=modelist.isModel(name);
	    if (ismodel) {
	      report(ERROR,"EvtGen")
		<<"Expected ';' but found:"<<
		name<<" on line:"<<
		parser.getLineofToken(itoken-1)<<endl;
	      report(ERROR,"EvtGen") 
		<< "Most probable error is omitted ';'."<<endl;
	      report(ERROR,"EvtGen") 
		<< "Will terminate execution!"<<endl;
	      ::abort();
	    }
          }while(name!=";");

	  //Found one decay.

	  brfrsum+=brfr;


	  temp_fcn_new->saveDecayInfo(ipar,n_daugh,
				      daught,
				      narg,
				      temp_fcn_new_args,
				      temp_fcn_new_model,
				      brfr);

	  double massmin=0.0;

          for (i=0;i<n_daugh;i++){
	    if ( EvtPDL::getMinMass(daught[i])>0.0001 ){
              massmin+=EvtPDL::getMinMass(daught[i]);
	    } else {
              massmin+=EvtPDL::getNominalMass(daught[i]);
	    }  
	  } 
	  
	  decaytable[ipar.getAlias()].addMode(temp_fcn_new,brfrsum,massmin);
	  

	}
      } while(token!="Enddecay");      

      decaytable[ipar.getAlias()].finalize();

    }
    else if (token!="End"){

      report(ERROR,"EvtGen") << "Found unknown command:'"<<token<<"' on line "
			     <<parser.getLineofToken(itoken)<<endl;
      report(ERROR,"EvtGen") << "Will terminate execution!"<<endl;
      ::abort();

    }

  } while ((token!="End")&&itoken!=parser.getNToken());
}

int  EvtDecayTable::findChannel(EvtId parent, EvtString model, 
				int ndaug, EvtId *daugs, 
				int narg, double *args){

   int i,j,right;
   EvtId daugs_scratch[50];
   int nmatch,k;

   for(i=0;i<decaytable[parent.getAlias()].getNMode();i++){

     right=1;

     right=right&&model==decaytable[parent.getAlias()].
		    getDecay(i).getDecayModel()->getModelName();
     right=right&&(ndaug==decaytable[parent.getAlias()].
	     getDecay(i).getDecayModel()->getNDaug());
     right=right&&(narg==decaytable[parent.getAlias()].
	     getDecay(i).getDecayModel()->getNArg());

     if ( right ){

       

       for(j=0;j<ndaug;j++){
	 daugs_scratch[j]=daugs[j];
       }

       nmatch=0;

       for(j=0;j<decaytable[parent.getAlias()].
	     getDecay(i).getDecayModel()->getNDaug();j++){

         for(k=0;k<ndaug;k++){
	   if (daugs_scratch[k]==decaytable[parent.getAlias()].
	       getDecay(i).getDecayModel()->getDaug(j)){
             daugs_scratch[k]=EvtId(-1,-1);
             nmatch++;
	     break;
	   }
	 }
       } 

       right=right&&(nmatch==ndaug);

       for(j=0;j<decaytable[parent.getAlias()].
	     getDecay(i).getDecayModel()->getNArg();j++){
	 right=right&&(args[j]==decaytable[parent.getAlias()].
		 getDecay(i).getDecayModel()->getArg(j));
       } 
     }
     if (right) return i;
   }
   return -1;
}

int  EvtDecayTable::inChannelList(EvtId parent, int ndaug, EvtId *daugs){

   int i,j,k;
   EvtId daugs_scratch[MAX_DAUG];

   int dsum=0;
   for(i=0;i<ndaug;i++){
     dsum+=daugs[i].getAlias();
   }

   int nmatch;

   int ipar=parent.getAlias();

   int nmode=decaytable[ipar].getNMode();

   for(i=0;i<nmode;i++){

     EvtDecayBase* thedecaymodel=decaytable[ipar].getDecay(i).getDecayModel();

     if (thedecaymodel->getDSum()==dsum){

       int nd=thedecaymodel->getNDaug();

       if (ndaug==nd){
	 for(j=0;j<ndaug;j++){
	   daugs_scratch[j]=daugs[j];
	 }
	 nmatch=0;
	 for(j=0;j<nd;j++){
	   for(k=0;k<ndaug;k++){
	     if (EvtId(daugs_scratch[k])==thedecaymodel->getDaug(j)){
	       daugs_scratch[k]=EvtId(-1,-1);
	       nmatch++;
	       break;
	     }
	   }
	 } 
	 if ((nmatch==ndaug)&&(thedecaymodel->getModelName()!="JETSET")){
	   return i;
	 }
       }
     }
   }

   return -1;
}
   
