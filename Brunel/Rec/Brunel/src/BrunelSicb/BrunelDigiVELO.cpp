// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Rec/Brunel/src/BrunelSicb/BrunelDigiVELO.cpp,v 1.1.1.1 2001-03-14 16:06:46 cattaneb Exp $


// Include files
#include "BrunelDigiVELO.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

//------------------------------------------------------------------------------
//
// Implementation of class :  BrunelDigiVELO
//
// Calls SICB digitisation routine VSDIGI.
// Currently calls dummy initialisation and finalisation routines DUMINIT, DUMANAL 
//
// Author:      Marco Cattaneo
// Created:     15th February 2000
//------------------------------------------------------------------------------


// IMPORTANT: global declarations for factory.
static const AlgFactory<BrunelDigiVELO>  s_factory;
const IAlgFactory& BrunelDigiVELOFactory = s_factory;


extern "C" {
#ifdef WIN32
  void __stdcall DUMINIT( void );
  void __stdcall VSDIGI( void );
  void __stdcall DUMLAST( void );
#else
  void duminit_( void );
  void vsdigi_( void );
  void dumlast_( void );
#define DUMINIT duminit_
#define VSDIGI  vsdigi_
#define DUMLAST dumlast_
#endif
}


//#############################################################################
// ---------------------------------------------------------------------------
// Routine: Constructor.
// Purpose:
// ---------------------------------------------------------------------------
//

BrunelDigiVELO::BrunelDigiVELO( const std::string& name, 
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

BrunelDigiVELO::~BrunelDigiVELO()
{ 
}

//#############################################################################
// ---------------------------------------------------------------------------
// Routine: create.
// Purpose: Necessary to instantiate algorithm from factory.
// ---------------------------------------------------------------------------
//

IAlgorithm* BrunelDigiVELO::create( const std::string& name, ISvcLocator* loc )
{
  IAlgorithm *alg = new BrunelDigiVELO( name, loc );
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

StatusCode BrunelDigiVELO::initialize() 
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

StatusCode BrunelDigiVELO::execute()
{
  VSDIGI();
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

StatusCode BrunelDigiVELO::finalize()
{
  DUMLAST();
  MsgStream log( messageService(), name() );
  log << MSG::DEBUG << "Finalisation completed" << endreq;
  return StatusCode::SUCCESS;
}
