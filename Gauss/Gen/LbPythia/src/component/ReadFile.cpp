// $Id: ReadFile.cpp,v 1.1 2007-07-06 08:10:22 ibelyaev Exp $
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
  , m_file () ///< no default value!
{
  declareProperty ( "InputFile" , m_file , "The name of input file with partonic events") ;
  ///
  PythiaProduction::m_frame  = "USER" ;
  PythiaProduction::m_beam   = " " ;
  PythiaProduction::m_target = " " ;
  PythiaProduction::m_variableEnergy = false ;
  //
  // switch off the processed, switched on in base class 
  addPygive ( "MSUB(11)=0;MSUB(12)=0;MSUB(13)=0") ;
  addPygive ( "MSUB(28)=0;MSUB(53)=0;MSUB(68)=0") ;
  addPygive ( "MSUB(86)=0;MSUB(87)=0;MSUB(88)=0") ;
  addPygive ( "MSUB(89)=0;MSUB(90)=");
  addPygive ( "MSUB(91)=0;MSUB(92)=0;MSUB(93)=0") ;
  addPygive ( "MSUB(94)=0;MSUB(95)=0;MSUB(96)=0") ;
  addPygive ( "MSUB(106)=0" ) ;
  //  
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
StatusCode LbPythia::ReadFile::finalize   () 
{ return PythiaProduction::finalize   () ; }
// ============================================================================
// Read the event from the file @see IProductionTool::generateEvent
// ============================================================================
StatusCode LbPythia::ReadFile::generateEvent 
( HepMC::GenEvent*    event     , 
  LHCb::GenCollision* collision ) 
{
  StatusCode sc = PythiaProduction::generateEvent ( event , collision ) ;
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
