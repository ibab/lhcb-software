//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package. If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 2011      University of Warwick, UK
//
// Module: EvtExternalGenFactory
//
// Description: A factory type method to create engines for external physics
// generators like Pythia.
//
// Modification history:
//
//    John Back       April 2011            Module created
//
//------------------------------------------------------------------------------
//

#include "EvtGenBase/EvtPatches.hh"
#include "EvtGenModels/EvtExternalGenFactory.hh"
#include "EvtGenBase/EvtReport.hh"

#include "EvtGenModels/EvtPythiaEngine.hh"
#include "EvtGenModels/EvtPhotosEngine.hh"
#include "EvtGenModels/EvtTauolaEngine.hh"

#include <iostream>
using std::endl;

EvtExternalGenFactory::EvtExternalGenFactory() {

  _extGenMap.clear();

}

EvtExternalGenFactory::~EvtExternalGenFactory() {

  ExtGenMap::iterator iter;
  for (iter = _extGenMap.begin(); iter != _extGenMap.end(); ++iter) {

    EvtAbsExternalGen* theGenerator = iter->second;
    delete theGenerator;

  }
  
  _extGenMap.clear();

}

EvtExternalGenFactory* EvtExternalGenFactory::getInstance() {

  static EvtExternalGenFactory* theFactory = 0;
  
  if (theFactory == 0) {
    theFactory = new EvtExternalGenFactory();
  }

  return theFactory;

}

void EvtExternalGenFactory::definePythiaGenerator(std::string xmlDir, bool convertPhysCodes) {

  int genId = EvtExternalGenFactory::PythiaGenId;

  EvtAbsExternalGen* pythiaGenerator = new EvtPythiaEngine(xmlDir, convertPhysCodes);
  _extGenMap[genId] = pythiaGenerator;

}

void EvtExternalGenFactory::definePhotosGenerator(std::string photonType) {

  int genId = EvtExternalGenFactory::PhotosGenId;

  EvtAbsExternalGen* photosGenerator = new EvtPhotosEngine(photonType);
  _extGenMap[genId] = photosGenerator;

}

void EvtExternalGenFactory::defineTauolaGenerator() {

  int genId = EvtExternalGenFactory::TauolaGenId;

  EvtAbsExternalGen* tauolaGenerator = new EvtTauolaEngine();
  _extGenMap[genId] = tauolaGenerator;

}

EvtAbsExternalGen* EvtExternalGenFactory::getGenerator(int genId) {

  EvtAbsExternalGen* theGenerator(0);

  ExtGenMap::iterator iter;

  if ((iter = _extGenMap.find(genId)) != _extGenMap.end()) {

    // Retrieve the external generator engine
    theGenerator = iter->second;

  } else {

    report(INFO,"EvtGen")<<"EvtAbsExternalGen::getGenerator: could not find generator for genId = "
			 <<genId<<endl;

  }

  return theGenerator;

}

void EvtExternalGenFactory::initialiseAllGenerators() {

  ExtGenMap::iterator iter;
  for (iter = _extGenMap.begin(); iter != _extGenMap.end(); ++iter) {

    EvtAbsExternalGen* theGenerator = iter->second;
    if (theGenerator != 0) {
      theGenerator->initialise();
    }

  }
  
}
