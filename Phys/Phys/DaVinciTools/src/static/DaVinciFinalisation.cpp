// $Id: DaVinciFinalisation.cpp,v 1.1.1.1 2001-07-09 09:28:42 gcorti Exp $
#define DAVINCI_FINALISATION_CPP

// Include files
#include "DaVinciFinalisation.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DaVinciFinalisation
//
// 01/06/2001 : Gloria Corti
//-----------------------------------------------------------------------------

// List of wrapped fortran 
extern "C" {
#ifdef WIN32
  void __stdcall SUANAL( void );
#else
  void suanal_( void ); 
#define SUANAL   suanal_
#endif
}

// Declaration of the Algorithm Factory
static const AlgFactory<DaVinciFinalisation>  s_factory;
const IAlgFactory& DaVinciFinalisationFactory = s_factory;

//=============================================================================
// Standard creator, no variables to initialize
//=============================================================================
DaVinciFinalisation::DaVinciFinalisation( const std::string& name, 
                                          ISvcLocator* pSvcLocator )
  : Algorithm( name, pSvcLocator ) {
}

//=============================================================================
// Standard destructor
//=============================================================================
DaVinciFinalisation::~DaVinciFinalisation() {
}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode DaVinciFinalisation::initialize() {
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode DaVinciFinalisation::execute() {

  // SICB user analysis (dummy by default)
  SUANAL();

  MsgStream log( messageService(), name() );
  log << MSG::DEBUG << "Event finalisation completed" << endreq;
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode DaVinciFinalisation::finalize() {

  MsgStream log( messageService(), name() );
  log << MSG::DEBUG << "SICB finalisation completed" << endreq;

  return StatusCode::SUCCESS;
}
