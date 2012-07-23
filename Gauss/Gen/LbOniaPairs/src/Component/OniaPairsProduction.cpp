// $Id: $
// ============================================================================
// local LbOniaPairs
#include "LbOniaPairs/OniaPairs.h"

// from GaudiKernel
#include "GaudiKernel/DeclareFactoryEntries.h"

// from Event
#include "Event/GenCollision.h"

// Generators 
#include "Generators/IBeamTool.h"

// LbPythia 
#include "LbPythia/Pythia.h"
// ============================================================================
// Local 
#include "OniaPairsProduction.h"
// ============================================================================
// Standard constructor, initializes variables
// ============================================================================
  OniaPairsProduction::OniaPairsProduction
( const std::string& type   , 
  const std::string& name   , 
  const IInterface*  parent ) 
  : PythiaProduction ( type , name , parent ) 
{
  
  declareInterface< IProductionTool >( this ) ;
  //
  declareProperty ( "Ecm"     , m_ecm     = 7e3 ) ;
  declareProperty ( "Psi1S1S" , m_psi1S1S = 0.  ) ;
  declareProperty ( "Psi1S2S" , m_psi1S2S = 0.  ) ;
  declareProperty ( "Psi2S2S" , m_psi2S2S = 0.  ) ;
  
  declareProperty ( "Ups1S1S" , m_ups1S1S = 0.  ) ;
  declareProperty ( "Ups1S2S" , m_ups1S2S = 0.  ) ;
  declareProperty ( "Ups1S3S" , m_ups1S3S = 0.  ) ;
  declareProperty ( "Ups2S2S" , m_ups2S2S = 0.  ) ;
  declareProperty ( "Ups2S3S" , m_ups2S3S = 0.  ) ;
  declareProperty ( "Ups3S3S" , m_ups3S3S = 0.  ) ;
  
  
  m_defaultSettings.push_back ( "pysubs ckin 41 12.0" ) ;
  m_defaultSettings.push_back ( "pypars mstp 2 1" ) ;
  m_defaultSettings.push_back ( "pypars mstp 33 0" ) ;
  m_defaultSettings.push_back ( "pypars mstp 81 1" ) ;
  m_defaultSettings.push_back ( "pypars mstp 82 4" ) ;
  m_defaultSettings.push_back ( "pypars mstp 52 2" ) ;
  m_defaultSettings.push_back ( "pypars mstp 51 10042" ) ;
  m_defaultSettings.push_back ( "pypars mstp 142 0" ) ;
  m_defaultSettings.push_back ( "pypars parp 67 4" ) ;
  m_defaultSettings.push_back ( "pypars parp 82 2.0" ) ;
  m_defaultSettings.push_back ( "pypars parp 89 1800" ) ;
  m_defaultSettings.push_back ( "pypars parp 90 0.16" ) ;
  m_defaultSettings.push_back ( "pypars parp 85 0.9" ) ;
  m_defaultSettings.push_back ( "pypars parp 86 0.95" ) ;
  m_defaultSettings.push_back ( "pypars parp 91 2.0" ) ;
  m_defaultSettings.push_back ( "pypars parp 149 0.02" ) ;
  m_defaultSettings.push_back ( "pypars parp 150 0.085" ) ;
  m_defaultSettings.push_back ( "pydat1 parj 11 0.5" ) ;
  m_defaultSettings.push_back ( "pydat1 parj 12 0.6" ) ;
  m_defaultSettings.push_back ( "pydat1 parj 13 0.75" ) ;
  m_defaultSettings.push_back ( "pydat1 parj 14 0.0" ) ;
  m_defaultSettings.push_back ( "pydat1 parj 15 0.0" ) ;
  m_defaultSettings.push_back ( "pydat1 parj 16 0.0" ) ;
  m_defaultSettings.push_back ( "pydat1 parj 17 0.0" ) ;
  m_defaultSettings.push_back ( "pydat1 mstj 26 0" ) ;
  m_defaultSettings.push_back ( "pydat1 parj 33 0.4" ) ;

  m_defaultSettings.push_back ( "pysubs msub 11 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 12 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 13 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 28 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 53 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 68 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 91 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 92 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 93 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 94 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 95 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 421 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 422 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 423 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 424 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 425 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 426 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 427 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 428 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 429 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 430 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 431 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 432 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 433 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 434 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 435 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 436 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 437 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 438 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 439 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 461 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 462 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 463 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 464 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 465 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 466 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 467 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 468 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 469 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 470 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 471 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 472 0" ) ;
  m_defaultSettings.push_back (  "pysubs msub 473 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 474 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 475 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 476 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 477 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 478 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 479 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 480 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 481 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 482 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 483 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 484 0" ) ;
  m_defaultSettings.push_back ( "pysubs msub 485 0" ) ;
  
  // postpone the PyInit initialization 
  //  setProperty ( "PostponePyInit" , true ) ;
  //  setProperty ( "ValidateHEPEVT" , true ) ;
 
} 
// =============================================================================
// initialize the production tool 
// =============================================================================
StatusCode OniaPairsProduction::initialize() 
{  
  /*  m_defaultSettings.clear() ;    
      m_commandVector.clear();
      
      PythiaProduction::m_defaultSettings.clear() ;    
      PythiaProduction::m_commandVector.clear() ;
  */
  
  Pythia::pydat1().mstu( 12 ) = 12345 ;
  Pythia::pydat1().mstu( 13 ) = 0 ;
  Pythia::pydat1().mstu( 25 ) = 0 ;
  Pythia::pypars().mstp( 122 ) = 0 ;
  m_initializationListingLevel = -1 ;
  
  /*
    Pythia::pydat1().mstu( 12 ) = 1 ;
    Pythia::pydat1().mstu( 13 ) = 1 ;
    Pythia::pydat1().mstu( 25 ) = 1 ;
    Pythia::pypars().mstp( 122 ) = 1 ;
  */

  OniaPairs::SetPar ( "ECM", m_ecm ) ;
  
  OniaPairs::SetPar ( "PSI1S1S", m_psi1S1S ) ;
  OniaPairs::SetPar ( "PSI1S2S", m_psi1S2S ) ;
  OniaPairs::SetPar ( "PSI2S2S", m_psi2S2S ) ;

  OniaPairs::SetPar ( "UPS1S1S", m_ups1S1S ) ;
  OniaPairs::SetPar ( "UPS1S2S", m_ups1S2S ) ;
  OniaPairs::SetPar ( "UPS1S3S", m_ups1S3S ) ;
  OniaPairs::SetPar ( "UPS2S2S", m_ups2S2S ) ;
  OniaPairs::SetPar ( "UPS2S3S", m_ups2S3S ) ;
  OniaPairs::SetPar ( "UPS3S3S", m_ups3S3S ) ;  
  
  // my user process number
  m_userProcess = 6 ;
  
  // set user process in pythia
  m_frame  = "USER" ;
  m_beam   = " "    ;
  m_target = " "    ;
  m_win    = 0.     ;
  
  // Initialize Pythia Production
  StatusCode sc = PythiaProduction::initialize( ) ;
  if ( sc.isFailure() ) return sc ;
  
  return StatusCode::SUCCESS ;
}
// =============================================================================
// finalize
// =============================================================================
StatusCode OniaPairsProduction::finalize() 
{
  
  // Finalize Pythia Production
  StatusCode sc = PythiaProduction::finalize( ) ;
  if ( sc.isFailure() ) return sc ;
  
  return StatusCode::SUCCESS ;
}
// =============================================================================
//   Function called to generate one event with Pythia
// =============================================================================
StatusCode OniaPairsProduction::generateEvent( HepMC::GenEvent * theEvent , 
					       LHCb::GenCollision * theCollision )
{
  
  // Generate event
  StatusCode sc = PythiaProduction::generateEvent(theEvent, theCollision) ;
  if ( sc.isFailure() ) return sc ;
  
  //Pythia::PyList(2);
  
  return StatusCode::SUCCESS ;
}
// =============================================================================
/// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( OniaPairsProduction )
// =============================================================================
// The END 
// =============================================================================

