// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Rec/Brunel/src/BrunelSicb/BrunelInitialisation.cpp,v 1.1.1.1 2001-03-14 16:06:46 cattaneb Exp $
#define BRUNEL_INITIALISATION_CPP

// Include files
#include "BrunelInitialisation.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

//------------------------------------------------------------------------------
//
// Implementation of class :  BrunelInitialisation
//
// Author:      Marco Cattaneo
// Created:     15th February 2000
//------------------------------------------------------------------------------
extern "C" {
#ifdef WIN32
  void __stdcall BRUNELINIT( void );
#else
  void brunelinit_( void );
#define BRUNELINIT brunelinit_
#endif
}

static const AlgFactory<BrunelInitialisation>  s_factory;
const IAlgFactory& BrunelInitialisationFactory = s_factory;

// Necessary to instantiate factory method
 IAlgorithm* BrunelInitialisation::create(const std::string& name, ISvcLocator* loc)   {
  IAlgorithm *alg = new BrunelInitialisation( name,loc);
  return alg;
}

BrunelInitialisation::BrunelInitialisation( const std::string& name, ISvcLocator* pSvcLocator )
 : Algorithm( name, pSvcLocator ) {
}

BrunelInitialisation::~BrunelInitialisation() {
}

StatusCode BrunelInitialisation::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode BrunelInitialisation::execute() {

	// Hack to initialise SICB stuff
  BRUNELINIT();

  return StatusCode::SUCCESS;
}

StatusCode BrunelInitialisation::finalize() {
  return StatusCode::SUCCESS;
}
