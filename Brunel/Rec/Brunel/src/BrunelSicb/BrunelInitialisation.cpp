// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Rec/Brunel/src/BrunelSicb/BrunelInitialisation.cpp,v 1.2 2001-06-06 16:54:10 cattaneb Exp $

// Include files
#include "BrunelInitialisation.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "SicbCnv/SicbFortran.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "LHCbEvent/MCEvent.h"

//----------------------------------------------------------------------------
//
// Implementation of class :  BrunelInitialisation
//
// Author:      Marco Cattaneo
// Created:     15th February 2000
// Modified: MC 2001/05/31 - to fix NPILEUP for RAWH2 data
//-----------------------------------------------------------------------------
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
 IAlgorithm* BrunelInitialisation::create(const std::string& name, 
                                          ISvcLocator* loc)   {
  IAlgorithm *alg = new BrunelInitialisation( name,loc);
  return alg;
}

BrunelInitialisation::BrunelInitialisation( const std::string& name, 
                                            ISvcLocator* pSvcLocator )
                     : Algorithm( name, pSvcLocator ) { }

BrunelInitialisation::~BrunelInitialisation() { }

StatusCode BrunelInitialisation::initialize() { return StatusCode::SUCCESS; }

StatusCode BrunelInitialisation::execute() {

  MsgStream log( msgSvc(), name() );

  // Initialise SICB stuff
  BRUNELINIT();

  // Update the common block variable NPILEUP with number of piled up events.
  // Necessary for generator level pileup.
  // May break some SICB DST code if pileup has been done in Brunel
  SmartDataPtr<MCEvent> evt( eventSvc(), "/Event/MC" );
  if( evt ) {
    int npile = evt->pileUp();
    SicbFortran::SetNPileUp( npile );
    log << MSG::DEBUG << "npile = " << npile << endreq;
  }
  return StatusCode::SUCCESS;
}

StatusCode BrunelInitialisation::finalize() { return StatusCode::SUCCESS; }
