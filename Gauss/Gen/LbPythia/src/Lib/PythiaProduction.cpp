// $Id: PythiaProduction.cpp,v 1.24 2010-02-04 19:06:34 robbep Exp $

// Include files
// STD * STL 
#include <fstream> 

// local
#include "LbPythia/Pythia.h"
#include "LbPythia/PythiaProduction.h"

// from boost
#include "boost/filesystem.hpp"

// from Gaudi
#include "GaudiKernel/System.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"

// from Event
#include "Event/GenCollision.h"

// Generators 
#include "Generators/StringParse.h"
#include "Generators/IBeamTool.h"
#include "Generators/F77Utils.h"

// HepMC
#include "HepMC/IO_HEPEVT.h"
//#include "HepMC/IO_Ascii.h"
#include "HepMC/HEPEVT_Wrapper.h"


// ============================================================================
/** @file 
 *  Implementation file for class PythiaProduction
 *
 *  @date 2005-08-16 
 *  @author Patrick Robbe, modified by Neal Gueissaz for supersymmetry
 */
//-----------------------------------------------------------------------------


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PythiaProduction::PythiaProduction( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiTool ( type, name , parent ) ,
    m_userProcess( 0 ) ,
    m_frame( "3MOM" )  ,
    m_beam( "p+" )     ,
    m_target( "p+" )   ,
    m_win( 0. )        ,
    //
    m_defaultSettings () ,
    m_commandVector   () ,
    m_pygive          () ,
    //
    m_variableEnergy( false ) ,
    //
    m_eventListingLevel          ( -1 ) ,
    m_eventListingLevel2         ( -1 ) ,
    m_initializationListingLevel ( 1  ) ,
    m_finalizationListingLevel   ( 1  ) ,
    m_pythiaListingFileName      ( "" ) ,
    m_pythiaListingUnit          ( 0  ) ,
    //
    m_particleDataUnit   ( 59 ) ,
    // no default value!
    m_particleDataOutput () , 
    // no default value!
    m_particleDataInput  () , 
    // no default value! 
    m_particleDataLevel  ( 0 ) ,
    m_beamTool   ( 0 ) ,
    // MSTU(1)/MSTU(2) for initialization PYLIST
    m_ini_mstu_1 ( 0 ) , 
    m_ini_mstu_2 ( 0 ) , 
    // MSTU(1)/MSTU(2) for "generateEvent" PYLIST
    m_eve_mstu_1 ( 0 ) , 
    m_eve_mstu_2 ( 0 ) , 
    // MSTU(1)/MSTU(2) for "hadronize" PYLIST
    m_had_mstu_1 ( 0 ) , 
    m_had_mstu_2 ( 0 ) , 
    // list of particles to be printed using PyList(12) 
    m_pdtlist    (   ) , 
    m_nEvents    ( 0 ) ,
    m_widthLimit ( 1.5e-6 * Gaudi::Units::GeV ) ,
    m_pdecaylist (   )
  /// boolean flag to force the valiadation of IO_HEPEVT 
  , m_validate_HEPEVT ( false ) // force the valiadation of IO_HEPEVT 
  /// the file to dump the HEPEVT inconsistencies 
  , m_inconsistencies ( "HEPEVT_inconsistencies.out" ) 
  , m_HEPEVT_errors ( 0 )
  // list of particles whose properties should *always* be taken from 
                                  // the ParticlePropertySvc
  , m_updatedParticles (   ) // TODO declare this one: vector<int> m_updatedParticles
{
  declareInterface< IProductionTool >( this ) ;
  declareProperty( "Commands" , m_commandVector ) ;
  declareProperty( "PygiveCommands" , m_pygive  ) ;
  declareProperty( "PDTList"        , m_pdtlist ) ;
  declareProperty( "BeamToolName" , m_beamToolName = "CollidingBeams" ) ;
  declareProperty( "WidthLimit" , m_widthLimit = 1.5e-6 * Gaudi::Units::GeV ) ;
  declareProperty( "SLHADecayFile" , m_slhaDecayFile = "empty" ) ;
  declareProperty( "PDecayList" , m_pdecaylist ) ;
  declareProperty( "SLHASpectrumFile" , m_slhaSpectrumFile = "empty" ) ;

  declareProperty 
    ( "ValidateHEPEVT"  , 
      m_validate_HEPEVT ,
      "The flag to force the validation (mother&daughter) of HEPEVT" ) ;
  
  declareProperty 
    ( "Inconsistencies" , 
      m_inconsistencies , 
      "The file to dump HEPEVT inconsinstencies" ) ;

  declareProperty( "UpdatedParticles", m_updatedParticles,
                   "Particles (specified by their pdgID's) whose properties should *always* be taken from the ParticlePropertySvc, regardless of Pythia defaults or other input files" );
  
  // Set the default settings for Pythia here:
  m_defaultSettings.clear() ;
  m_defaultSettings.push_back( "pysubs msel 0" ) ;
  m_defaultSettings.push_back( "pysubs msub 11 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 12 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 13 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 28 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 53 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 68 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 91 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 92 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 93 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 94 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 95 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 421 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 422 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 423 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 424 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 425 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 426 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 427 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 428 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 429 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 430 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 431 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 432 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 433 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 434 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 435 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 436 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 437 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 438 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 439 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 461 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 462 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 463 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 464 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 465 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 466 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 467 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 468 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 469 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 470 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 471 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 472 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 473 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 474 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 475 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 476 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 477 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 478 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 479 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 480 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 481 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 482 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 483 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 484 1" ) ;
  m_defaultSettings.push_back( "pysubs msub 485 1" ) ;
  // Allows generation of resonances (psi(3770) for example) in 2 -> 2 processes
  m_defaultSettings.push_back( "pysubs ckin 41 3.0" ) ;
  m_defaultSettings.push_back( "pypars mstp 2 2" ) ;
  m_defaultSettings.push_back( "pypars mstp 33 3" ) ;
  m_defaultSettings.push_back( "pypars mstp 128 2" ) ;
  m_defaultSettings.push_back( "pypars mstp 81 21" ) ;
  m_defaultSettings.push_back( "pypars mstp 82 3" ) ;
  m_defaultSettings.push_back( "pypars mstp 52 2" ) ;
  m_defaultSettings.push_back( "pypars mstp 51 10042" ) ;
  m_defaultSettings.push_back( "pypars mstp 142 2" ) ;
  m_defaultSettings.push_back( "pypars parp 67 1.0" ) ;
  m_defaultSettings.push_back( "pypars parp 82 4.28" ) ;
  m_defaultSettings.push_back( "pypars parp 89 14000" ) ;
  m_defaultSettings.push_back( "pypars parp 90 0.238" ) ;
  m_defaultSettings.push_back( "pypars parp 85 0.33" ) ;
  m_defaultSettings.push_back( "pypars parp 86 0.66" ) ;
  m_defaultSettings.push_back( "pypars parp 91 1.0" ) ;
  m_defaultSettings.push_back( "pypars parp 149 0.02" ) ;
  m_defaultSettings.push_back( "pypars parp 150 0.085" ) ;
  m_defaultSettings.push_back( "pydat1 parj 11 0.4" ) ;
  m_defaultSettings.push_back( "pydat1 parj 12 0.4" ) ;
  m_defaultSettings.push_back( "pydat1 parj 13 0.769" ) ;
  m_defaultSettings.push_back( "pydat1 parj 14 0.09" ) ;
  m_defaultSettings.push_back( "pydat1 parj 15 0.018" ) ;
  m_defaultSettings.push_back( "pydat1 parj 16 0.0815" ) ;
  m_defaultSettings.push_back( "pydat1 parj 17 0.0815" ) ;
  m_defaultSettings.push_back( "pydat1 mstj 26 0" ) ;
  m_defaultSettings.push_back( "pydat1 parj 33 0.4" ) ;
} ;


//=============================================================================
// Destructor 
//=============================================================================
PythiaProduction::~PythiaProduction( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode PythiaProduction::initialize( ) {
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;

  always()
    << "=================================================================="
    << endmsg;
  always()
    << "Using as production engine  " << this->type()
    << endmsg;
  always()
    << "=================================================================="
    << endmsg;

  // Obtain beam tool
  if ( 0 == m_beamTool ) 
    m_beamTool = tool< IBeamTool >( m_beamToolName , this ) ;

  // Name of PYSLHA file to read
  if ( "empty" != m_slhaDecayFile ) {
    if ( "UNKNOWN" != System::getEnv( "DECFILESROOT" ) ) {
      std::string temp = m_slhaDecayFile ;
      m_slhaDecayFile = System::getEnv( "DECFILESROOT" ) +
	"/lhafiles/" + temp ;
    }
    // Check if file exists
    boost::filesystem::path slhaDecayFile( m_slhaDecayFile ) ;
    if ( ! boost::filesystem::exists( slhaDecayFile ) ){ 
      return Error("The SLHA decay file does not exist : " +  m_slhaDecayFile);
    }
  }

  // Name of PYSLHA mass spectrum file to read
  if ( "empty" != m_slhaSpectrumFile ) {

    if ( "UNKNOWN" != System::getEnv( "DECFILESROOT" ) ) {
      std::string temp = m_slhaSpectrumFile ;
      m_slhaSpectrumFile = System::getEnv( "DECFILESROOT" ) +
	"/lhafiles/" + temp ;
    }

    // Check if file exists
    boost::filesystem::path shlaSpectrumFile( m_slhaSpectrumFile ) ;
    if ( ! boost::filesystem::exists( shlaSpectrumFile ) ) 
      return Error( "The SLHA mass spectrum file does not exist : " + 
		    m_slhaSpectrumFile ) ;
  }

  // Set size of common blocks in HEPEVT: note these correspond to stdhep
  HepMC::HEPEVT_Wrapper::set_sizeof_int( 4 ) ;
  HepMC::HEPEVT_Wrapper::set_sizeof_real( 8 ) ;
  HepMC::HEPEVT_Wrapper::set_max_number_entries( 10000 ) ;  

  sc = initializeGenerator() ;
  if ( ! sc.isSuccess() ) return sc ;
  
  // Now that Pythia is initialized, update the mass of the special particles
  // in the particle property service (it is because Pythia may have changed
  // these masses after computation from its internal parameters)
  // retrieve the particle property service
  // LHCb::IParticlePropertySvc * ppSvc = 
  //  svc< LHCb::IParticlePropertySvc >( "Gaudi::ParticlePropertySvc" , true ) ;
  //LHCb::IParticlePropertySvc::iterator iter ;
  // for ( iter = ppSvc -> begin() ; iter != ppSvc -> end() ; ++iter ) {
  //if ( isSpecialParticle( *iter ) ) {
      // int pythiaId = (*iter) -> pythiaID() ;
      // int kc = Pythia::PyComp( pythiaId ) ;
      //      (*iter) -> setMass( Pythia::pydat2().pmas( kc , 1 ) * Gaudi::Units::GeV ) ;
      // Not possible anymore with the new particle property service...
  // }
  // }
  //release( ppSvc ) ;
  return StatusCode::SUCCESS ;
}

//=============================================================================
// Part specific to generator initialization
//=============================================================================
StatusCode PythiaProduction::initializeGenerator( ) {
  StatusCode sc = StatusCode::SUCCESS ;
  
    // Initialize output
  if ( msgLevel( MSG::DEBUG ) ) {
    Pythia::pydat1().mstu( 12 ) = 1 ;
    Pythia::pydat1().mstu( 13 ) = 1 ;
    Pythia::pydat1().mstu( 25 ) = 1 ;
    Pythia::pypars().mstp( 122 ) = 1 ;
  } else {
    Pythia::pydat1().mstu( 12 ) = 12345 ;
    Pythia::pydat1().mstu( 13 ) = 0 ;
    Pythia::pydat1().mstu( 25 ) = 0 ;
    Pythia::pypars().mstp( 122 ) = 0 ;
    m_initializationListingLevel = -1 ;
  }
  
  // Add extra processes for LHCb pythia
  // Process 480: g + g -> Psi(2S) + g Color Singlet
  Pythia::pyint2().iset(480) = 2 ;
  Pythia::pyint2().kfpr(480,1) = 100443 ;
  Pythia::pyint2().kfpr(480,2) = 21 ;
  // Process 481: g + g -> Upsilon(2S) + g Color Singlet
  Pythia::pyint2().iset(481) = 2 ;
  Pythia::pyint2().kfpr(481,1) = 100553 ;
  Pythia::pyint2().kfpr(481,2) = 21 ;
  // Process 482: g + g -> Upsilon(3S) + g Color Singlet
  Pythia::pyint2().iset(482) = 2 ;
  Pythia::pyint2().kfpr(482,1) = 200553 ;
  Pythia::pyint2().kfpr(482,2) = 21 ;
  // Process 483: g + g -> Upsilon(4S) + g Color Singlet
  Pythia::pyint2().iset(483) = 2 ;
  Pythia::pyint2().kfpr(483,1) = 300553 ;
  Pythia::pyint2().kfpr(483,2) = 21 ;
  // Process 484: g + g -> Upsilon(5S) + g Color Singlet
  Pythia::pyint2().iset(484) = 2 ;
  Pythia::pyint2().kfpr(484,1) = 9000553 ;
  Pythia::pyint2().kfpr(484,2) = 21 ;
  // Process 485: g + g -> psi(3770) + g Color Singlet
  Pythia::pyint2().iset(485) = 2 ;
  Pythia::pyint2().kfpr(485,1) = 30443 ;
  Pythia::pyint2().kfpr(485,2) = 21 ;
  
  // Set User process to 0 for normal Pythia to be overriden for
  // specific generation
  Pythia::SetUserProcess( m_userProcess ) ;

  // Prepare the PYINIT call by setting the beam parameters if necessary
  std::transform( m_frame.begin() , m_frame.end() , m_frame.begin() , 
                  tolower ) ;
  if ( ( "4mom" == m_frame ) || ( "5mom" == m_frame ) ) 
    return Error( "This FRAME is not yet implemented" ) ;
  
  if ( "3mom" == m_frame ) {
    Gaudi::XYZVector pBeam1 , pBeam2 ;
    m_beamTool -> getMeanBeams( pBeam1 , pBeam2 ) ;
    // Pythia Units are GeV
    Pythia::SetBeam( pBeam1.X() / Gaudi::Units::GeV , 
                     pBeam1.Y() / Gaudi::Units::GeV , 
                     pBeam1.Z() / Gaudi::Units::GeV ,
                     pBeam2.X() / Gaudi::Units::GeV , 
                     pBeam2.Y() / Gaudi::Units::GeV , 
                     pBeam2.Z() / Gaudi::Units::GeV ) ;

    Pythia::pypars().mstp( 171 ) = 1 ;  // new energy given for each event 
    Pythia::pypars().mstp( 172 ) = 1 ;  // event generated at requested energy
    m_variableEnergy = true ;
  }
  
  Pythia::pydat3().mdme( 4178 , 1 ) = -1 ; // like in egpyinit  

  // Set default Pythia settings
  sc = parsePythiaCommands( m_defaultSettings ) ;
  // read Pythia command vector from job options
  sc = parsePythiaCommands( m_commandVector ) ;
  if ( ! sc.isSuccess( ) ) 
    return Error( "Unable to read Pythia commands" , sc ) ;
  
  // read input decay table (if needed)
  if ( 0 != m_particleDataUnit && !m_particleDataInput.empty() ) {

    if ( 2 != m_particleDataLevel && 3 != m_particleDataLevel ) { 
      return Error ( "Illegal value for pdtlevel ( should be [2-3])" ) ; 
    }

    //
    StatusCode sc = F77Utils::openOld( m_particleDataUnit , 
                                       m_particleDataInput ) ;

    if ( sc.isFailure() ) { 
      return Error ( "Could not open input PDT file '" + 
                      m_particleDataInput+"'" , sc ) ; 
    }

    // update the table
    info() << " CALL PYUPDA(" << m_particleDataLevel 
           << ","  << m_particleDataUnit
           << "/'" << m_particleDataInput <<"') " << endreq ;
    Pythia::PyUpda( m_particleDataLevel  , m_particleDataUnit ) ;

    // close the file 
    F77Utils::close ( m_particleDataUnit ) ;
    always() <<" Particle Data Table  has been read from the file '" 
             << m_particleDataInput << "'" << endreq ;  
  }
  
  // use PYGIVE commands (if any) (as THE LAST action)
  for ( CommandVector::const_iterator item = m_pygive.begin() ; 
        m_pygive.end() != item ; ++item ) {
    // use FORTRAN PYGIVE routine
    debug() << " CALL PYGIVE(' " << (*item) << "')" << endreq ;
    const int mstu_13 = Pythia::pydat1().mstu(13) ;
    Pythia::pydat1().mstu(13) =1   ;
    Pythia::PyGive( *item ) ;
    Pythia::pydat1().mstu(13) = mstu_13 ;
  }

  // Now call pyinit and set listing
  // if file already exist, delete it
  std::remove( m_pythiaListingFileName.c_str() ) ;
  Pythia::InitPyBlock( m_pythiaListingUnit , m_pythiaListingFileName ) ;

 // if specified, read SHLA mass spectrum file
  int lunUnit = -1;
  if ( "empty" != m_slhaSpectrumFile ) {
    lunUnit = F77Utils::getUnit( msgLevel( MSG::DEBUG ) ) ;
    if ( 0 >= lunUnit )
      return Error( "No Fortran Unit available" ) ;
    sc = F77Utils::openOld( lunUnit , m_slhaSpectrumFile ,
                            msgLevel( MSG::INFO ) ) ;
    if ( sc.isFailure() )
      return Error( "Cannot open SLHA mass spectrum file" ) ;
    Pythia::pymssm().imss(21) = lunUnit ;
    //If no decay file provided, assume it is in the spectrum file
    if ( "empty" == m_slhaDecayFile ) Pythia::pymssm().imss(22) = lunUnit ;
  }

  // if specified, read SHLA decay file
  int lunUnit2 = -1;
  if ( "empty" != m_slhaDecayFile ) {
    lunUnit2 = F77Utils::getUnit( msgLevel( MSG::DEBUG ) ) ;
    if ( 0 >= lunUnit2 )
      return Error( "No Fortran Unit available" ) ;
    sc = F77Utils::openOld( lunUnit2 , m_slhaDecayFile ,
                            msgLevel( MSG::INFO ) ) ;
    if ( sc.isFailure() )
      return Error( "Cannot open SLHA decay file" ) ;

    Pythia::pymssm().imss(22) = lunUnit2 ;
    int status = 0 ;
    if( m_pdecaylist.size() != 0 ){
      for( std::vector<int>::const_iterator i = m_pdecaylist.begin();
	   i != m_pdecaylist.end(); i++ ){
	Pythia::PySlha( 2 , *i , status ) ;
	debug() << "Updating Particle "<< *i <<", Status " << status <<endreq;
	if(status != 0) return Error( "Could not update particle " ) ;
      }
      sc = F77Utils::close( lunUnit2 , msgLevel( MSG::INFO ) ) ;
      if ( sc.isFailure() )
	return Error( "Cannot close SLHA decay file" ) ;
    }
  }
  
  debug () 
    << " initialize PYTHIA with "
    << "  FRAME='"      << m_frame       << "'"
    << "   BEAM='"      << m_beam        << "'"
    << " TARGET='"      << m_target      << "'"
    << "    WIN='"      << m_win         << "'"
    << " USERPROCESS='" << m_userProcess << "'" << endmsg ;
  
  Pythia::PyInit( m_frame, m_beam, m_target, m_win ) ;  
  
  //Close mass spectrum file
  if ( "empty" != m_slhaSpectrumFile ) {
    sc = F77Utils::close( lunUnit , msgLevel( MSG::INFO ) ) ;
    if ( sc.isFailure() )
      return Error( "Cannot close SLHA mass spectrum file" ) ;
  }

 
  //Close decay file
   if ( "empty" != m_slhaDecayFile && 0 == m_pdecaylist.size() ) {  
     sc = F77Utils::close( lunUnit2 , msgLevel( MSG::INFO ) ) ;
     if ( sc.isFailure() )
       return Error( "Cannot close SLHA decay file" ) ;
   }

  // Reset forced fragmentation flag
  Pythia::pydat1().mstu( 150 ) = 0 ;

  // Reset the "updated particles" to their defaults
  if ( m_updatedParticles.size() != 0 ) {
    LHCb::IParticlePropertySvc* ppSvc = svc< LHCb::IParticlePropertySvc>("LHCb::ParticlePropertySvc" , true);
    for ( std::vector<int>::const_iterator it = m_updatedParticles.begin(); it != m_updatedParticles.end(); ++it ) {
      updateParticleProperties(ppSvc->find( LHCb::ParticleID( *it ) ));
    }
    release(ppSvc);
  }

  return sc ;
}

//=============================================================================
//   Function called to generate one event with Pythia
//=============================================================================
StatusCode PythiaProduction::generateEvent( HepMC::GenEvent * theEvent , 
                                            LHCb::GenCollision * theCollision )
{
  // Set beam parameters if variable energy
  if ( m_variableEnergy ) {
    Gaudi::XYZVector pBeam1 , pBeam2 ;
    m_beamTool -> getBeams( pBeam1 , pBeam2 ) ;
    // PYTHIA Units are GeV
    Pythia::SetBeam( pBeam1.X() / Gaudi::Units::GeV ,
                     pBeam1.Y() / Gaudi::Units::GeV , 
                     pBeam1.Z() / Gaudi::Units::GeV ,
                     pBeam2.X() / Gaudi::Units::GeV , 
                     pBeam2.Y() / Gaudi::Units::GeV , 
                     pBeam2.Z() / Gaudi::Units::GeV ) ;
  }
  
  // Generate Event
  Pythia::PyEvnt( ) ;
  
  // Update event counter
  ++m_nEvents ;
  
  // Debugging output: print each event if required
  if ( m_eventListingLevel >= 0 ) 
  { 
    const int mstu_1 =  Pythia::pydat1().mstu(1) ;
    const int mstu_2 =  Pythia::pydat1().mstu(2) ;    
    Pythia::pydat1().mstu(1) = m_eve_mstu_1 ;
    Pythia::pydat1().mstu(2) = m_eve_mstu_2 ;
    Pythia::PyList( m_eventListingLevel ) ;
    Pythia::pydat1().mstu(1) = mstu_1 ;
    Pythia::pydat1().mstu(2) = mstu_2 ;
  }
  
  return toHepMC( theEvent , theCollision ) ;
}

//=============================================================================
// Set stable the given particle in Pythia
//=============================================================================
void PythiaProduction::setStable( const LHCb::ParticleProperty * thePP ) {
  int pythiaId = thePP -> pythiaID() ;
  if ( 0 != pythiaId ) {
    int kc = Pythia::PyComp( pythiaId ) ;
    if ( kc > 0 ) Pythia::pydat3().mdcy( kc , 1 ) = 0 ;
  }
}

//=============================================================================
// Update particle properties
//=============================================================================
void PythiaProduction::updateParticleProperties( const LHCb::ParticleProperty * 
                                                 thePP ) {
  int pythiaId = thePP -> pythiaID() ;

  //If MSSM mass spectrum is provided, no need to update susy particles.
  if( m_slhaSpectrumFile != "empty" ){
    if( abs(pythiaId)>1000000 && abs(pythiaId)<1001000 ) return;
    if( abs(pythiaId)>2000000 && abs(pythiaId)<2001000 ) return;
    if( pythiaId == 25 || pythiaId == 35 || pythiaId == 36 ||
        abs(pythiaId) == 37 ) return;
  }

  //If MSSM decay file is provided, do not update particles from PDecayList.
  if( m_slhaDecayFile != "empty" ){
    for( std::vector<int>::const_iterator i = m_pdecaylist.begin();
	 i != m_pdecaylist.end(); i++ ){
      if( pythiaId == *i ) return;
    }
  }

  double pwidth , lifetime ;
  if ( 0 != pythiaId ) {
    int kc = Pythia::PyComp( pythiaId ) ;
    if ( kc > 0 ) {
      if ( 0 == thePP -> lifetime() ) pwidth = 0. ;
      else pwidth = ( Gaudi::Units::hbarc / 
		      ( thePP -> lifetime() * Gaudi::Units::c_light ) ) ;
      if ( pwidth < m_widthLimit ) pwidth = 0. ;

      lifetime =  thePP -> lifetime() * Gaudi::Units::c_light ;
      if ( ( lifetime <= 1.e-4 * Gaudi::Units::mm ) || 
           ( lifetime >= 1.e16 * Gaudi::Units::mm ) ) 
        lifetime = 0. ;
      
      verbose() << "Change particle property of KC = " << kc 
                << " (" << pythiaId << ")" << endmsg ;
      verbose() << "Mass (GeV) from " << Pythia::pydat2().pmas( kc , 1 ) 
                << " to " << thePP -> mass() / Gaudi::Units::GeV << endmsg ;
      verbose() << "Width (GeV) from " << Pythia::pydat2().pmas( kc , 2 ) 
                << " to " << pwidth / Gaudi::Units::GeV << endmsg ;
      verbose() << "MaxWidth (GeV) from " << Pythia::pydat2().pmas( kc , 3 ) 
                << " to " << thePP -> maxWidth() / Gaudi::Units::GeV << endmsg ;
      verbose() << "Lifetime from " << Pythia::pydat2().pmas( kc , 4 ) 
                << " to " << lifetime / Gaudi::Units::mm << endmsg ;
      
      Pythia::pydat2().pmas( kc , 1 ) = thePP -> mass() / Gaudi::Units::GeV ;
      // For Higgs, top, Z and W: update only masses
      if ( ( 6 != pythiaId ) && ( 23 != pythiaId ) && ( 24 != pythiaId ) 
           && ( 25 != pythiaId ) ) {
        Pythia::pydat2().pmas( kc , 2 ) = pwidth / Gaudi::Units::GeV ;
        Pythia::pydat2().pmas( kc , 3 ) = thePP -> maxWidth() / Gaudi::Units::GeV ;
        Pythia::pydat2().pmas( kc , 4 ) = lifetime / Gaudi::Units::mm ;
      }
    }
  }
}


//=============================================================================
// Retrieve the Hard scatter information
//=============================================================================
void PythiaProduction::hardProcessInfo( LHCb::GenCollision * theCollision ) {
  theCollision -> setProcessType( Pythia::pypars().msti(1) ) ;
  theCollision -> setSHat( Pythia::pypars().pari(14) );
  theCollision -> setTHat( Pythia::pypars().pari(15) );
  theCollision -> setUHat( Pythia::pypars().pari(16) );
  theCollision -> setPtHat( Pythia::pypars().pari(17) );
  theCollision -> setX1Bjorken( Pythia::pypars().pari(33) );
  theCollision -> setX2Bjorken( Pythia::pypars().pari(34) );

  debug() << "Hard process = " 
          << Pythia::pypars().msti(1)  << " "
          << Pythia::pypars().pari(14) << " " 
          << Pythia::pypars().pari(15) << " " 
          << Pythia::pypars().pari(16) << " " 
          << Pythia::pypars().pari(17) << " " 
          << Pythia::pypars().pari(33) << " " 
          << Pythia::pypars().pari(34)
          << endreq ;
} 

//=============================================================================
// Finalize method
//=============================================================================
StatusCode PythiaProduction::finalize( ) {
  Pythia::PyStat( m_finalizationListingLevel ) ;  
  
  if ( 0 != m_HEPEVT_errors ) 
  { delete m_HEPEVT_errors ; m_HEPEVT_errors = 0 ; }
  
  return GaudiTool::finalize( ) ;
}  

//=============================================================================
// Parse Pythia commands stored in a vector
//=============================================================================
StatusCode PythiaProduction::parsePythiaCommands( const CommandVector & 
                                                  theCommandVector ) {
  //
  // Parse Commands and Set Values from Properties Service...
  //
  CommandVector::const_iterator iter ;
  for ( iter = theCommandVector.begin() ; theCommandVector.end() != iter ; 
        ++iter ) {
    debug() << " Command is: " << (*iter) << endmsg ;
    StringParse mystring( *iter ) ;
    std::string block = mystring.piece(1);
    std::string entry = mystring.piece(2);
    std::string str   = mystring.piece(3);
    int    int1  = mystring.intpiece(3);
    int    int2  = mystring.intpiece(4);
    int    int3  = mystring.intpiece(5);
    int    int4  = mystring.intpiece(6);
    double fl1   = mystring.numpiece(4);
    double fl2   = mystring.numpiece(5);
    double fl0   = mystring.numpiece(3);
    
    // Note that Pythia needs doubles hence the convert here
    debug() << block << " block  " << entry << " item  " << int1 
            << "  value " << fl1 << endmsg ;
    if ( "pyinit" == block ) 
      if      ( "pbar"    == entry ) m_beam                       = "pbar-" ; 
      else if ( "win"     == entry ) m_win                        = fl0   ; 
      else if ( "pylisti" == entry ) 
      { 
        m_initializationListingLevel   = int1 ;
        if ( 0 < int2 ) { m_ini_mstu_1 = int2 ; }
        if ( 0 < int3 ) { m_ini_mstu_2 = int3 ; }
      }
      else if ( "pyliste" == entry ) 
      {
        m_eventListingLevel          = int1  ; 
        if ( 0 < int2 ) { m_eve_mstu_1 = int2 ; }
        if ( 0 < int3 ) { m_eve_mstu_2 = int3 ; }
      }
      else if ( "pylisth" == entry ) 
      {
        m_eventListingLevel2         = int1  ; 
        if ( 0 < int2 ) { m_had_mstu_1 = int2 ; }
        if ( 0 < int3 ) { m_had_mstu_2 = int3 ; }
      }
      else if ( "pystatf" == entry ) m_finalizationListingLevel   = int1  ; 
      else if ( "output"  == entry ) {
        m_pythiaListingFileName         = str                 ;
        m_pythiaListingUnit             = 49                  ;
        Pythia::pydat1().mstu(11)       = m_pythiaListingUnit ; 
        Pythia::pydat1().mstu( 12 )     = 1                   ;
        Pythia::pydat1().mstu( 13 )     = 1                   ;
        Pythia::pydat1().mstu( 25 )     = 1                   ;
        Pythia::pypars().mstp( 122 )    = 1                   ;
      }
    //
      else if (  "pdtunit"   == entry ) { m_particleDataUnit   = int1 ; }
      else if (  "pdtoutput" == entry ) { m_particleDataOutput = str  ; }
      else if (  "pdtlevel"  == entry ) { m_particleDataLevel  = int1 ; }
      else if (  "pdtinput"  == entry ) { m_particleDataInput  = str  ; }
    //
      else return Error(std::string("PYTHIA ERROR, entry PYINIT has PBAR ")+
                        std::string("PYLISTI PYLISTE OUTPUT DUMPR AND WIN: ")+ 
                        std::string("YOU HAVE SPECIFIED ") + 
                        std::string( entry ) ) ;
    else if ( "pysubs" == block )  
      if      ( "msel" == entry ) Pythia::pysubs().msel()              = int1 ;
      else if ( "msub" == entry ) Pythia::pysubs().msub( int1 )        = int2 ;
      else if ( "ckin" == entry ) Pythia::pysubs().ckin( int1 )        = fl1  ;
      else if ( "kfin" == entry ) Pythia::pysubs().kfin( int1 , int2 ) = int3 ;
      else return Error( std::string( "PYTHIA ERROR, block PYSUBS has MSEL" ) +
                         std::string( ", MSUB, KFIN AND CKIN: YOU HAVE " ) + 
                         std::string( "SPECIFIED " ) + std::string( entry ) ) ;
    else if ( "pypars" == block ) 
      if      ( "mstp" == entry ) Pythia::pypars().mstp( int1 ) = int2 ;
      else if ( "msti" == entry ) Pythia::pypars().msti( int1 ) = int2 ;
      else if ( "parp" == entry ) Pythia::pypars().parp( int1 ) = fl1  ;
      else if ( "pari" == entry ) Pythia::pypars().pari( int1 ) = fl1  ; 
      else return Error( std::string( "PYTHIA ERROR, block PYPARS has MSTP," )+
                         std::string( "PARP, MSTI AND PARI: YOU HAVE " ) +
                         std::string( "SPECIFIED " ) + std::string( entry ) );
    else if ( "pydat1" == block ) 
      if      ( "mstu" == entry ) Pythia::pydat1().mstu( int1 ) = int2 ;
      else if ( "mstj" == entry ) Pythia::pydat1().mstj( int1 ) = int2 ;
      else if ( "paru" == entry ) Pythia::pydat1().paru( int1 ) = fl1  ; 
      else if ( "parj" == entry ) Pythia::pydat1().parj( int1 ) = fl1  ;
      else return Error( std::string( "PYTHIA ERROR, block PYDAT1  HAS ")+
                         std::string( "MSTU, MSTJ, PARU AND PARJ: YOU HAVE ") +
                         std::string( "SPECIFIED " ) + std::string( entry ) );
    else if ( "pydat2" == block ) 
      if      ( "kchg" == entry ) Pythia::pydat2().kchg( int1 , int2 ) = int3 ;
      else if ( "pmas" == entry ) Pythia::pydat2().pmas( int1 , int2 ) = fl2  ;
      else if ( "parf" == entry ) Pythia::pydat2().parf( int1 )        = fl1  ;
      else if ( "vckm" == entry ) Pythia::pydat2().vckm( int1 , int2 ) = fl2  ;
      else return Error( std::string("PYTHIA ERROR, block PYDAT2  HAS KCHG,")+
                         std::string("PMAS, PARF AND VCKM: YOU HAVE ")+
                         std::string("SPECIFIED " ) + std::string(entry )) ;
    else if ( "pydat3" == block ) 
      if      ( "mdcy" == entry ) 
        Pythia::pydat3().mdcy(Pythia::PyComp( int1 ) , int2 ) = int3 ; 
      else if ( "mdme" == entry ) Pythia::pydat3().mdme( int1 , int2 ) = int3 ;
      else if ( "brat" == entry ) Pythia::pydat3().brat( int1 )        = fl1  ;
      else if ( "kfdp" == entry ) Pythia::pydat3().kfdp( int1 , int2 ) = int3 ;
      else return Error( std::string("PYTHIA ERROR, block PYDAT3  HAS KFDP, ")+
                         std::string("MDCY, BRAT AND MDME : YOU HAVE " ) + 
                         std::string("SPECIFIED ") + std::string( entry ) ) ;
    else if ( "pydatr" == block ) 
      if      ( "mrpy" == entry ) Pythia::pydatr().mrpy( int1 ) = int2 ;
      else if ( "rrpy" == entry ) Pythia::pydatr().rrpy( int1 ) = fl1  ;
      else return Error( std::string( "PYTHIA ERROR, block PYDATR HAS MRPY " )+
                         std::string( "AND RRPY : YOU HAVE SPECIFIED " ) +
                         std::string( entry ) ) ;
    else if ( "pymssm" == block )
      if      ( "imss" == entry ) Pythia::pymssm().imss( int1 ) = int2 ;
      else if ( "rmss" == entry ) Pythia::pymssm().rmss( int1 ) = fl1  ;
      else return Error( std::string( "PYTHIA ERROR, block PYMSSM has IMSS " )+
                         std::string( "AND RMSS: YOU HAVE SPECIFIED " ) +
                         std::string( entry ) ) ;
    else if ( "pyint2" == block ) 
      if      ( "iset" == entry ) Pythia::pyint2().iset( int1 )        = int2 ;
      else if ( "kfpr" == entry ) Pythia::pyint2().kfpr( int1 , int2 ) = int3 ;
      else if ( "coef" == entry ) Pythia::pyint2().coef( int1 , int2 ) = fl2  ;
      else if ( "icol" == entry ) 
        Pythia::pyint2().icol( int1 , int2 , int3 ) = int4 ;
      else return Error( std::string( "PYTHIA ERROR, block PYINT2 has " ) +
                         std::string( "ISET KFPR COEF AND ICOL: YOU HAVE " ) +
                         std::string( "SPECIFIED ") + std::string( entry ) ) ;
    else return Error( std::string( " ERROR in PYTHIA PARAMETERS   " ) +
                       std::string( block ) +
                       std::string( " is and invalid common block name !" ) ) ;
  }
  
  return StatusCode::SUCCESS ;
}

//=============================================================================
// Print Pythia parameters
//=============================================================================
void PythiaProduction::printPythiaParameter( ) {  
  debug() << "****************************************************" << endmsg ;
  debug() << "**        Pythia parameters for this job            " << endmsg ;
  debug() << "**                                                  " << endmsg ;
  debug() << "** MSTP(2) ( alpha_s order )                      : " 
          << Pythia::pypars().mstp( 2 )   << endmsg ;
  debug() << "** MSTP(33) ( k factors )                         : "  
          << Pythia::pypars().mstp( 33 )  << endmsg ;
  debug() << "** MSTP(128) ( doc. lines )                       : " 
          << Pythia::pypars().mstp( 128 ) << endmsg ;
  debug() << "** MSTP(82) ( multiple interactions )             : "
          << Pythia::pypars().mstp( 82 )  << endmsg ;
  debug() << "** MSTP(52) ( PDF 1 = Pythia, 2 = PDFLIB )        : "  
          << Pythia::pypars().mstp( 52 )  << endmsg ;
  debug() << "** MSTP(51) ( PDF Function Code )                 : "
          << Pythia::pypars().mstp( 51 )  << endmsg ;
  debug() << "** PARP(82) ( scale for mult inter )              : "
          << Pythia::pypars().parp( 82 )  << endmsg ;
  debug() << "** PARP(89) ( reference energy scale )            : "
          << Pythia::pypars().parp( 89 )  << endmsg ;
  debug() << "** PARP(90) ( power of energy resc. term )        : " 
          << Pythia::pypars().parp( 90 )  << endmsg ;
  debug() << "** PARJ(1) : "
          << Pythia::pydat1().parj( 1 )  << endmsg ;
   debug() << "** PARJ(2) : "
          << Pythia::pydat1().parj( 2 )  << endmsg ;
 debug() << "** PARJ(3) : "
          << Pythia::pydat1().parj( 3 )  << endmsg ;
 debug() << "** PARJ(4) : "
        << Pythia::pydat1().parj( 4 )  << endmsg ;
 debug() << "** PARJ(5) : "
          << Pythia::pydat1().parj( 5 )  << endmsg ;
 debug() << "** PARJ(6) : "
          << Pythia::pydat1().parj( 6 )  << endmsg ;
 debug() << "** PARJ(7) : "
          << Pythia::pydat1().parj( 7 )  << endmsg ;
 debug() << "** PARJ(11) : "
          << Pythia::pydat1().parj( 11 )  << endmsg ;
 debug() << "** PARJ(12) : "
          << Pythia::pydat1().parj( 12 )  << endmsg ;
  debug() << "** PARJ(13) ( prob. that heavy meson has spin 1 ) : "
          << Pythia::pydat1().parj( 13 )  << endmsg ;
  debug() << "** PARJ(14) ( prob. for spin=0 meson to be produced" << endmsg ; 
  debug() << "**            with ang. mom= 1 if total spin=1 )  : "
          << Pythia::pydat1().parj( 14 )  << endmsg ;
  debug() << "** PARJ(15) ( prob. for spin=1 meson to be produced " << endmsg ;
  debug() << "**            with ang. mom= 1 if total spin=0 )  : "
          << Pythia::pydat1().parj( 15 )  << endmsg ;
  debug() << "** PARJ(16) ( prob. for spin=1 meson to be produced " << endmsg ;
  debug() << "**            with ang. mom= 1 if total spin=1 )  : "
          << Pythia::pydat1().parj( 16 )  << endmsg ;
  debug() << "** PARJ(17) ( prob. for spin=1 meson to be produced " << endmsg ;
  debug() << "**            with ang. mom= 1 if total spin=2 )  : "
          << Pythia::pydat1().parj( 17 )  << endmsg ;
  debug() << "** MSTJ(26) ( B0 mixing in Pythia, must be 0 )    : "
          << Pythia::pydat1().mstj( 26 )  << endmsg ;
  debug() << "** PARJ(33) ( cut off for fragmentation )         : "
          << Pythia::pydat1().parj( 33 )  << endmsg;
  debug() << "** PARP(141)                                      : " 
	  << Pythia::pypars().parp( 141 ) << endmsg ;
  debug() << "** PARP(142)                                      : " 
	  << Pythia::pypars().parp( 142 ) << endmsg ;
  debug() << "** PARP(143)                                      : " 
	  << Pythia::pypars().parp( 143 ) << endmsg ;
  debug() << "** PARP(144)                                      : " 
	  << Pythia::pypars().parp( 144 ) << endmsg ;
  debug() << "** PARP(145)                                      : " 
	  << Pythia::pypars().parp( 145 ) << endmsg ;
  debug() << "** PARP(146)                                      : " 
	  << Pythia::pypars().parp( 146 ) << endmsg ;
  debug() << "** PARP(147)                                      : " 
	  << Pythia::pypars().parp( 147 ) << endmsg ;
  debug() << "** PARP(148)                                      : " 
	  << Pythia::pypars().parp( 148 ) << endmsg ;
  debug() << "** PARP(149)                                      : " 
	  << Pythia::pypars().parp( 149 ) << endmsg ;
  debug() << "** PARP(150)                                      : " 
	  << Pythia::pypars().parp( 150 ) << endmsg ;
  debug() << "** MSTP(148)                                      : " 
	  << Pythia::pypars().mstp( 148 ) << endmsg ;
  debug() << "** MSTP(149)                                      : " 
	  << Pythia::pypars().mstp( 149 ) << endmsg ;
  debug() << "** MSTP(142)                                      : " 
	  << Pythia::pypars().mstp( 142 ) << endmsg ;
  debug() << "** MSEL ( processes )                             : "
          << Pythia::pysubs().msel()      << endmsg ;
  debug() << "** Activated processes                            : " << endmsg ;
  for ( int i = 1 ; i <= 500 ; ++i ) if ( 1 == Pythia::pysubs() . msub( i ) ) 
    debug() << "** --- Process " << i << endmsg ;
  debug() << "**                                                  " << endmsg ;
  debug() << "****************************************************" << endmsg ;
  
}

//=============================================================================
// Turn on fragmentation in Pythia
//=============================================================================
void PythiaProduction::turnOnFragmentation( ){
  Pythia::pydat1().mstj( 1 ) = 1 ;
}

//=============================================================================
// Turn off fragmentation in Pythia
//=============================================================================
void PythiaProduction::turnOffFragmentation( ){
  Pythia::pydat1().mstj( 1 ) = 0 ;
}

//=============================================================================
// Save parton event
//=============================================================================
void PythiaProduction::savePartonEvent( HepMC::GenEvent * /* theEvent */ ) {
  Pythia::PyEdit( 21 ) ;
}

//=============================================================================
// Load parton event
//=============================================================================
void PythiaProduction::retrievePartonEvent( HepMC::GenEvent * /* theEvent */ )
{
  Pythia::PyEdit( 22 ) ;
}

//=============================================================================
// Hadronize Pythia event
//=============================================================================
StatusCode PythiaProduction::hadronize( HepMC::GenEvent * theEvent , 
                                        LHCb::GenCollision * theCollision ) {
  Pythia::PyExec( ) ;
  
  // Debugging output: print each event if required
  if ( m_eventListingLevel2 >= 0 ) {
    const int mstu_1 = Pythia::pydat1().mstu(1) ;
    const int mstu_2 = Pythia::pydat1().mstu(2) ;
    Pythia::pydat1().mstu(1) = m_had_mstu_1 ;
    Pythia::pydat1().mstu(2) = m_had_mstu_2 ;
    Pythia::PyList ( m_eventListingLevel2 ) ;
    Pythia::pydat1().mstu(1) = mstu_1 ;
    Pythia::pydat1().mstu(2) = mstu_2  ;
  }

  return toHepMC ( theEvent , theCollision ) ;  
} ;


//=============================================================================
// Debug print out to be printed after all initializations
//=============================================================================
void PythiaProduction::printRunningConditions( ) 
{
  // PYLIST call is managed by Pythia job options
  if ( m_initializationListingLevel >= 0 ) {
    info() << " CALL PYLIST(" <<  m_initializationListingLevel << ") " 
           << " using MSTU(1/2)=" << m_ini_mstu_1 << "/" << m_ini_mstu_2 
           << endreq ;
    //
    const int mstu_1 = Pythia::pydat1().mstu(1) ;
    const int mstu_2 = Pythia::pydat1().mstu(2) ;
    //
    Pythia::pydat1().mstu(1) = m_ini_mstu_1 ;
    Pythia::pydat1().mstu(2) = m_ini_mstu_2 ; 
    Pythia::PyList( m_initializationListingLevel ) ;
    Pythia::pydat1().mstu(1) = mstu_1 ;
    Pythia::pydat1().mstu(2) = mstu_2 ; 
  }

  // print out Pythia settings
  printPythiaParameter() ;

  // print particle properties (if needed)
  for ( std::vector<int>::const_iterator ip = m_pdtlist.begin() ; 
        m_pdtlist.end() != ip ; ++ip ) {
    const int i = *ip ;
    if ( 0 > i ) { continue ; } 
    const int mstu_1 = Pythia::pydat1().mstu(1) ;
    const int mstu_2 = Pythia::pydat1().mstu(2) ;      
    Pythia::pydat1().mstu(1) = i ;
    Pythia::pydat1().mstu(2) = i ;
    Pythia::PyList( 12 ) ;      
    Pythia::pydat1().mstu(1) = mstu_1 ;
    Pythia::pydat1().mstu(2) = mstu_2 ; 
  }
  
  // write output decay table (if needed)
  if ( 0 != m_particleDataUnit && !m_particleDataOutput.empty() ) {
    StatusCode sc = F77Utils::open ( m_particleDataUnit , 
                                     m_particleDataOutput ) ;
    if ( sc.isFailure() ) { 
      Error ( "Could not open output PDS file '" + 
              m_particleDataOutput + "'" , sc ) ; 
      return ;
    }
    
    // update the table 
    info() << " CALL PYUPDA(1," 
           << m_particleDataUnit<<"/'" << m_particleDataOutput << "') " 
           << endreq ;
    Pythia::PyUpda( 1 , m_particleDataUnit ) ;
    // close the file 
    F77Utils::close ( m_particleDataUnit ) ;
    always() <<" Particle Data Table  has been dump to  the file '" 
             << m_particleDataOutput << "'" << endreq;
  }
}

//=============================================================================
// TRUE if the particle is a special particle which must not be modify
//=============================================================================
bool PythiaProduction::isSpecialParticle( const LHCb::ParticleProperty * thePP ) 
  const {
  switch ( thePP -> pid() . abspid() ) {
  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
  case 7:
  case 8:
  case 17:
  case 18:
  case 21:
  case 110:
  case 990:
    //case 32:
  case 33:
  case 34:
    //case 35:
    //case 36:
  case 37:
  case 39:
  case 41:
  case 42:
  case 43:
  case 44:
  case 81:
  case 82:
  case 83:
  case 84:
  case 85:
  case 88:
  case 89:
  case 90:
  case 91:
  case 92:
  case 93:
  case 94:
  case 95:
  case 96:
  case 97:
  case 98:
  case 99:
  case 9900110:
  case 9900210:
  case 9900220:
  case 9900330:
  case 9900440:
  case 9902110:
  case 9902210:
  case 1101:
  case 1103:
  case 2101:
  case 2103:
  case 2201:
  case 2203:
  case 3101:
  case 3103:
  case 3201:
  case 3203:
  case 3301:
  case 3303:
  case 4101:
  case 4103:
  case 4201:
  case 4203:
  case 4301:
  case 4303:
  case 4401:
  case 4403:
  case 5101:
  case 5103:
  case 5201:
  case 5203:
  case 5301:
  case 5303:
  case 5401:
  case 5403:
  case 5501:
  case 5503:
  case 1000022:
  case 1000024:
    return true ;
    break ;
  default:
    return false ;
    break ;
  }
  return false ;    
}
//=============================================================================
// Setup for forced fragmentation 
//=============================================================================
StatusCode PythiaProduction::setupForcedFragmentation( const int thePdgId ) {
  Pythia::pydat1().mstj( 30 ) = 1 ;
  Pythia::pydat1().mstj( 31 ) = 100 ;
  Pythia::pydat1().mstj( 33 ) = thePdgId ;
  Pythia::pydat1().mstu( 150 ) = 1 ;
  Pythia::pydat1().mstu( 151 ) = 0 ;
  Pythia::pydat1().mstu( 152 ) = 0 ;

  switch( thePdgId ) {
  case 511:
  case -511:
  case 521:
  case -521:
  case 531:
  case -531:
  case 541:
  case -541:
  case 551:
    Pythia::pydat1().mstj( 32 ) = - thePdgId / 100 ;
    Pythia::pydat1().mstj( 34 ) = ( thePdgId / 10 ) % 10 ;
    break ;
  case 5122:
  case -5122:
    Pythia::pydat1().mstj( 32 ) = thePdgId / 1000 ;
    Pythia::pydat1().mstj( 34 ) = thePdgId / abs( thePdgId ) * 2101 ;
    break ;
  default:
    error() << 
      format( "It is not possible to force fragmentation for particleID %d" ,
              thePdgId ) << endmsg ;
    return StatusCode::FAILURE ;
    break ;
  }

  return StatusCode::SUCCESS ;  
}
// ============================================================================
/// PYTHIA -> HEPEVT -> HEPMC 
// ============================================================================
StatusCode PythiaProduction::toHepMC
( HepMC::GenEvent*     theEvent    , 
  LHCb::GenCollision * theCollision )
{
  
  // Convert to HepEvt format
  Pythia::LunHep( 1 ) ;
  
  // Convert event in HepMC Format
  HepMC::IO_HEPEVT theHepIO ;
  
  // Force the verification of the HEPEVT  record 
  if ( m_validate_HEPEVT ) 
  { 
    theHepIO.set_trust_both_mothers_and_daughters ( true ) ;
    theHepIO.set_trust_mothers_before_daughters   ( true ) ;
    theHepIO.set_print_inconsistency_errors       ( false );
    if ( msgLevel( MSG::DEBUG ) ) 
      theHepIO.set_print_inconsistency_errors       ( true );
    
    if ( !m_inconsistencies.empty() && 0 == m_HEPEVT_errors )
    { m_HEPEVT_errors = new std::ofstream ( m_inconsistencies.c_str() ) ; }
    
    if ( 0 != m_HEPEVT_errors ) 
    {
      if ( !HepMC::HEPEVT_Wrapper::check_hepevt_consistency ( *m_HEPEVT_errors ) )
      { Warning ( "Inconsistencies in HEPEVT structure are found" ) ; }   
    }
    else 
    {
      MsgStream& log = warning() ;
      if ( !HepMC::HEPEVT_Wrapper::check_hepevt_consistency ( log.stream() ) )
      {
        log << endreq ;
        Warning ( "Inconsistencies in HEPEVT structure are found" ) ; 
      } 
    }
  }
  
  if ( ! theHepIO.fill_next_event( theEvent ) )
    return Error( "Could not fill HepMC event" ) ;
  
  // Now convert to LHCb units:
  for ( HepMC::GenEvent::particle_iterator p = theEvent -> particles_begin() ;
        p != theEvent -> particles_end() ; ++p ) {
    (*p) -> 
      set_momentum(
        HepMC::FourVector( (*p) -> momentum().px() * Gaudi::Units::GeV ,
                           (*p) -> momentum().py() * Gaudi::Units::GeV , 
                           (*p) -> momentum().pz() * Gaudi::Units::GeV , 
			   (*p) -> momentum().e() * Gaudi::Units::GeV ) ) ;
    // Since HepMC 2, the generated mass is also stored
    (*p) -> set_generated_mass( (*p)-> generated_mass() * Gaudi::Units::GeV ) ;
  }
  
  for ( HepMC::GenEvent::vertex_iterator v = theEvent -> vertices_begin() ;
        v != theEvent -> vertices_end() ; ++v ) {
    HepMC::FourVector newPos ;
    newPos.setX( (*v) -> position() . x() ) ;
    newPos.setY( (*v) -> position() . y() ) ;
    newPos.setZ( (*v) -> position() . z() ) ;
    newPos.setT( ( (*v) -> position() . t() * Gaudi::Units::mm ) 
                 / Gaudi::Units::c_light ) ;
    
    (*v) -> set_position( newPos ) ;
  }
  
  theEvent -> set_signal_process_id( Pythia::pypars().msti( 1 ) ) ;
  
  // Retrieve hard process information
  hardProcessInfo( theCollision ) ;
  
  return StatusCode::SUCCESS ;
} ;
// ============================================================================


// ============================================================================
/// The END 
// ============================================================================

