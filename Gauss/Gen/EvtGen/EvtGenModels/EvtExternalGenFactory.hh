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

#ifndef EVTEXTERNALGENFACTORY_HH
#define EVTEXTERNALGENFACTORY_HH

#include "EvtGenModels/EvtAbsExternalGen.hh"

#include <map>

class EvtExternalGenFactory {

public:
  
  enum genId {PythiaGenId = 0, PhotosGenId, TauolaGenId};

  static EvtExternalGenFactory* getInstance();

  EvtAbsExternalGen* getGenerator(int genId = 0);

  void initialiseAllGenerators();

  void definePythiaGenerator(std::string xmlDir, bool convertPhysCodes);
  void definePhotosGenerator(std::string photonType = "gamma");
  void defineTauolaGenerator();

protected:

  EvtExternalGenFactory();
  ~EvtExternalGenFactory();

  typedef std::map<int, EvtAbsExternalGen*> ExtGenMap;

private:

  EvtExternalGenFactory(const EvtExternalGenFactory&) {};

  ExtGenMap _extGenMap;

};

#endif
