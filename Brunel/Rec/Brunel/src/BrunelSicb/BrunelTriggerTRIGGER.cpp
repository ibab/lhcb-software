// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Rec/Brunel/src/BrunelSicb/BrunelTriggerTRIGGER.cpp,v 1.1.1.1 2001-03-14 16:06:47 cattaneb Exp $


// Include files
#include "BrunelTriggerTRIGGER.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

//------------------------------------------------------------------------------
//
// Implementation of class :  BrunelTriggerTRIGGER
//
// Calls SICB TRIGGER steering routines.
// Currently calls dummy initialisation routine DUMINIT 
//
// Author:      Marco Cattaneo
// Created:     15th February 2000
//------------------------------------------------------------------------------



// IMPORTANT: global declarations for factory.
static const AlgFactory<BrunelTriggerTRIGGER>  s_factory;
const IAlgFactory& BrunelTriggerTRIGGERFactory = s_factory;


extern "C" {
#ifdef WIN32
  void __stdcall DUMINIT( void );
  void __stdcall TRIGGER( void );
  void __stdcall TRLAST( void );
#else
  void duminit_( void );
  void trigger_( void );
  void trlast_( void );
#define DUMINIT duminit_
#define TRIGGER trigger_
#define TRLAST trlast_
#endif
}


//#############################################################################
// ---------------------------------------------------------------------------
// Routine: Constructor.
// Purpose:
// ---------------------------------------------------------------------------
//

BrunelTriggerTRIGGER::BrunelTriggerTRIGGER( const std::string& name, 
                                    ISvcLocator* pSvcLocator )
  : Algorithm( name, pSvcLocator )
{
}

//#############################################################################
// ---------------------------------------------------------------------------
// Routine: Destructor.
// Purpose:
// ---------------------------------------------------------------------------
//

BrunelTriggerTRIGGER::~BrunelTriggerTRIGGER()
{ 
}

//#############################################################################
// ---------------------------------------------------------------------------
// Routine: create.
// Purpose: Necessary to instantiate algorithm from factory.
// ---------------------------------------------------------------------------
//

IAlgorithm* BrunelTriggerTRIGGER::create( const std::string& name, ISvcLocator* loc )
{
  IAlgorithm *alg = new BrunelTriggerTRIGGER( name, loc );
  return alg;
}

//#############################################################################
// ---------------------------------------------------------------------------
// Routine: initialize.
// Purpose: This is called once to perform any initialisation tasks.
//          It should call base class initialisation and create any sub
//          algorithms.
// ---------------------------------------------------------------------------
//

StatusCode BrunelTriggerTRIGGER::initialize() 
{
  DUMINIT();

  MsgStream log( messageService(), name() );
  log << MSG::INFO << "Initialization completed successfully" << endreq;
  
  // Return status code.
  return StatusCode::SUCCESS;
}

//#############################################################################
// ---------------------------------------------------------------------------
// Routine: execute
// Purpose: This is called to run the algorithm
//          It should also execute any sub-algorithms.
// ---------------------------------------------------------------------------
//

StatusCode BrunelTriggerTRIGGER::execute()
{
  TRIGGER();
  MsgStream log( messageService(), name() );
  log << MSG::DEBUG << "Event analysis completed" << endreq;
  return StatusCode::SUCCESS;
}

//#############################################################################
// ---------------------------------------------------------------------------
// Routine: finalize
// Purpose: This is called once at the end of the job
//          It should finalize and release any sub-algorithms.
// ---------------------------------------------------------------------------
//

StatusCode BrunelTriggerTRIGGER::finalize()
{
  TRLAST();
  MsgStream log( messageService(), name() );
  log << MSG::DEBUG << "Finalisation completed" << endreq;
  return StatusCode::SUCCESS;
}
