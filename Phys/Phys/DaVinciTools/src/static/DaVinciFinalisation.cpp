// $Id: DaVinciFinalisation.cpp,v 1.2 2001-07-16 15:16:18 gcorti Exp $
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
  void __stdcall SUINIT( void );
  void __stdcall SUANAL( void );
  void __stdcall SULAST( void );
#else
  void suinit_( void ); 
  void suanal_( void ); 
  void sulast_( void ); 
#define SUINIT   suinit_
#define SUANAL   suanal_
#define SULAST   sulast_
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

  // SICB user analysis (dummy by default)
  SUINIT();
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode DaVinciFinalisation::execute() {

  // SICB user analysis (dummy by default)
  SUANAL();

  MsgStream log( messageService(), name() );
  log << MSG::DEBUG << "Event by event finalisation completed" << endreq;
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode DaVinciFinalisation::finalize() {

  // SICB user analysis (dummy by default)
  SULAST();

  MsgStream log( messageService(), name() );
  log << MSG::DEBUG << "SICB finalisation completed" << endreq;

  return StatusCode::SUCCESS;
}
