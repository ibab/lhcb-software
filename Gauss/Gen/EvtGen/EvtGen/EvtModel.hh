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
// Module: EvtGen/EvtModel.hh
//
// Description: 
//
// Modification history:
//
//    DJL/RYD     August 8, 1998         Module created
//
//------------------------------------------------------------------------

#ifndef EVTMODEL_HH
#define EVTMODEL_HH

#include "EvtGen/EvtDecayBase.hh"
#include "EvtGen/EvtId.hh"
#include "EvtGen/EvtDecayTable.hh"
#include "EvtGen/EvtStringHash.hh"
#include <fstream.h>

class EvtString;

//Class to read in and handle the decays available
//to EvtGen for each particle, and the model to be
//used for each one.

class EvtModel{

public:

  EvtModel();
  void Register(EvtDecayBase* prototype);
      
  void ModelsStored(void);

  int isModel(EvtString name);

  EvtDecayBase* GetFcn(EvtString model_name);

  int isCommand(EvtString cmd);
  void storeCommand(EvtString cmd,EvtString cnfgstr);


private:

  EvtStringHash<EvtDecayBase>* _modelNameHash;
  EvtStringHash<EvtDecayBase>* _commandNameHash;

};

#endif



