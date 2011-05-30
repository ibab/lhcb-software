// $Id: $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
// ============================================================================
// Local 
// ============================================================================
#include "OniaPairsProduction.h"
// ============================================================================
/** @file
 *  Implementation file for class OniaPairsProduction 
 *
 *  @author Alexey Nososelov Alexey.Novoselov@cern.ch
 *  @date 2011-05-30
 *  
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ==========================================================================--
// Standard constructor, initializes variables
// ==========================================================================--
OniaPairsProduction::OniaPairsProduction
( const std::string& type   , 
  const std::string& name   , 
  const IInterface*  parent ) 
  : PythiaProduction ( type , name , parent ) 
  , m_ecm ( 7 * Gaudi::Units::TeV ) 
{
  //
  declareProperty ( "EnergyCM" , m_ecm      , "CM-energy" ) ;
  // 
  // postpone the PyInit initialization 
  //
  setProperty ( "PostponePyInit" , true ) ;
  setProperty ( "ValidateHEPEVT" , true ) ;
  //
  // PythiaProduction::m_defaultSettings.clear() ;    
  //
  m_commandVector.push_back ( "pysubs msub 11 0"  ) ;  
  m_commandVector.push_back ( "pysubs msub 12 0"  ) ;
  m_commandVector.push_back ( "pysubs msub 13 0"  ) ;
  m_commandVector.push_back ( "pysubs msub 28 0"  ) ;
  m_commandVector.push_back ( "pysubs msub 53 0"  ) ;
  m_commandVector.push_back ( "pysubs msub 68 0"  ) ;
  m_commandVector.push_back ( "pysubs msub 91 0"  ) ;
  m_commandVector.push_back ( "pysubs msub 92 0"  ) ;
  m_commandVector.push_back ( "pysubs msub 93 0"  ) ;
  m_commandVector.push_back ( "pysubs msub 94 0"  ) ;
  m_commandVector.push_back ( "pysubs msub 95 0"  ) ;
  m_commandVector.push_back ( "pysubs msub 421 0" ) ;
  m_commandVector.push_back ( "pysubs msub 422 0" ) ;  
  m_commandVector.push_back ( "pysubs msub 423 0" ) ;
  m_commandVector.push_back ( "pysubs msub 424 0" ) ;
  m_commandVector.push_back ( "pysubs msub 425 0" ) ;
  m_commandVector.push_back ( "pysubs msub 426 0" ) ;
  m_commandVector.push_back ( "pysubs msub 427 0" ) ;
  m_commandVector.push_back ( "pysubs msub 428 0" ) ;
  m_commandVector.push_back ( "pysubs msub 429 0" ) ;
  m_commandVector.push_back ( "pysubs msub 430 0" ) ;
  m_commandVector.push_back ( "pysubs msub 431 0" ) ;
  m_commandVector.push_back ( "pysubs msub 432 0" ) ;
  m_commandVector.push_back ( "pysubs msub 433 0" ) ;
  m_commandVector.push_back ( "pysubs msub 434 0" ) ;
  m_commandVector.push_back ( "pysubs msub 435 0" ) ;
  m_commandVector.push_back ( "pysubs msub 436 0" ) ;
  m_commandVector.push_back ( "pysubs msub 437 0" ) ;
  m_commandVector.push_back ( "pysubs msub 438 0" ) ;
  m_commandVector.push_back ( "pysubs msub 439 0" ) ;
  m_commandVector.push_back ( "pysubs msub 461 0" ) ;
  m_commandVector.push_back ( "pysubs msub 462 0" ) ;
  m_commandVector.push_back ( "pysubs msub 463 0" ) ;
  m_commandVector.push_back ( "pysubs msub 464 0" ) ;
  m_commandVector.push_back ( "pysubs msub 465 0" ) ;
  m_commandVector.push_back ( "pysubs msub 466 0" ) ;
  m_commandVector.push_back ( "pysubs msub 467 0" ) ;
  m_commandVector.push_back ( "pysubs msub 468 0" ) ;
  m_commandVector.push_back ( "pysubs msub 469 0" ) ;
  m_commandVector.push_back ( "pysubs msub 470 0" ) ; 
  m_commandVector.push_back ( "pysubs msub 471 0" ) ;
  m_commandVector.push_back ( "pysubs msub 472 0" ) ;
  m_commandVector.push_back ( "pysubs msub 473 0" ) ;
  m_commandVector.push_back ( "pysubs msub 474 0" ) ;
  m_commandVector.push_back ( "pysubs msub 475 0" ) ;
  m_commandVector.push_back ( "pysubs msub 476 0" ) ; 
  m_commandVector.push_back ( "pysubs msub 477 0" ) ;  
  m_commandVector.push_back ( "pysubs msub 478 0" ) ; 
  m_commandVector.push_back ( "pysubs msub 479 0" ) ;
  m_commandVector.push_back ( "pysubs msub 480 0" ) ;
  m_commandVector.push_back ( "pysubs msub 481 0" ) ;
  m_commandVector.push_back ( "pysubs msub 482 0" ) ;
  m_commandVector.push_back ( "pysubs msub 483 0" ) ;
  m_commandVector.push_back ( "pysubs msub 484 0" ) ;
  m_commandVector.push_back ( "pysubs msub 485 0" ) ;
} 
// ============================================================================
// initilize the production tool 
// ============================================================================
StatusCode OniaPairsProduction::initialize() 
{  
  
  /*
    m_pygive.push_back( "mstu(12)=12345" ) ;
    m_pygive.push_back( "mstu(13)=0" ) ;
    m_pygive.push_back( "mstu(25)=0" ) ;
  */
  //
  Pythia::pydat1().mstu (  12 ) = 1 ;
  Pythia::pydat1().mstu (  13 ) = 1 ;
  Pythia::pydat1().mstu (  25 ) = 1 ;
  Pythia::pypars().mstp ( 122 ) = 1 ;
  //
  // set Ecm
  OniaPairs::SetPar ( "ECM", m_ecm / Gaudi::Units::GeV ) ;
  //
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
// ============================================================================
// finalize
// ============================================================================
StatusCode OniaPairsProduction::finalize() 
{ return  PythiaProduction::finalize () ; }
// ============================================================================
// Function called to generate one event with Pythia
// ============================================================================
StatusCode OniaPairsProduction::generateEvent
( HepMC::GenEvent * theEvent , 
  LHCb::GenCollision * theCollision )
{
  // Generate event
  StatusCode sc = PythiaProduction::generateEvent(theEvent, theCollision) ;
  if ( sc.isFailure() ) return sc ;
  //
  return StatusCode::SUCCESS ;
}

// ============================================================================
/// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( OniaPairsProduction ) 
// ============================================================================ 
// The END 
// ============================================================================


