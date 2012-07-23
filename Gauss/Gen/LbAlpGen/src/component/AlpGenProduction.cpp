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
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/IRndmEngine.h"
#include "GaudiKernel/SystemOfUnits.h"

// from Event
#include "Event/GenCollision.h"

// local
#include "Generators/StringParse.h"
#include "Generators/IBeamTool.h"
#include "Generators/F77Utils.h"
#include "LbPythia/Pythia.h"
#include "LbPythia/Hepeup.h"
#include "LbAlpGen/AlpGenFunctions.h"
#include "LbAlpGen/Ahio.h"
#include "LbAlpGen/Aiounits.h"

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
  : PythiaProduction ( type, name , parent ) ,
    m_nevents  ( 0  ) , 
    m_randSvc  ( 0  ) ,
    m_engine   ( 0  ) ,
    m_fileLabel( "" ) {
  declareInterface< IProductionTool >( this ) ;
  declareProperty( "FileLabel" , m_fileLabel = "zbb3" ) ;
}

//=============================================================================
// Destructor 
//=============================================================================
AlpGenProduction::~AlpGenProduction( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode AlpGenProduction::initialize( ) {
  // Do some cleaning of files
  boost::filesystem::remove( boost::filesystem::path( "cnfg.dat" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( "par.list" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".grid1" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".grid2" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".mon" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".par" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".pyt-log" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".stat" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".top" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".unw" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + "_unw.par" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + "_unw.top" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".wgt" ) ) ;
  //
  m_nevents = 0 ;
  // generate events with default seed for Pythia initialization
  generateWeightedEvents( ) ;
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
  g << m_fileLabel << std::endl ;    // label of the output files
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
// Finalize
//=============================================================================
StatusCode AlpGenProduction::finalize( ) {
  AlpGenFunctions::AlSfin() ;
  Ahio ahio ;
  F77Utils::close( ahio.nunit() ) ;
  F77Utils::close( ahio.nunitout() ) ;

  Aiounits aiounits ;
  F77Utils::close( aiounits.niosta() ) ;
  F77Utils::close( aiounits.niopar() ) ;
  F77Utils::close( aiounits.niowgt() ) ;
  F77Utils::close( aiounits.niounw() ) ;

  // Do some cleaning of files
  boost::filesystem::remove( boost::filesystem::path( "cnfg.dat" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( "par.list" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".grid1" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".grid2" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".mon" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".par" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".pyt-log" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".stat" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".top" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".unw" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + "_unw.par" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + "_unw.top" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".wgt" ) ) ;
  
  return PythiaProduction::finalize() ;
}

//=============================================================================
//   Function called to generate one event with Pythia --> AlpGen
//=============================================================================
StatusCode AlpGenProduction::generateEvent( HepMC::GenEvent * theEvent , 
                                            LHCb::GenCollision * theCollision )
{
  m_nevents++ ;
  if ( 1 == m_nevents ) {
    // generate events with correct random seed for the first event
    Ahio ahio ;
    F77Utils::close( ahio.nunit() ) ;
    F77Utils::close( 99 ) ;
    Aiounits aiounits ;
    F77Utils::close( aiounits.niosta() ) ;
    F77Utils::close( aiounits.niopar() ) ;
    F77Utils::close( aiounits.niowgt() ) ;
    F77Utils::close( aiounits.niounw() ) ;
    generateWeightedEvents( ) ;
    generateUnweightedEvents( ) ;
    F77Utils::open( ahio.nunit() , m_fileLabel + ".unw" , false ) ;    
  }

  StatusCode sc = PythiaProduction::generateEvent(theEvent, theCollision) ;
  if ( sc.isFailure() ) return sc ;

  Hepeup hepeup ;
  
  if ( 0 == hepeup.nup() ) {
    // end of ALPGEN file reached, generate a new file
    debug() << "End of ALPGEN file reached. Generate a new one" << endmsg ;
    Ahio ahio ;
    F77Utils::close( ahio.nunit() ) ;
    F77Utils::close( 99 ) ;
    Aiounits aiounits ;
    F77Utils::close( aiounits.niosta() ) ;
    F77Utils::close( aiounits.niopar() ) ;
    F77Utils::close( aiounits.niowgt() ) ;
    F77Utils::close( aiounits.niounw() ) ;
    generateWeightedEvents( ) ;
    generateUnweightedEvents( ) ;
    F77Utils::open( ahio.nunit() , m_fileLabel + ".unw" , false ) ;

    sc = PythiaProduction::generateEvent(theEvent, theCollision) ;
    if ( sc.isFailure() ) return sc ;
  }

  return StatusCode::SUCCESS ;
}

//============================================================================
// Generate weighted events with ALPGEN
//============================================================================
StatusCode AlpGenProduction::generateWeightedEvents( ) {
  // create temporary file to input to ALPGEN
  boost::filesystem::path alpfile( std::tmpnam( NULL ) ) ;
  if ( boost::filesystem::exists( alpfile ) ) 
    boost::filesystem::remove( alpfile ) ;
  
  std::ofstream g( alpfile.string().c_str() ) ;
  g << 1 << std::endl // Generation mode
    << m_fileLabel << std::endl          // label of the output files
    << 0 << std::endl              // new grid
    << 10000 << " " << 2 << std::endl // nevents per iteraction and n iterations
    << 100000 << std::endl            // nevents to generate
    << "izdecmode " << 2 << std::endl 
    << "ihvy " << 5 << std::endl ;
  
  // Obtain beam tool
  if ( 0 == m_beamTool ) 
    m_beamTool = tool< IBeamTool >( m_beamToolName , this ) ;

  Gaudi::XYZVector pBeam1 , pBeam2 ;
  m_beamTool -> getMeanBeams( pBeam1 , pBeam2 ) ;

  g << "ebeam " << pBeam1.R() / Gaudi::Units::GeV << std::endl  // beam energy
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
    << "ilep " << 0 << std::endl ;
  
  if ( 0 == m_nevents ) {
    // use default seeds
    g << "iseed1 " << 12345 << std::endl 
      << "iseed2 " << 67890 << std::endl
      << "iseed3 " << 12345 << std::endl 
      << "iseed4 " << 67890 << std::endl ;
  } else {
    // use Gauss seeds
    if( 0 == m_randSvc ) m_randSvc = svc<IRndmGenSvc>( "RndmGenSvc", true );
    if( 0 == m_engine  )
    {
      m_engine  = m_randSvc->engine();
      if( 0 == m_engine ) {
        return Error( "Random number engine not found!" );
      }
    }
    std::vector< long > theSeeds ;
    m_engine -> seeds( theSeeds ) ;

    g << "iseed1 " << theSeeds[ 0 ] << std::endl
      << "iseed2 " << theSeeds[ 1 ] << std::endl
      << "iseed3 " << theSeeds[ 0 ] << std::endl
      << "iseed4 " << theSeeds[ 1 ] << std::endl ;

    release( m_randSvc ) ;
    m_randSvc = 0 ;
    m_engine = 0 ;
  }  
  
  g << "izdecmod " << 2 << std::endl 
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

  return StatusCode::SUCCESS ;
}

//============================================================================
// Generate weighted events with ALPGEN
//============================================================================
void AlpGenProduction::generateUnweightedEvents( ) {
  // create temporary file to input to ALPGEN
  boost::filesystem::path alpfile( std::tmpnam( NULL ) ) ;
  if ( boost::filesystem::exists( alpfile ) ) 
    boost::filesystem::remove( alpfile ) ;
  
  std::ofstream g( alpfile.string().c_str() ) ;
  g << 2 << std::endl // Generation mode
    << m_fileLabel << std::endl  ;        // label of the output files
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
