// $Id: ReadFile.cpp,v 1.3 2007-07-31 12:49:53 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// Generators  
// ============================================================================
#include "Generators/F77Utils.h"
// ============================================================================
// LbPythia
// ============================================================================
#include "LbPythia/Pypars.h"
#include "LbPythia/Pythia.h"
#include "LbPythia/PythiaProduction.h"
// ============================================================================
// Local 
// ============================================================================
#include "ReadFile.h"
// ============================================================================
/** @file
 *  Implementation file for base class LbPythia::ReadFile
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-06-29
 */
// ============================================================================
// standard contructor
// ============================================================================
LbPythia::ReadFile::ReadFile
( const std::string& type,
  const std::string& name,
  const IInterface* parent ) 
  : PythiaProduction ( type , name , parent ) 
// the name of LHE input file  (no DEFAULT value!)
  , m_file ("<UNCPECIFIED-INPUT-FILENAME>") ///< no default value!
{
  // Do not use TEMPORARLY the third argument to allow to use it in Gauss v25rX
  declareProperty 
    ( "InputFile" , 
      m_file      , 
      "the name of input file") ;
  ///
  PythiaProduction::m_frame  = "USER" ;
  PythiaProduction::m_beam   = " " ;
  PythiaProduction::m_target = " " ;
  PythiaProduction::m_variableEnergy = false ;
  
  PythiaProduction::m_frame  = "USER" ;
  PythiaProduction::m_beam   = " " ;
  PythiaProduction::m_target = " " ;
  PythiaProduction::m_win    = 0.     ;
  
  //
  // switch off the processed, switched on in base class 
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
// destructor 
// ============================================================================
LbPythia::ReadFile::~ReadFile(){}
// ============================================================================
// initialization
// ============================================================================
StatusCode LbPythia::ReadFile::initialize () 
{ 
  // check input file name 
  if ( file().empty() ) { return Error("Input file name is not specified!") ; }
  return PythiaProduction::initialize () ; 
}
// ============================================================================
// finalization 
// ============================================================================
StatusCode LbPythia::ReadFile::finalize () 
{ return PythiaProduction::finalize   () ; }
// ============================================================================
// Read the event from the file @see IProductionTool::generateEvent
// ============================================================================
StatusCode LbPythia::ReadFile::generateEvent 
( HepMC::GenEvent*    event     , 
  LHCb::GenCollision* collision ) 
{
  debug () << "LbPythia::ReadFile::generateEvent" << endreq ;
  //
  StatusCode sc = PythiaProduction::generateEvent ( event , collision ) ;
  //
  if ( sc.isFailure() ) 
  { return Error ( "Error from PythiaProduction::generateEvent" , sc ) ; }
  if ( 0 != Pythia::pypars().msti(51) ) 
  {
    Pythia::PyGive("MSTI(51)=");
    return Warning ("MSTI(51)!=0 : No more event in the input file?");  
  }
  return StatusCode::SUCCESS ;
}
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
