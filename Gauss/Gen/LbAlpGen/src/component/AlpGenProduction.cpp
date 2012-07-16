// Include files 

// local
#include "AlpGenProduction.h"

// STL 
#include <fstream>

// Boost
#include "boost/filesystem.hpp"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ParticleProperty.h"

// from Event
#include "Event/GenCollision.h"

// local
#include "Generators/StringParse.h"
#include "Generators/IBeamTool.h"
#include "Generators/F77Utils.h"
#include "LbPythia/Pythia.h"
#include "LbAlpGen/AlpGenFunctions.h"

//-----------------------------------------------------------------------------
// Implementation file for class : AlpGenProduction
//
// 
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( AlpGenProduction );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
AlpGenProduction::AlpGenProduction( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : PythiaProduction ( type, name , parent ) {

  declareInterface< IProductionTool >( this ) ;
}

//=============================================================================
// Destructor 
//=============================================================================
AlpGenProduction::~AlpGenProduction( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode AlpGenProduction::initialize( ) {
  generateWeightedEvents() ;
  generateUnweightedEvents() ;

  // For Jet Parton Matching MSTP(143) = 1
  Pythia::pypars().mstp( 143 ) = 1 ;

 //Change the parameter to 5, for AlpGen,....
  m_userProcess = 5 ;

  // User process
  m_frame = "USER" ;
  m_beam = "p+";
  m_target = "p+" ;

  // SET Filename
  boost::filesystem::path alpfile( std::tmpnam( NULL ) ) ;
  if ( boost::filesystem::exists( alpfile ) ) 
    boost::filesystem::remove( alpfile ) ;
  
  std::ofstream g( alpfile.string().c_str() ) ;
  g << "zbb3" << std::endl ;    // label of the output files
  g.close() ;

  F77Utils::open( 89 , alpfile.string() , false ) ;  

  //Initialize of Pythia done here
  StatusCode sc = PythiaProduction::initialize( ) ;

  F77Utils::close( 89 ) ;
  boost::filesystem::remove( alpfile ) ;

  if ( sc.isFailure() ) return sc ;

  return sc ;
}

//=============================================================================
//   Function called to generate one event with Pythia --> AlpGen
//=============================================================================
StatusCode AlpGenProduction::generateEvent( HepMC::GenEvent * theEvent , 
                                            LHCb::GenCollision * theCollision )
{
  StatusCode sc = PythiaProduction::generateEvent(theEvent, theCollision) ;
  if ( sc.isFailure() ) return sc ;

  return StatusCode::SUCCESS ;
}

void AlpGenProduction::generateWeightedEvents( ) {
  // create temporary file to input to ALPGEN
  boost::filesystem::path alpfile( std::tmpnam( NULL ) ) ;
  if ( boost::filesystem::exists( alpfile ) ) 
    boost::filesystem::remove( alpfile ) ;
  
  std::ofstream g( alpfile.string().c_str() ) ;
  g << 1 << std::endl // Generation mode
    << "zbb3" << std::endl          // label of the output files
    << 0 << std::endl              // new grid
    << 10000 << " " << 2 << std::endl // nevents per iteraction and n iterations
    << 100000 << std::endl            // nevents to generate
    << "izdecmode " << 2 << std::endl 
    << "ihvy " << 5 << std::endl 
    << "ebeam " << 3500.0 << std::endl  // beam energy
    << "ndns " << 5 << std::endl // parton density
    << "mb " << 4.7 << std::endl // b mass
    << "ptjmin " << 0.0 << std::endl 
    << "ptbmin " << 3 << std::endl 
    << "ptlmin " << 1. << std::endl 
    << "etalmin " << 1.4 << std::endl 
    << "etajmax " << 10 << std::endl 
    << "etabmax " << 10 << std::endl 
    << "etalmax " << 10. << std::endl 
    << "drjmin " << 0.7 << std::endl 
    << "mllmin " << 5 << std::endl 
    << "drbmin " << 0.1 << std::endl 
    << "drlmin " << 0 << std::endl 
    << "ilep " << 0 << std::endl 
    << "iseed1 " << 12345 << std::endl 
    << "iseed2 " << 67890 << std::endl
    << "iseed3 " << 12345 << std::endl 
    << "iseed4 " << 67890 << std::endl 
    << "izdecmod " << 2 << std::endl 
    << "print " << 2 << std::endl ;
  g.close() ;

  AlpGenFunctions::AlSprc() ;

  F77Utils::open( 5 , alpfile.string() , false ) ;  
  AlpGenFunctions::AlSdef() ;
  F77Utils::close( 5 ) ;

  boost::filesystem::remove( alpfile ) ;

  AlpGenFunctions::AlHset() ;
  AlpGenFunctions::AlInit() ;
  AlpGenFunctions::AlSbkk() ;
  AlpGenFunctions::AlShis() ;
  AlpGenFunctions::AlSgrd() ;
  AlpGenFunctions::AlIgrd() ;
  AlpGenFunctions::AlEgen() ;
  AlpGenFunctions::AlFhis() ;
  AlpGenFunctions::AlFbkk() ;
  
  F77Utils::close( 10 ) ;
  F77Utils::close( 12 ) ;
}

void AlpGenProduction::generateUnweightedEvents( ) {
  // create temporary file to input to ALPGEN
  boost::filesystem::path alpfile( std::tmpnam( NULL ) ) ;
  if ( boost::filesystem::exists( alpfile ) ) 
    boost::filesystem::remove( alpfile ) ;
  
  std::ofstream g( alpfile.string().c_str() ) ;
  g << 2 << std::endl // Generation mode
    << "zbb3" << std::endl  ;        // label of the output files
  g.close() ;

  AlpGenFunctions::AlSprc() ;

  F77Utils::open( 5 , alpfile.string() , false ) ;  
  AlpGenFunctions::AlSdef() ;
  F77Utils::close( 5 ) ;

  boost::filesystem::remove( alpfile ) ;

  AlpGenFunctions::AlHset() ;
  AlpGenFunctions::AlInit() ;
  AlpGenFunctions::AlSbkk() ;
  AlpGenFunctions::AlShis() ;
  AlpGenFunctions::AlSgrd() ;
  AlpGenFunctions::AlIgrd() ;
  AlpGenFunctions::AlEgen() ;
  AlpGenFunctions::AlFhis() ;
  AlpGenFunctions::AlFbkk() ;

  F77Utils::close( 11 ) ;
  F77Utils::close( 10 ) ;
  F77Utils::close( 12 ) ;
}
