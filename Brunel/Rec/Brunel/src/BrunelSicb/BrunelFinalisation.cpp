// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Rec/Brunel/src/BrunelSicb/BrunelFinalisation.cpp,v 1.1.1.1 2001-03-14 16:06:46 cattaneb Exp $
#define BRUNEL_FINALISATION_CPP

// Include files
#include "BrunelFinalisation.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

//------------------------------------------------------------------------------
//
// Implementation of class :  BrunelFinalisation
//
// Author:      Marco Cattaneo
// Created:     15th February 2000
//------------------------------------------------------------------------------
extern "C" {
#ifdef WIN32
  void __stdcall RECEVOUT( void );
  void __stdcall SUANAL( void );
  void __stdcall UGLAST( void );
#else
  void recevout_( void );
  void suanal_( void ); 
  void uglast_( void ); 
#define RECEVOUT recevout_
#define SUANAL   suanal_
#define UGLAST   uglast_
#endif
}

static const AlgFactory<BrunelFinalisation>  s_factory;
const IAlgFactory& BrunelFinalisationFactory = s_factory;

// Necessary to instantiate factory method
 IAlgorithm* BrunelFinalisation::create(const std::string& name, ISvcLocator* loc)   {
  IAlgorithm *alg = new BrunelFinalisation( name,loc);
  return alg;
}

BrunelFinalisation::BrunelFinalisation( const std::string& name, ISvcLocator* pSvcLocator )
 : Algorithm( name, pSvcLocator ) {
}

BrunelFinalisation::~BrunelFinalisation() {
}

StatusCode BrunelFinalisation::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode BrunelFinalisation::execute() {

	// SICB user analysis (dummy by default)
	SUANAL();
	// Write out SICBDST
	RECEVOUT();

    MsgStream log( messageService(), name() );
    log << MSG::DEBUG << "Event finalisation completed" << endreq;
    return StatusCode::SUCCESS;
}

StatusCode BrunelFinalisation::finalize() {

	// SICB finalisation
	UGLAST();

    MsgStream log( messageService(), name() );
    log << MSG::DEBUG << "SICB finalisation completed" << endreq;
    return StatusCode::SUCCESS;
}