// $Id: $
// ============================================================================
// local LbOniaPairs
#include "LbOniaPairs/OniaPairsProduction.h"
#include "LbOniaPairs/OniaPairs.h"

// from GaudiKernel
#include "GaudiKernel/DeclareFactoryEntries.h"

// from Event
#include "Event/GenCollision.h"

// Generators 
//#include "Generators/IBeamTool.h"

// LbPythia 
#include "LbPythia/Pythia.h"

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

  declareProperty ( "Ecm"     , m_ecm     = 0. ) ;
  declareProperty ( "Psi1S1S" , m_psi1S1S = 0. ) ;
  declareProperty ( "Psi1S2S" , m_psi1S2S = 0. ) ;
  declareProperty ( "Psi2S2S" , m_psi2S2S = 0. ) ;
  
  declareProperty ( "Ups1S1S" , m_ups1S1S = 0. ) ;
  declareProperty ( "Ups1S2S" , m_ups1S2S = 0. ) ;
  declareProperty ( "Ups1S3S" , m_ups1S3S = 0. ) ;
  declareProperty ( "Ups2S2S" , m_ups2S2S = 0. ) ;
  declareProperty ( "Ups2S3S" , m_ups2S3S = 0. ) ;
  declareProperty ( "Ups3S3S" , m_ups3S3S = 0. ) ;

  declareProperty ( "ScaleFactorInAlpS"    , m_ScfAlpS = 1. ) ;
  declareProperty ( "ScaleFactorInPDF"     , m_ScfPDF  = 1. ) ;
  declareProperty ( "ScaleFactorInShowers" , m_ScfShwr = 1. ) ;

  declareProperty ( "MaxWeightMultiplier" , m_MaxWghtMult = 1. ) ;

  declareProperty( "PyCommVec" , m_PyCommVec ) ;
  
  // Use raw Pythia, except ...
  m_PyDefComm.clear() ;
  // PDFs - select CTEQ6l1 
  //m_PyDefComm.push_back( "pypars mstp 52 2" ) ;
  //m_PyDefComm.push_back( "pypars mstp 51 10042" ) ;
  // Tune - Perugia 2012 - already with CTEQ6L1
  m_PyDefComm.push_back( "pypars mstp 5 370" ) ;
  // event record - store decay products in main section
  m_PyDefComm.push_back( "pypars mstp 128 2" ) ;

} 
// =============================================================================
// initialize the production tool 
// =============================================================================
StatusCode OniaPairsProduction::initialize() 
{  

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
  
  OniaPairs::SetPar ( "ScfAlpS", m_ScfAlpS ) ;
  OniaPairs::SetPar ( "ScfPDF" , m_ScfPDF  ) ;
  OniaPairs::SetPar ( "ScfShwr", m_ScfShwr ) ;  

  OniaPairs::SetPar ( "MaxWghtMult", m_MaxWghtMult ) ;  

  // my user process number
  m_userProcess = 6 ;
  
  // set user process in pythia
  m_frame  = "USER" ;
  m_beam   = "p"    ;
  m_target = "p"    ;
  m_win    = m_ecm  ;

  // Discard default settings from LbPythia
  m_defaultSettings.clear() ;
  // Do not configure Pythia from behind
  m_commandVector.clear() ;
  
  // Configure Pythia from this tool only
  m_defaultSettings = m_PyDefComm;
  m_commandVector   = m_PyCommVec;

  StatusCode sc = PythiaProduction::initialize( ) ;
  if ( sc.isFailure() ) return sc ;
  
  return StatusCode::SUCCESS ;
}
// =============================================================================
// finalize
// =============================================================================
StatusCode OniaPairsProduction::finalize() 
{
  
  OniaPairs::PrintCSTable () ;

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
    
  return StatusCode::SUCCESS ;
}
// =============================================================================
/// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( OniaPairsProduction )
// =============================================================================
// The END 
// =============================================================================

