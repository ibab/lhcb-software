// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Rec/Brunel/src/ObsoleteSicb/BrunelDigiECAL.cpp,v 1.1 2001-05-17 13:40:20 cattaneb Exp $


// Include files
#include "BrunelDigiECAL.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

//------------------------------------------------------------------------------
//
// Implementation of class :  BrunelDigiECAL
//
// Calls SICB digitisation routine ECDIGI.
// Currently calls dummy initialisation and finalisation routines DUMINIT, DUMANAL 
//
// Author:      Marco Cattaneo
// Created:     15th February 2000
//------------------------------------------------------------------------------


// IMPORTANT: global declarations for factory.
static const AlgFactory<BrunelDigiECAL>  s_factory;
const IAlgFactory& BrunelDigiECALFactory = s_factory;


extern "C" {
#ifdef WIN32
  void __stdcall DUMINIT( void );
  void __stdcall ECDIGI( void );
  void __stdcall DUMLAST( void );
#else
  void duminit_( void );
  void ecdigi_( void );
  void dumlast_( void );
#define DUMINIT duminit_
#define ECDIGI  ecdigi_
#define DUMLAST dumlast_
#endif
}


//#############################################################################
// ---------------------------------------------------------------------------
// Routine: Constructor.
// Purpose:
// ---------------------------------------------------------------------------
//

BrunelDigiECAL::BrunelDigiECAL( const std::string& name, 
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

BrunelDigiECAL::~BrunelDigiECAL()
{ 
}

//#############################################################################
// ---------------------------------------------------------------------------
// Routine: create.
// Purpose: Necessary to instantiate algorithm from factory.
// ---------------------------------------------------------------------------
//

IAlgorithm* BrunelDigiECAL::create( const std::string& name, ISvcLocator* loc )
{
  IAlgorithm *alg = new BrunelDigiECAL( name, loc );
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

StatusCode BrunelDigiECAL::initialize() 
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

StatusCode BrunelDigiECAL::execute()
{
  ECDIGI();
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

StatusCode BrunelDigiECAL::finalize()
{
  DUMLAST();
  MsgStream log( messageService(), name() );
  log << MSG::DEBUG << "Finalisation completed" << endreq;
  return StatusCode::SUCCESS;
}
