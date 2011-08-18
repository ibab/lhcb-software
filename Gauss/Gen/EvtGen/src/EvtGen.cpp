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
// Module: EvtGen.cc
//
// Description: Main class to provide user interface to EvtGen.
//
// Modification history:
//
//    RYD     March 24, 1998        Module created
//    JBack   June 2011             Added HepMC event interface
//
//------------------------------------------------------------------------
// 
#include "EvtGenBase/EvtPatches.hh"
#include <stdio.h>
#include <fstream>
#include <math.h>
#include "EvtGenBase/EvtComplex.hh"
#include <stdlib.h>
#include "EvtGen/EvtGen.hh"
#include "EvtGenBase/EvtVector4R.hh"
#include "EvtGenBase/EvtVectorParticle.hh"
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtScalarParticle.hh"
#include "EvtGenBase/EvtDecayTable.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtStdHep.hh"
#include "EvtGenBase/EvtSecondary.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenBase/EvtId.hh"
#include "EvtGenBase/EvtRandom.hh"
#include "EvtGenBase/EvtRandomEngine.hh"
#include "EvtGenBase/EvtSimpleRandomEngine.hh"
#include "EvtGenBase/EvtParticleFactory.hh"
#include "EvtGenModels/EvtModelReg.hh"
#include "EvtGenBase/EvtStatus.hh"
#include "EvtGenBase/EvtAbsRadCorr.hh"
#include "EvtGenBase/EvtRadCorr.hh"
#include "EvtGenModels/EvtPHOTOS.hh"
#include "EvtGenBase/EvtCPUtil.hh"
#include "EvtGenBase/EvtHepMCEvent.hh"

#include "EvtGenModels/EvtExternalGenFactory.hh"
#include <string>

using std::endl;
using std::fstream;
using std::ifstream;

EvtGen::~EvtGen(){

  //This is a bit uggly, should not do anything
  //in a destructor. This will fail if EvtGen is made a static
  //because then this destructor might be called _after_
  //the destruction of objects that it depends on, e.g., EvtPDL.

  if (getenv("EVTINFO")){
    EvtDecayTable::getInstance()->printSummary();
  }

}

EvtGen::EvtGen(const char* const decayName,
	       const char* const pdtTableName,
	       EvtRandomEngine* randomEngine,
	       EvtAbsRadCorr* isrEngine,
	       const std::list<EvtDecayBase*>* extraModels,
	       int mixingType){


  report(INFO,"EvtGen") << "Initializing EvtGen"<<endl;

  if (randomEngine==0){
    static EvtSimpleRandomEngine defaultRandomEngine;
    EvtRandom::setRandomEngine((EvtRandomEngine*)&defaultRandomEngine);
    report(INFO,"EvtGen") <<"No random engine given in "
			  <<"EvtGen::EvtGen constructor, "
			  <<"will use default EvtSimpleRandomEngine."<<endl;
  }
  else{
    EvtRandom::setRandomEngine(randomEngine);    
  }

  report(INFO,"EvtGen") << "Storing known decay models"<<endl;
  EvtModelReg dummy(extraModels);

  report(INFO,"EvtGen") << "Main decay file name  :"<<decayName<<endl;
  report(INFO,"EvtGen") << "PDT table file name   :"<<pdtTableName<<endl;
  
  _pdl.readPDT(pdtTableName);

  EvtDecayTable::getInstance()->readDecayFile(decayName,false);

  _mixingType = mixingType;
  report(INFO,"EvtGen") << "Mixing type integer set to "<<_mixingType<<endl;
  EvtCPUtil::getInstance()->setMixingType(_mixingType);

  // Get the external generator factory interface initialised. Do this after
  // the particle data table and decay.dec files have been read, since
  // we normally need to pass particle data info using the EvtPDL class,
  // which is only set-up properly once the previous steps have been done.

  EvtExternalGenFactory* externalGenerators = EvtExternalGenFactory::getInstance();

  // Set the radiative correction engine
  report(INFO,"EvtGen") << "Defining the radiative correction engine"<<endl;

  if (isrEngine != 0) {

    EvtRadCorr::setRadCorrEngine(isrEngine);    

  } else {

    // Define the photon type (and pass it to the external generator, not EvtPHOTOS).
    std::string photonType("gamma");
    externalGenerators->definePhotosGenerator(photonType);

    EvtPHOTOS* defaultRadCorrEngine = new EvtPHOTOS();
    EvtRadCorr::setRadCorrEngine(defaultRadCorrEngine);

  }

  // Set the Pythia external generator
  // We are using Pythia 6 physics codes in the decay.dec file(s).
  std::string xmlDir("./xmldoc");
  bool convertPhysCode(true);
  if (convertPhysCode) {
    report(INFO,"EvtGen") << "Defining the PYTHIA 8 generator: data tables in "
			  << xmlDir << ".\n Will convert Pythia 6 codes in decayfiles "
			  << "to Pythia 8 codes." << endl;
  } else {
    report(INFO,"EvtGen") << "Defining the PYTHIA 8 generator: data tables in "
			  << xmlDir << ".\n Decay files must use Pythia 8 physics codes." << endl;
  }

  externalGenerators->definePythiaGenerator(xmlDir, convertPhysCode);

  // Set the Tauola external generator
  externalGenerators->defineTauolaGenerator();

  _initExternalGenerators = false;

  report(INFO,"EvtGen") << "Done initializing EvtGen"<<endl;

}


void EvtGen::readUDecay(const char* const uDecayName){

  ifstream indec;

  if ( uDecayName[0] == 0) {
    report(INFO,"EvtGen") << "Is not reading a user decay file!"<<endl;
  }
  else{  
    indec.open(uDecayName);
    if (indec) {
      EvtDecayTable::getInstance()->readDecayFile(uDecayName,true);
    }    
    else{
      
      report(INFO,"EvtGen") << "Can not find UDECAY file '"
			    <<uDecayName<<"'.  Stopping"<<endl;
      ::abort();
    }
  }
  
}

void EvtGen::initExternalGenerators() {

  report(INFO,"EvtGen")<<"Initialising all external generators"<<endl;
  
  EvtExternalGenFactory* externalGenerators = EvtExternalGenFactory::getInstance();
  externalGenerators->initialiseAllGenerators();

  _initExternalGenerators = true;

  report(INFO,"EvtGen")<<"Done initialising external generators"<<endl;

}

EvtHepMCEvent* EvtGen::generateDecay(int PDGId, EvtVector4R refFrameP4,
				     EvtVector4R translation,
				     EvtSpinDensity* spinDensity) {

  EvtParticle* theParticle(0);

  if (spinDensity == 0 ){
    theParticle = EvtParticleFactory::particleFactory(EvtPDL::evtIdFromStdHep(PDGId),
						      refFrameP4);
  } else {
    theParticle = EvtParticleFactory::particleFactory(EvtPDL::evtIdFromStdHep(PDGId),
						      refFrameP4, *spinDensity);
  }

  generateDecay(theParticle);
  EvtHepMCEvent* hepMCEvent = new EvtHepMCEvent();
  hepMCEvent->constructEvent(theParticle, translation);

  theParticle->deleteTree();

  return hepMCEvent;

}

void EvtGen::generateDecay(EvtParticle *p){

  if (_initExternalGenerators == false) {
    // Make sure any external generators are initialised. Check this for the
    // first decay we want. This initialisation can only be done after all decay
    // files have been read in.
    this->initExternalGenerators();
  }
 
  int times=0;
  do{
    times+=1;
    EvtStatus::initRejectFlag();

    p->decay();
    //ok then finish.
    if ( EvtStatus::getRejectFlag()==0 ) { 
      times=0;
    }
    else{   
      for (size_t ii=0;ii<p->getNDaug();ii++){
	EvtParticle *temp=p->getDaug(ii);
	temp->deleteTree();
      }
      p->resetFirstOrNot();
      p->resetNDaug();
      
    }

    if ( times==10000) {
      report(ERROR,"EvtGen") << "Your event has been rejected 10000 times!"<<endl;
      report(ERROR,"EvtGen") << "Will now abort."<<endl;
      ::abort();
      times=0;
    }
  } while (times);

}
