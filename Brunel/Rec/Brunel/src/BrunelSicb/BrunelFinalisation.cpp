// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Rec/Brunel/src/BrunelSicb/BrunelFinalisation.cpp,v 1.2 2001-06-06 16:54:10 cattaneb Exp $

// Include files
#include "BrunelFinalisation.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

//-----------------------------------------------------------------------------
//
// Implementation of class :  BrunelFinalisation
//
// Author:      Marco Cattaneo
// Created:     15th February 2000
// Modified: MC 2001-06-01 - Add call to DROPBANKS
//-----------------------------------------------------------------------------
extern "C" {
#ifdef WIN32
  void __stdcall DROPBANKS( void );
  void __stdcall RECEVOUT( void );
  void __stdcall SUANAL( void );
  void __stdcall UGLAST( void );
#else
  void dropbanks_( void );
  void recevout_( void );
  void suanal_( void ); 
  void uglast_( void ); 
#define DROPBANKS dropbanks_
#define RECEVOUT  recevout_
#define SUANAL    suanal_
#define UGLAST    uglast_
#endif
}

static const AlgFactory<BrunelFinalisation>  s_factory;
const IAlgFactory& BrunelFinalisationFactory = s_factory;

BrunelFinalisation::BrunelFinalisation( const std::string& name, 
                                        ISvcLocator* pSvcLocator )
                   : Algorithm( name, pSvcLocator )                { 
  declareProperty( "DropDSTBanks", m_dropBanks = true );
}

BrunelFinalisation::~BrunelFinalisation() { }

StatusCode BrunelFinalisation::initialize() { return StatusCode::SUCCESS; }

StatusCode BrunelFinalisation::execute() {

// SICB user analysis (dummy by default)
  SUANAL();
// Drop unwanted banks from DST
  if (m_dropBanks) DROPBANKS();
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
