// $Id: BrunelDigiMUON.cpp,v 1.2 2001-06-25 08:24:57 cattaneb Exp $


// Include files
#include "BrunelDigiMUON.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

//-----------------------------------------------------------------------------
//
// Implementation of class :  BrunelDigiMUON
//
// Calls SICB digitisation routines to add muon background and do digitisation
//
// Author:      Marco Cattaneo
// Created:     15th February 2000
// Modified:    25th June 2001     to add muon background
//-----------------------------------------------------------------------------


// Global declarations for factory.
static const AlgFactory<BrunelDigiMUON>  s_factory;
const IAlgFactory& BrunelDigiMUONFactory = s_factory;

extern "C" {
#ifdef WIN32
  void __stdcall MURMBG( void );
  void __stdcall MUBACKGROUND( void );
  void __stdcall MUDIGI( void );
#else
  void murmbg_( void );
  void mubackground_( void );
  void mudigi_( void );
#define MURMBG        murmbg_
#define MUBACKGROUND  mubackground_
#define MUDIGI        mudigi_
#endif
}

//----------------------------------------------------------------------------
BrunelDigiMUON::BrunelDigiMUON( const std::string& name, 
                                ISvcLocator* pSvcLocator )
                              : Algorithm( name, pSvcLocator )
//----------------------------------------------------------------------------
{
  declareProperty( "addBkg", m_addBkg = true );
}

//----------------------------------------------------------------------------
BrunelDigiMUON::~BrunelDigiMUON() { }
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
StatusCode BrunelDigiMUON::initialize() 
//----------------------------------------------------------------------------
{
  MsgStream log( msgSvc(), name() );

  if( false == m_addBkg ) {
    log << MSG::WARNING << "Muon background will NOT be added" << endreq;
  }
  log << MSG::INFO << "Initialization completed successfully" << endreq;
  
  // Return status code.
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------
StatusCode BrunelDigiMUON::execute()
//----------------------------------------------------------------------------
{
  if( true == m_addBkg ) {
    MURMBG();
    MUBACKGROUND();
  }
  
  MUDIGI();
  MsgStream log( msgSvc(), name() );
  log << MSG::DEBUG << "Event analysis completed" << endreq;
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------
StatusCode BrunelDigiMUON::finalize()
//----------------------------------------------------------------------------
{
  MsgStream log( msgSvc(), name() );
  log << MSG::DEBUG << "Finalisation completed" << endreq;
  return StatusCode::SUCCESS;
}
