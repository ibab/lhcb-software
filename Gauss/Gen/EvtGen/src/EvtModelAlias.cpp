//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 2002      Caltech, LLNL
//
// Module: EvtGen/EvtModelAlias.cc
//
// Description:Class to keep track of model aliases 
//             read in from the decay table
//
// Modification history:
//
//    Lange     January 19, 2002         Module created
//
//------------------------------------------------------------------------

#ifdef WIN32 
  #pragma warning( disable : 4786 ) 
  // Disable anoying warning about symbol size 
#endif 
#include "EvtGenBase/EvtModelAlias.hh"

EvtModelAlias::EvtModelAlias(std::string alias, std::string model, std::vector<std::string> args):

  _aliasName(alias)
  ,_model(model)

{

  if ( args.size() > 0 ) {
    unsigned int i;
    for ( i=0; i<args.size(); i++) {
      std::string tempStr=args[i];
      _modelArgs.push_back(tempStr);
    }
  }
}

EvtModelAlias::EvtModelAlias(const EvtModelAlias &copyMe) :

  _aliasName(copyMe._aliasName)
  ,_model(copyMe._model)
{
  if ( copyMe._modelArgs.size() > 0 ) {
    unsigned int i;
    for ( i=0; i<copyMe._modelArgs.size(); i++) _modelArgs.push_back(copyMe._modelArgs[i]);
  }
}

EvtModelAlias EvtModelAlias::operator=(const EvtModelAlias &copyMe) {

  _aliasName=copyMe._aliasName;
  _model=copyMe._model;

  if ( copyMe._modelArgs.size() > 0 ) {
    unsigned int i;
    for ( i=0; i<copyMe._modelArgs.size(); i++) _modelArgs.push_back(copyMe._modelArgs[i]);
  }
  return *this;
}

std::vector<std::string> EvtModelAlias::getArgList() {
  
  return _modelArgs;
}



